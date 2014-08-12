/* <base/shutting_down.cc>

   Implements <base/shutting_down.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/shutting_down.h>

#include <atomic>
#include <functional>
#include <map>
#include <mutex>

#include <base/opt.h>
#include <util/stl.h>

using namespace Base;
using namespace std;

/* If shutting_down is true, returns true iff this is the thread that first set shut down.
   If shutting_down is false, returns the value of the internal ShuttingDown flag. */
static bool SetOrGetShuttingDown(bool shutting_down) {
  static atomic<bool> ShuttingDown(false);

  if (shutting_down) {
    return !ShuttingDown.exchange(shutting_down);
  }
  return ShuttingDown;
}

typedef function<void ()> TShutdownListener;
typedef map<uint64_t, TShutdownListener> TListenerCollection;

/* We keep track of the shutdown listeners by an id so we can add/remove easily with an RAII wrapper. */
static uint64_t ManipulateShutdownListeners(TShutdownListener new_listener,
                                     Base::TOpt<uint64_t> remove_id,
                                     const function<void(TShutdownListener)> &for_each_listener) {
  static TListenerCollection Listeners;
  static uint64_t NextListenerId=1; // We start at 1 so we can use 0 as "no id removed".
  static recursive_mutex Mutex;

  lock_guard<recursive_mutex> lock(Mutex);
  if (remove_id) {
    Util::EraseOrFail(Listeners, *remove_id);
  }
  if (for_each_listener) {
    for(auto &listener: Listeners) {
      for_each_listener(listener.second);
    }
  }
  if (new_listener) {
    uint64_t new_id = NextListenerId;
    ++NextListenerId;
    Listeners.emplace(new_id, move(new_listener));
    return new_id;
  }
  return 0;
}

void Base::ShutDown() {
  if (SetOrGetShuttingDown(true)) {
    // Notify all listeners
    ManipulateShutdownListeners(TShutdownListener(), Base::TOpt<uint64_t>(), [](TShutdownListener cb) {
      cb();
    });
  }
}

bool Base::IsShuttingDown() {
  return SetOrGetShuttingDown(false);
}

TShutdownCallback::TShutdownCallback(TShutdownListener callback)
    : Id(ManipulateShutdownListeners(move(callback), Base::TOpt<uint64_t>(), function<void(TShutdownListener)>())) {}

TShutdownCallback::~TShutdownCallback() {
  ManipulateShutdownListeners(nullptr, Id, function<void(TShutdownListener)>());
}