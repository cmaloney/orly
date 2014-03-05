/* <base/latch.test.cc>

   Unit test for <base/latch.h>.

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

#include <base/latch.h>

#include <thread>

#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(Basics) {
  int request, reply;
  TLatch<int, int> latch;
  // start off empty
  EXPECT_FALSE(latch.GetRequestIsReadyFd().IsReadable());
  EXPECT_FALSE(latch.GetReplyIsReadyFd().IsReadable());
  // push a request
  latch.PushRequest(101);
  EXPECT_TRUE(latch.GetRequestIsReadyFd().IsReadable());
  EXPECT_FALSE(latch.GetReplyIsReadyFd().IsReadable());
  // pop the request
  latch.PopRequest(request);
  EXPECT_FALSE(latch.GetRequestIsReadyFd().IsReadable());
  EXPECT_FALSE(latch.GetReplyIsReadyFd().IsReadable());
  EXPECT_EQ(request, 101);
  // push a reply
  latch.PushReply(202);
  EXPECT_FALSE(latch.GetRequestIsReadyFd().IsReadable());
  EXPECT_TRUE(latch.GetReplyIsReadyFd().IsReadable());
  // pop the reply
  latch.PopReply(reply);
  EXPECT_EQ(reply, 202);
  EXPECT_FALSE(latch.GetRequestIsReadyFd().IsReadable());
  EXPECT_FALSE(latch.GetReplyIsReadyFd().IsReadable());
}

static void AdderMain(TLatch<int, int> &latch) {
  int request, reply = 0;
  for (;;) {
    latch.TransactReply(request, reply);
    if (request < 0) {
      break;
    }
    reply = request + 1;
  }
}

FIXTURE(Transaction) {
  int result;
  TLatch<int, int> latch;
  auto adder = thread(AdderMain, ref(latch));
  latch.TransactRequest(result, 11);
  EXPECT_EQ(result, 0);
  latch.TransactRequest(result, 22);
  EXPECT_EQ(result, 12);
  latch.TransactRequest(result, -1);
  EXPECT_EQ(result, 23);
  adder.join();
}

static void PongerMain(TLatch<void, void> &latch, bool &flag) {
  flag = true;
  latch.TransactReply();
}

FIXTURE(VoidTransaction) {
  TLatch<void, void> latch;
  bool flag = false;
  auto ponger = thread(PongerMain, ref(latch), ref(flag));
  latch.TransactRequest();
  ponger.join();
  EXPECT_TRUE(flag);
}
