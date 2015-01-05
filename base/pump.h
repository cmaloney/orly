/* <base/pump.h>

   A pump for pipes.

   Problems can sometimes arise with pipes when the reader is not responding quickly
   enough.  The writer becomes blocked when the pipe's internal buffer fills and this
   can lead to deadlocks when the reader and writer are synchronizing with each other.

   This pump seeks to alleviate the problem by reading avidly from the pipe and storing
   the results in. The pump runs a single background thread which can handle the traffic
   of many separate pipes.

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

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_set>

#include <base/class_traits.h>
#if defined __APPLE__  // TPumper implementations
#include <base/pump_kqueue.h>
#elif defined __linux__
#include <base/pump_epoll.h>
#else
#error Not Implemented
#endif
#include <base/pump_util.h>
#include <base/fd.h>
#include <util/stl.h>

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

namespace Pump {

// TODO: this should be pulled out into a generic "pool of buffers" implementation
// NOTE: We never reclaim any blocks currently.
// TODO: Should really be an invasive containment type thing between pools and blocks so blocks
// can't get lost.
template <uint64_t BlockSize>
class TGrowingPool {
  NO_COPY(TGrowingPool);

  public:
  using TPtr = std::unique_ptr<uint8_t[]>;

  TGrowingPool() = default;

  /* Get a new block (The block continues to be owned by the pool. */
  uint8_t *Allocate() {
    if(!AvailBlocks.empty()) {
      return Util::Pop(AvailBlocks);
    }

    TPtr block(new uint8_t[BlockSize]);
    auto ret = block.get();
    Blocks.emplace_back(std::move(block));
    return ret;
  }

  /* Return a block to the pool */
  void Recycle(uint8_t *block) {
    assert(block);
    AvailBlocks.push(block);
  }

  private:
  std::queue<uint8_t *> AvailBlocks;
  std::vector<TPtr> Blocks;
};

/* Max number of bytes which are forwarded at a time through a TPipe */
static constexpr uint64_t ReadBufSize = 4096;

class TPipe;

}  // Pump

/* A pump for pipes. */
class TPump final {
  NO_COPY(TPump);

  public:
  /* Construct with no pipes. */
  TPump();

  /* Destroy all pipes and lose all unread data. */
  ~TPump();

  /* Create a new pipe and return both ends of it. */
  void NewPipe(TFd &read, TFd &write);

  /* Wait for the pump to become idle. */
  void WaitForIdle() const {
    assert(this);
    std::unique_lock<std::mutex> lock(PipeMutex);
    while(!IsIdle()) {
      PipeDied.wait(lock);
    }
  }

  bool IsIdle() const;

  private:
  bool ServicePipe(Pump::TPipe *pipe);

  using TBlockPool = Pump::TGrowingPool<Pump::ReadBufSize>;

  /* Signals when a pipe dies.  This wakes up the WaitForIdle() functions. */
  mutable std::mutex PipeMutex;
  mutable std::condition_variable PipeDied;

  /* Collection of the owned pipes.

     NOTE: It would be really nice to use unique_ptr here, but we can't because we can't lookup a
     pipe given a raw
     pointer later... */
  std::unordered_set<Pump::TPipe *> Pipes;

  Pump::TPumper Pumper;

  /* Pool of blocks for stashing I/O in temporarily. */
  TBlockPool BlockPool;

  friend class Pump::TPipe;
  friend class Pump::TPumper;
};  // TPump

}  // Base
