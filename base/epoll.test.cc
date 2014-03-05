/* <base/epoll.test.cc>

   Unit test for <base/epoll.h>.

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

#include <base/epoll.h>

#include <base/event_semaphore.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(Typical) {
  TEventSemaphore s1, s2;
  TEpoll e;
  e.Add(s1.GetFd());
  e.Add(s2.GetFd());
  EXPECT_FALSE(e.GetFd().IsReadable());
  s1.Push();
  EXPECT_TRUE(e.GetFd().IsReadable());
  if (EXPECT_EQ(e.Wait(1, 0), 1u)) {
    int fd, flags;
    e.GetEvent(0, fd, flags);
    EXPECT_TRUE(fd == s1.GetFd());
    EXPECT_TRUE(flags == EPOLLIN);
  }
  EXPECT_TRUE(e.GetFd().IsReadable());
  s1.Pop();
  EXPECT_FALSE(e.GetFd().IsReadable());
  s1.Push();
  s2.Push();
  EXPECT_TRUE(e.GetFd().IsReadable());
  EXPECT_EQ(e.Wait(2, 0), 2u);
}
