/* <server/half_latch.cc>

   Implements <server/half_latch.h>.

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

#include <server/half_latch.h>

#include <unistd.h>

#include <base/os_error.h>

using namespace Base;
using namespace Server;

THalfLatch::THalfLatch()
    : IsSet(false) {
  int fds[2];
  TOsError::IfLt0(HERE, pipe(fds));
  RecvFd = TFd(fds[0]);
  SendFd = TFd(fds[1]);
}

void THalfLatch::Recv() {
  assert(this);
  char dummy;
  TOsError::IfLt0(HERE, read(RecvFd, &dummy, 1));
  IsSet = false;
}

void THalfLatch::Send() {
  assert(this);
  if (!IsSet) {
    TOsError::IfLt0(HERE, write(SendFd, ".", 1));
    IsSet = true;
  }
}
