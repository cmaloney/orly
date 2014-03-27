/* <base/pump.test.cc>

   Unit test for <base/pump.h>.

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

#include <base/pump.h>

#include <atomic>
#include <cstring>
#include <thread>
#include <vector>

#include <base/io_utils.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;

static const char *Msg = "Mofo the Psychic Gorilla!";

static const size_t MsgSize = strlen(Msg);

FIXTURE(OnePipeManyCycles) {
  const size_t cycle_repeat_count = 300;
  size_t i;
  for (i = 0; i < cycle_repeat_count; ++i) {
    const size_t
        msg_repeat_count = 300,
        expected_size = msg_repeat_count * MsgSize;
    size_t actual_size = 0;
    TPump pump;
    TFd in, out;
    pump.NewPipe(in, out);
    thread reader(
        [&actual_size, &in] {
          char buf[MsgSize / 3];
          for (;;) {
            ssize_t size;
            IfLt0(size = ReadAtMost(in, buf, sizeof(buf)));
            if (!size) {
              break;
            }
            actual_size += size;
          }
          in.Reset();
        }
    );
    thread writer(
        [msg_repeat_count, &out] {
          for (size_t i = 0; i < msg_repeat_count; ++i) {
            WriteExactly(out, Msg, MsgSize);
          }
          out.Reset();
        }
    );
    reader.join();
    writer.join();
    if (actual_size != expected_size) {
      break;
    }
  }
  EXPECT_EQ(i, cycle_repeat_count);
}

FIXTURE(OneCycleManyPipes) {
  TPump pump;
  atomic_size_t success_count(0);
  vector<thread> pipes;
  for (size_t i = 0; i < 300; ++i) {
    pipes.push_back(thread(
        [&pump, &success_count] {
          const size_t
              msg_repeat_count = 300,
              expected_size = msg_repeat_count * MsgSize;
          size_t actual_size = 0;
          TFd in, out;
          pump.NewPipe(in, out);
          thread reader(
              [&actual_size, &in] {
                char buf[MsgSize / 3];
                for (;;) {
                  ssize_t size;
                  IfLt0(size = ReadAtMost(in, buf, sizeof(buf)));
                  if (!size) {
                    break;
                  }
                  actual_size += size;
                }
                in.Reset();
              }
          );
          thread writer(
              [msg_repeat_count, &out] {
                for (size_t i = 0; i < msg_repeat_count; ++i) {
                  WriteExactly(out, Msg, MsgSize);
                }
                out.Reset();
              }
          );
          reader.join();
          writer.join();
          if (actual_size == expected_size) {
            ++success_count;
          }
        }
    ));
  }
  for (auto &pipe: pipes) {
    pipe.join();
  }
  EXPECT_EQ(success_count, pipes.size());
}

FIXTURE(WaitForIdle) {
  TPump pump;
  TFd in, out;
  pump.NewPipe(in, out);
  thread waiter(
      [&pump] {
        pump.WaitForIdle();
      }
  );
  in.Reset();
  out.Reset();
  waiter.join();
}

FIXTURE(WaitForIdleForSuccess) {
  TPump pump;
  TFd in, out;
  pump.NewPipe(in, out);
  atomic_bool
      is_ready(false),
      is_idle(false);
  thread waiter(
      [&pump, &is_ready, &is_idle] {
        while (!is_ready);
        is_idle = pump.WaitForIdleFor(milliseconds(100));
      }
  );
  in.Reset();
  out.Reset();
  is_ready = true;
  waiter.join();
  EXPECT_TRUE(is_idle);
}

FIXTURE(WaitForIdleForTimeout) {
  TPump pump;
  TFd in, out;
  pump.NewPipe(in, out);
  atomic_bool
      is_ready(false),
      is_idle(true);
  thread waiter(
      [&pump, &is_ready, &is_idle] {
        while (!is_ready);
        is_idle = pump.WaitForIdleFor(milliseconds(100));
      }
  );
  is_ready = true;
  waiter.join();
  EXPECT_FALSE(is_idle);
}

FIXTURE(WaitForIdleUntilSuccess) {
  TPump pump;
  TFd in, out;
  pump.NewPipe(in, out);
  atomic_bool
      is_ready(false),
      is_idle(false);
  thread waiter(
      [&pump, &is_ready, &is_idle] {
        while (!is_ready);
        is_idle = pump.WaitForIdleUntil(system_clock::now() + milliseconds(100));
      }
  );
  in.Reset();
  out.Reset();
  is_ready = true;
  waiter.join();
  EXPECT_TRUE(is_idle);
}

FIXTURE(WaitUntilIdleForTimeout) {
  TPump pump;
  TFd in, out;
  pump.NewPipe(in, out);
  atomic_bool
      is_ready(false),
      is_idle(true);
  thread waiter(
      [&pump, &is_ready, &is_idle] {
        while (!is_ready);
        is_idle = pump.WaitForIdleUntil(system_clock::now() + milliseconds(100));
      }
  );
  is_ready = true;
  waiter.join();
  EXPECT_FALSE(is_idle);
}
