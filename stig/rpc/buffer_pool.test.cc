/* <stig/rpc/buffer_pool.test.cc> 

   Unit test for <stig/rpc/buffer_pool.h>.

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

#include <stig/rpc/buffer_pool.h>

#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rpc;

static constexpr size_t
    BufferCount      =  10,
    SmallRepeatCount =   3,
    ThreadCount      =   5,
    BigRepeatCount   =  10;

/* Counts the threads in each herd that locally pass their tests. */
static atomic_size_t PassCount;

/* A flag that each worker thread waits for, so they all start at once. */
static mutex Mutex;
static condition_variable GoIsSet;
static bool Go;

/* Each worker repeats its own alloc/free cycles multiple times and increments PassCount, above, iff. each cycle is ok. */
static void WorkerMain(TBufferPool &buffer_pool) {
  /* extra */ {
    /* Wait for it... */
    unique_lock<mutex> lock(Mutex);
    while (!Go) {
      GoIsSet.wait(lock);
    }
  }
  /* Go! */
  bool pass = true;
  for (size_t repeat = 0; pass && repeat < SmallRepeatCount; ++repeat) {
    for (size_t expected_buffer_count = 0; pass && expected_buffer_count <= BufferCount; ++expected_buffer_count) {
      /* Allocate some buffers, count them, and free them. */
      TBuffer *buffer = buffer_pool.AllocBuffers(expected_buffer_count);
      size_t actual_buffer_count = buffer ? buffer->GetBufferCount() : 0;
      buffer_pool.FreeBuffers(buffer);
      /* We pass iff. we got the number we asked for. */
      pass = (actual_buffer_count == expected_buffer_count);
    }
  }
  /* If every alloc/free cycle was ok, then this thread passes. */
  if (pass) {
    ++PassCount;
  }
}

FIXTURE(SingleThread) {
  bool pass = true;
  for (size_t repeat = 0; pass && repeat < BigRepeatCount; ++repeat) {
    for (size_t preallocate_count = 0; pass && preallocate_count <= BufferCount; ++preallocate_count) {
      /* Reset for this round of testing. */
      TBufferPool buffer_pool(preallocate_count);
      PassCount = 0;
      Go = false;
      /* Launch a herd of workers, each waiting for the go signal. */
      vector<thread> workers;
      for (size_t i = 0; i < ThreadCount; ++i) {
        workers.push_back(thread(WorkerMain, ref(buffer_pool)));
      }
      /* extra */ {
        /* Release the hounds! */
        unique_lock<mutex> lock(Mutex);
        Go = true;
        GoIsSet.notify_all();
      }
      /* Wait for all the workers to stop. */
      for (auto &worker: workers) {
        worker.join();
      }
      /* We pass iff. all threads passed. */
      pass = (PassCount == ThreadCount);
    }
  }
  EXPECT_TRUE(pass);
}

