/* <base/pump.test.cc>

   Unit test for <base/pump.h>.

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/pump.h>

#include <atomic>
#include <cstring>
#include <thread>
#include <vector>

#include <util/io.h>

#include <test/kit.h>

using namespace Base;
using namespace std;
using namespace std::chrono;
using namespace Util;

static const char *Msg = "Mofo the Psychic Gorilla!";

static constexpr size_t MsgSize = 25;

FIXTURE(OnePipeManyCycles) {
  assert(strlen(Msg) == MsgSize);
  const size_t cycle_repeat_count = 300;
  size_t i;
  for(i = 0; i < cycle_repeat_count; ++i) {
    const size_t msg_repeat_count = 300, expected_size = msg_repeat_count * MsgSize;
    ssize_t actual_size = 0;
    TPump pump;
    TFd read, write;
    pump.NewPipe(read, write);
    assert(read);
    assert(write);
    thread reader([&actual_size, &read] {
      char buf[MsgSize / 3];
      for(;;) {
        ssize_t size;
        size = IfLt0(ReadAtMost(read, buf, sizeof(buf)));
        if(!size) {
          break;
        }
        actual_size += size;
      }
      read.Reset();
    });
    thread writer([msg_repeat_count, &write] {
      for(size_t msg_idx = 0; msg_idx < msg_repeat_count; ++msg_idx) {
        WriteExactly(write, Msg, MsgSize);
      }
      write.Reset();
    });
    reader.join();
    writer.join();
    if(actual_size != expected_size) {
      break;
    }
  }
  EXPECT_EQ(i, cycle_repeat_count);
}

FIXTURE(OneCycleManyPipes) {
  TPump pump;
  atomic_size_t success_count(0);
  vector<thread> pipes;
  for(size_t i = 0; i < 300; ++i) {
    pipes.push_back(thread([&pump, &success_count, i] {
      const size_t msg_repeat_count = 300, expected_size = msg_repeat_count * MsgSize;
      ssize_t actual_size = 0;
      TFd read, write;
      pump.NewPipe(read, write);
      thread reader([&actual_size, &read] {
        char buf[MsgSize / 3];
        for(;;) {
          ssize_t size;
          IfLt0(size = ReadAtMost(read, buf, sizeof(buf)));
          if(!size) {
            break;
          }
          actual_size += size;
        }
        read.Reset();
      });
      thread writer([msg_repeat_count, &write] {
        for(size_t msg_idx = 0; msg_idx < msg_repeat_count; ++msg_idx) {
          WriteExactly(write, Msg, MsgSize);
        }
        write.Reset();
      });
      reader.join();
      writer.join();
      if(actual_size == expected_size) {
        ++success_count;
      }
    }));
  }
  for(auto &pipe : pipes) {
    pipe.join();
  }
  EXPECT_EQ(success_count, pipes.size());
}

FIXTURE(WaitForIdle) {
  TPump pump;
  TFd read, write;
  pump.NewPipe(read, write);
  thread waiter([&pump] { pump.WaitForIdle(); });
  read.Reset();
  write.Reset();
  waiter.join();
}
