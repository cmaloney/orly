/* <base/event_semaphore.test.cc>

   Unit test for <base/event_semaphore.h>.

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

#include <test/kit.h>

using namespace Base;

FIXTURE(Typical) {
  static const uint64_t actual_count = 3;
  TEventSemaphore sem;
  EXPECT_FALSE(sem.GetFd().IsReadable());
  sem.Push(actual_count);
  for (uint64_t i = 0; i < actual_count; ++i) {
    EXPECT_TRUE(sem.GetFd().IsReadable());
    sem.Pop();
  }
  EXPECT_FALSE(sem.GetFd().IsReadable());
}

FIXTURE(Nonblocking) {
  static const uint64_t actual_count = 3;
  TEventSemaphore sem(0, true);
  EXPECT_FALSE(sem.GetFd().IsReadable());
  EXPECT_FALSE(sem.Pop());
  EXPECT_FALSE(sem.GetFd().IsReadable());
  sem.Push(actual_count);
  for (uint64_t i = 0; i < actual_count; ++i) {
    EXPECT_TRUE(sem.GetFd().IsReadable());
    EXPECT_TRUE(sem.Pop());
  }
  EXPECT_FALSE(sem.GetFd().IsReadable());
  EXPECT_FALSE(sem.Pop());
}