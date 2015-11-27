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

#include <unistd.h>

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

FIXTURE(SingleActorManyCycles) {
  assert(strlen(Msg) == MsgSize);
  const size_t cycle_repeat_count = 300;
  size_t i;

  for (i = 0; i < cycle_repeat_count; ++i) {
    const size_t msg_repeat_count = 1000, expected_size = msg_repeat_count * MsgSize;
    ssize_t actual_size = 0;
    TPump pump;

    // Make sure the test won't overflow the cyclic buffers.
    static_assert(msg_repeat_count * MsgSize < TCyclicBuffer::MaxBlocks * TCyclicBuffer::BlockSize,
                  "Messages need to fit in a cyclic buffer without overflowing");

    // Write the data into the read buffer, construct pipes to pass the data.
    auto read_buffer = make_shared<TCyclicBuffer>();
    auto write_buffer = make_shared<TCyclicBuffer>();

    for (size_t j = 0; j < msg_repeat_count; ++j) {
      read_buffer->Write(Msg, MsgSize);
    }

    EXPECT_EQ(read_buffer->GetBytesAvailable(), MsgSize * msg_repeat_count);

    TFd write_to;
    std::future<void> write_to_finished;

    TFd read_from = pump.NewReadFromBuffer(read_buffer);
    tie(write_to, write_to_finished) = pump.NewWriteToBuffer(write_buffer);

    assert(read_from);
    assert(write_to);

    thread actor([&actual_size, &read_from, &write_to] {
      // Shorter than message to help tease out off by one bugs.
      char buf[MsgSize / 3];
      for (;;) {
        // Read out from the buffer
        ssize_t size;
        size = IfLt0(ReadAtMost(read_from, buf, sizeof(buf)));
        assert(size >= 0);
        if (!size) {
          break;
        }

        // Copy the message into the destination.
        WriteExactly(write_to, buf, size_t(size));

        actual_size += size;
      }
      read_from.Reset();
      write_to.Reset();
    });

    // Join the thread / wait for completion.
    write_to_finished.wait();
    actor.join();

    // Make sure the right number of bits, right values were written.
    EXPECT_EQ(actual_size, ssize_t(expected_size));
    EXPECT_EQ(write_buffer->GetBytesAvailable(), expected_size);

    // TODO(cmaloney): Assert write_buffer contains the message written msg_repeat_count times.
  }
  EXPECT_EQ(i, cycle_repeat_count);
}

FIXTURE(ManyActorsOnceCycle) {
  TPump pump;
  atomic_size_t read_source_success_count(0);
  atomic_size_t read_destination_success_count(0);
  atomic_size_t write_success_count(0);
  vector<thread> pipes;
  for (size_t i = 0; i < 300; ++i) {
    pipes.push_back(thread([&pump, &read_source_success_count, &write_success_count,
                            &read_destination_success_count, i] {
      const size_t msg_repeat_count = 300, expected_size = msg_repeat_count * MsgSize;
      ssize_t actual_size = 0;

      auto read_buffer = make_shared<TCyclicBuffer>();
      auto write_buffer = make_shared<TCyclicBuffer>();

      for (size_t j = 0; j < msg_repeat_count; ++j) {
        read_buffer->Write(Msg, MsgSize);
      }

      TFd write_to;
      std::future<void> write_to_finished;

      TFd read_from = pump.NewReadFromBuffer(read_buffer);
      tie(write_to, write_to_finished) = pump.NewWriteToBuffer(write_buffer);

      assert(write_to);
      assert(write_to_finished.valid());
      assert(read_from);

      thread reader([&actual_size, &read_from] {
        char buf[MsgSize / 3];
        for (;;) {
          // Read out from buffer
          ssize_t size;
          IfLt0(size = ReadAtMost(read_from, buf, sizeof(buf)));
          assert(size >= 0);
          if (!size) {
            break;
          }

          actual_size += size;
        }
        read_from.Reset();
      });
      thread writer([msg_repeat_count, &write_to] {
        for (size_t msg_idx = 0; msg_idx < msg_repeat_count; ++msg_idx) {
          WriteExactly(write_to, Msg, MsgSize);
        }
        write_to.Reset();
      });
      reader.join();
      writer.join();

      write_to_finished.wait();
      pump.WaitForIdle();

      if (expected_size == write_buffer->GetBytesAvailable()) {
        ++write_success_count;
      } else {
        // DEBUG: cout << AsStr(expected_size, " != ", write_buffer->GetBytesAvailable(), '\n');
      }

      if (0 == read_buffer->GetBytesAvailable()) {
        ++read_source_success_count;
      }

      if (actual_size == expected_size) {
        ++read_destination_success_count;
      }
    }));
  }
  for (auto &pipe : pipes) {
    pipe.join();
  }
  EXPECT_EQ(read_source_success_count, pipes.size());
  // EXPECT_EQ(read_destination_success_count, pipes.size());
  EXPECT_EQ(write_success_count, pipes.size());
}

FIXTURE(WaitForIdle) {
  TPump pump;

  auto read_buffer = make_shared<TCyclicBuffer>();
  auto write_buffer = make_shared<TCyclicBuffer>();
  TFd read_from = pump.NewReadFromBuffer(read_buffer);

  TFd write_to;
  std::future<void> write_to_finished;
  tie(write_to, write_to_finished) = pump.NewWriteToBuffer(write_buffer);

  thread waiter([&pump] { pump.WaitForIdle(); });

  read_from.Reset();
  write_to.Reset();
  waiter.join();
}
