/* <stig/rpc/safe_transceiver.test.cc> 

   Unit test for <stig/rpc/safe_transceiver.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/safe_transceiver.h>

#include <string>
#include <thread>

#include <base/error_utils.h>
#include <base/event_semaphore.h>
#include <base/fd.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig::Rpc;

FIXTURE(Typical) {
  /* Construct a semaphore and a local socket pair and a tranceiver which will use them. */
  TEventSemaphore sem;
  TFd sock_a, sock_b;
  TFd::SocketPair(sock_a, sock_b, AF_UNIX, SOCK_STREAM, 0);
  TSafeTransceiver xver(sem.GetFd());
  /* Send a message in two pieces. */
  string expected("Hello, world!");
  size_t
      size = expected.size(),
      half = size / 2;
  char *data = const_cast<char *>(expected.data());
  auto *vecs = xver.GetIoVecs(2);
  vecs[0].iov_base = data;
  vecs[0].iov_len  = half;
  vecs[1].iov_base = data + half;
  vecs[1].iov_len  = size - half;
  xver.Send(sock_a);
  /* Receive the message in one piece. */
  char actual[size];
  vecs = xver.GetIoVecs(1);
  vecs[0].iov_base = actual;
  vecs[0].iov_len  = size;
  xver.Recv(sock_b);
  /* Did we get it? */
  EXPECT_EQ(string(actual, size), expected);
}

static const char
    *InterruptedMsg = "interrupted",
    *TimedOutMsg    = "timed out";

static void Reader(int interrupt_fd, int sock_fd, string &data) {
  try {
    /* Make a xver and arm a short deadline. */
    TSafeTransceiver xver(interrupt_fd);
    xver.SetDeadlineInterval(chrono::milliseconds(50));
    xver.ArmDeadline();
    /* Try to receive some data and return it in the out-param. */
    char buf[5];
    auto vecs = xver.GetIoVecs(1);
    vecs[0].iov_base = buf;
    vecs[0].iov_len  = sizeof(buf);
    xver.Recv(sock_fd);
    data.assign(buf, sizeof(buf));
  } catch (const TSafeTransceiver::TInterrupted &) {
    data = InterruptedMsg;
  } catch (const TSafeTransceiver::TTimedOut &) {
    data = TimedOutMsg;
  } catch (const exception &ex) {
    data = ex.what();
  }
}

FIXTURE(Normal) {
  /* Set up to transceive. */
  TEventSemaphore sem;
  TFd reader_sock, writer_sock;
  TFd::SocketPair(reader_sock, writer_sock, AF_UNIX, SOCK_STREAM, 0);
  /* Have some data reader for the reader. */
  IfLt0(send(writer_sock, "hello", 5, MSG_NOSIGNAL));
  /* Run the reader. */
  string data;
  thread(Reader, static_cast<int>(sem.GetFd()), static_cast<int>(reader_sock), ref(data)).join();
  EXPECT_EQ(data, "hello");
}

FIXTURE(Interrupted) {
  /* Set up to transceive. */
  TEventSemaphore sem;
  TFd reader_sock, writer_sock;
  TFd::SocketPair(reader_sock, writer_sock, AF_UNIX, SOCK_STREAM, 0);
  /* Have an interruption reader for the reader. */
  sem.Push();
  /* Run the reader. */
  string data;
  thread(Reader, static_cast<int>(sem.GetFd()), static_cast<int>(reader_sock), ref(data)).join();
  EXPECT_EQ(data, InterruptedMsg);
}

FIXTURE(TimedOut) {
  /* Set up to transceive. */
  TEventSemaphore sem;
  TFd reader_sock, writer_sock;
  TFd::SocketPair(reader_sock, writer_sock, AF_UNIX, SOCK_STREAM, 0);
  /* Run the reader, knowing there's nothing to read and no interruption coming. */
  string data;
  thread(Reader, static_cast<int>(sem.GetFd()), static_cast<int>(reader_sock), ref(data)).join();
  EXPECT_EQ(data, TimedOutMsg);
}

