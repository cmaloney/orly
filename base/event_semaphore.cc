/* <base/event_semaphore.cc>

   Implements <base/event_semaphore.h>.

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

#include <base/event_semaphore.h>

#include <cerrno>

#include <sys/eventfd.h>
#include <unistd.h>

#include <base/error_utils.h>
#include <base/io_utils.h>

using namespace Base;

TEventSemaphore::TEventSemaphore(uint64_t initial_count, bool nonblocking)
    : Fd(eventfd(initial_count, EFD_SEMAPHORE)) {
  if (nonblocking) {
    SetNonBlocking(Fd);
  }
}

bool TEventSemaphore::Pop() {
  assert(this);
  uint64_t dummy;
  ssize_t ret = read(Fd, &dummy, sizeof(dummy));

  if (ret < 0) {
    if (errno == EAGAIN) {
      /* The nonblocking option was passed to the constructor, and the
         semaphore was unavailable when we tried to do the pop. */
      return false;
    }

    IfLt0(ret);  // this will throw
  }

  return true;
}

void TEventSemaphore::Push(uint64_t count) {
  assert(this);
  IfLt0(eventfd_write(Fd, count));
}
