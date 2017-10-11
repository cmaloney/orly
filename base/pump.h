/* <base/pump.h>

   A pump for pipes.

   Problems can sometimes arise with pipes when the reader is not responding quickly
   enough.  The writer becomes blocked when the pipe's internal buffer fills and this
   can lead to deadlocks when the reader and writer are synchronizing with each other.

   This pump seeks to alleviate the problem by reading avidly from the pipe and storing
   the results in memory. The pump runs a single background thread which can handle the
   traffic of many separate pipes.

   The pump is backed by a infinitely growing collection of pages. This will consume ram
   infinitely until we run out of space. Care should be taken not to have this happen.

   When the time comes to shut down the pump, you have two choices: shut it down nicely,
   waiting for any pending data to drain through, or shut it down hard, throwing away
   any data which hasn't yet been read.

   For a nice shutdown, use WaitForIdle().  This will block until the pump has delivered
   all pending data.  If other threads continue to create pipes during this time, the
   wait may never be satisfied, so be sure you take steps to prevent this from happening.

   For a hard shutdown, just destroy the pump.  The destructor will not return until the
   background thread has halted.

   Sample Usage:
    TPump pump;
    TFd read, write;
    pump.NewPipe(read, write);
    WriteExactly(write, "test", 4);

    char data[4];
    ReadExactly(read, data, 4);

    EXPECT_EQ(data[0], 'd');
    EXPECT_EQ(data[1], 'a');
    EXPECT_EQ(data[2], 't');
    EXPECT_EQ(data[3], 'a');

  TODO:
    PipeMutex is overly coarse and will become contentious with lots of adding/removing.

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

#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <unordered_set>


#include <base/class_traits.h>
#include <base/cyclic_buffer.h>
#include <base/notify_fd.h>
#include <base/fd.h>

/* Pumper abstract interface

  // Pumps data abstracting out platform specific behavior.
  class TPumper {

    enum TEvent {
      Read,
      Write
    };

    // Add the given fd to the epoll with the given events and associate the given pipe with it.
    void Join(int fd, TEvent event, TPipe *pipe);

    // Remove the given fd from the epoll.
    void Leave(int fd);

    // Pumps data until told to shutdown. Launch this in a backtround
    // thread. All other member functions re synchronized properly with this.
    void Pump();

  };
*/

namespace Base {

/* A pump for pipes. */
class TPump final {
  NO_COPY(TPump)

  public:
  enum class TPipeDirection { WriteToBuffer, ReadFromBuffer };

  /* Arbitrary fixed max size to make adding/removing pipes cheaper / easier. */
  static constexpr uint64_t MaxPipes = 512;

  /* Construct with no pipes. */
  TPump();

  /* Destroy all pipes and lose all unread data. */
  ~TPump();

  // Create an fd which when read pulls data from the given cyclic buffer.
  TFd NewReadFromBuffer(std::shared_ptr<TCyclicBuffer> &source);

  // Create an fd which when written puts data into the given cyclic buffer.
  std::tuple<TFd, std::future<void>> NewWriteToBuffer(std::shared_ptr<TCyclicBuffer> &target);

  std::future<void> AddPipe(TPipeDirection, TFd &&fd, std::shared_ptr<TCyclicBuffer> &buffer);

  /* Wait for the pump to become idle. */
  void WaitForIdle() const;

  private:
  class TPipe;
  public:

  // TODO(cmaloney): Only public so a helper function is happy...
  // TODO(cmaloney): This should use a more generic event wrapper / libevent
  class TPumper {
    public:

    enum TEvent { Read, Write };

    /* Max number of epoll events returned simultaneously */
    static const size_t MaxEventCount = 64;

    TPumper(TPump &pump);
    void Join(int fd, TEvent event_type, TPipe *pipe);
    void Leave(int fd, TEvent event_type);

    // Returns after the background thread has shut down.
    void Shutdown();

    private:
    void BackgroundMain();

    // TODO(cmaloney): pump really shouldn't be needed.
    TPump &Pump;
    /* Pushed in the destructor.  It causes the background thread to exit. */
    TNotifyFd ShutdownFd;
    TFd Fd;
    std::thread Background;
  };

  private:
  bool ServicePipe(TPipe *pipe);

  /* Signals when a pipe dies.  This wakes up the WaitForIdle() functions. */
  mutable std::mutex PipeMutex;
  mutable std::condition_variable PipeDied;

  /* Collection of the owned pipes.

     NOTE: It would be really nice to use unique_ptr here, but we can't because we can't lookup a
     pipe given a raw
     pointer later... */
  // TODO(cmaloney): Make this resizable
  std::array<std::atomic<TPipe *>, MaxPipes> Pipes;

  TPumper Pumper;
};  // TPump

}  // Base
