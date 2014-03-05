/* <base/event_counter.cc>

   Implements <base/event_counter.h>.

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

#include <base/event_counter.h>

#include <sys/eventfd.h>

#include <base/error_utils.h>

using namespace Base;

TEventCounter::TEventCounter(uint64_t initial_count)
    : Fd(eventfd(initial_count, 0)) {}


const TFd &TEventCounter::GetFd() const {
  assert(this);
  return Fd;
}

uint64_t TEventCounter::Pop() {
  assert(this);
  uint64_t count;
  IfLt0(eventfd_read(Fd, &count));
  return count;
}

void TEventCounter::Push(uint64_t count) {
  assert(this);
  IfLt0(eventfd_write(Fd, count));
}
