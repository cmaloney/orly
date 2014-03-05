/* <multi_event/multi_event.cc>

   Implements <multi_event/multi_event.h>.

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <multi_event/multi_event.h>

using namespace std;
using namespace MultiEvent;

TEvent::~TEvent() {}

void TEvent::Fire() {
  assert(this);
  lock_guard<mutex> lock(Mutex);
  if (!Flag) {
    Flag = true;
    for (auto waiter: Waiters) {
      waiter->Notify(this);
    }
  }
}

bool TWaiter::Wait(const TProducer &producer, const TConsumer &consumer, const Base::TOpt<chrono::milliseconds> &timeout) {
  assert(this);
  assert(&producer);
  assert(&consumer);
  assert(&timeout);
  Flag = false;
  try {
    /* Spin through our events. */
    producer([&consumer, this](const shared_ptr<TEvent> &event) {
      assert(&event);
      assert(event);
      lock_guard<mutex> lock(event->Mutex);
      if (event->Flag) {
        /* The event has already fired, so consume it immediately.
           This will also prevent us from collecting up any more events to wait for. */
        consumer(event);
        Flag = true;
      } else if (!Flag) {
        /* This event hasn't fired and we've not yet found any that have, so collect this event and let it know we'll be watching it. */
        Events.insert(event);
        event->Waiters.insert(this);
      }
    });
    if (!Flag) {
      /* We didn't find any events that were already fired, so it's time to wait.
         Notify() will set Flag if any event in Events fires. */
      unique_lock<mutex> lock(Mutex);
      if (timeout) {
        /* Wait for a while. */
        FlagIsSet.wait_for(lock, *timeout, [this] {
          return Flag;
        });
      } else {
        /* Wait forever. */
        FlagIsSet.wait(lock, [this] {
          return Flag;
        });
      }
      /* If didn't time out, spin through our events and consume any that have fired. */
      if (Flag) {
        for (auto event: Events) {
          lock_guard<mutex> lock(event->Mutex);
          if (event->Flag) {
            consumer(event);
          }
        }
      }
    }
  } catch (...) {
    Reset();
    throw;
  }
  Reset();
  return Flag;
}

void TWaiter::Notify(const TEvent *event) {
  assert(this);
  assert(event);
  unique_lock<mutex> lock(Mutex);
  Flag = true;
  FlagIsSet.notify_one();
}

void TWaiter::Reset() {
  assert(this);
  for (auto event: Events) {
    lock_guard<mutex> lock(event->Mutex);
    event->Waiters.erase(this);
  }
  Events.clear();
}
