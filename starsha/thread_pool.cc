/* <starsha/thread_pool.cc>

   Implements <starsha/thread_pool.h>.

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

#include <starsha/thread_pool.h>

#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>

using namespace std;
using namespace Starsha;

TThreadPool::~TThreadPool() {
  assert(this);
  Stop();
}

bool TThreadPool::ForEachErrorMsg(const function<bool (const string &, const string &)> &cb) {
  assert(this);
  assert(&cb);
  bool result, is_empty;
  TErrorMsg error_msg;
  for (;;) {
    /* extra */ {
      unique_lock<mutex> lock(Mutex);
      is_empty = ErrorMsgQueue.empty();
      if (!is_empty) {
        error_msg = ErrorMsgQueue.front();
        ErrorMsgQueue.pop();
      }
    }
    if (is_empty) {
      result = true;
      break;
    }
    if (!cb(error_msg.first, error_msg.second)) {
      result = false;
      break;
    }
  }
  return result;
}

void TThreadPool::Push(TJob &&job) {
  assert(this);
  unique_lock<mutex> lock(Mutex);
  JobQueue.push(move(job));
  WorkerIsNeeded.notify_one();
}

void TThreadPool::Start(size_t worker_count) {
  assert(this);
  assert(worker_count);
  Stop();
  try {
    unique_lock<mutex> lock(Mutex);
    IdleWorkerCount = 0;
    StopFlag = false;
    Workers.resize(worker_count);
    for (size_t i = 0; i < worker_count; ++i) {
      Workers[i] = make_shared<thread>(&TThreadPool::WorkerMain, this);
    }
  } catch (...) {
    Stop();
    throw;
  }
  WaitUntilIdle();
}

void TThreadPool::Stop() {
  assert(this);
  /* extra */ {
    unique_lock<mutex> lock(Mutex);
    StopFlag = true;
    WorkerIsNeeded.notify_all();
  }
  for (shared_ptr<thread> &worker: Workers) {
    if (worker) {
      worker->join();
    }
  }
  Workers.clear();
}

void TThreadPool::WaitUntilIdle() {
  assert(this);
  unique_lock<mutex> lock(Mutex);
  static bool is_real_tty = isatty(STDOUT_FILENO);
  static size_t last_size = 0;
  chrono::milliseconds wait_time{100};
  while (!JobQueue.empty() || IdleWorkerCount < Workers.size()) {
    size_t new_size = JobQueue.size();
    if(last_size != new_size) {
      last_size = new_size;
      if(is_real_tty) {
        cout<<"\r\e[K";
      }
      cout<<'['<<JobQueue.size()<<"] Jobs queued in wave";
      if(is_real_tty) {
        cout<<flush;
      } else {
        cout<<'\n';
      }
    }
    WorkerIsIdle.wait_for(lock, wait_time);
  }
}

void TThreadPool::WorkerMain() {
  assert(this);
  try {
    /* The main loop of the worker.  Loop here until we're told to stop. */
    TErrorMsg error_msg;
    bool error_flag = false;
    for (;;) {
      /* Wait until there's a job to do or until we're told to stop.
         If we're told to stop, exit the work loop. */
      TJob job;
      if (!WorkerTryPop(job, error_flag ? &error_msg : 0)) {
        break;
      }
      /* We have a job to do, so do it.  If we get an error, capture the message so we can push it when we loop back. */
      try {
        (job.second)();
        error_flag = false;
      } catch (const exception &ex) {
        error_msg.first = job.first;
        error_msg.second = ex.what();
        error_flag = true;
      } catch (...) {
        error_msg.first = job.first;
        error_msg.second = "non-standard exception";
        error_flag = true;
      }
    }  // for (;;)
  } catch (...) {
    /* Any exception which escapes the above try-block will abort the whole process.
       This is by design.  We're catching and reporting job-related exceptions.
       Anything else that goes wrong is in the 'catastrophe' range and should shut us down. */
    abort();
  }
}

bool TThreadPool::WorkerTryPop(TJob &job, const TErrorMsg *prev_error_msg) {
  assert(this);
  assert(&job);
  bool result;
  unique_lock<mutex> lock(Mutex);
  /* If we've carried in an error message from our last iteration, push it onto the error message queue. */
  if (prev_error_msg) {
    ErrorMsgQueue.push(*prev_error_msg);
  }
  /* Loop here until we're told to stop or until there's a job in the queue. */
  for (;;) {
    /* Is there a job in the queue? */
    if (!JobQueue.empty()) {
      job = JobQueue.front();
      JobQueue.pop();
      result = true;
      break;
    }
    /* Is it time to stop? */
    if (StopFlag) {
      result = false;
      break;
    }
    /* Go idle and wait until we're needed. */
    ++IdleWorkerCount;
    WorkerIsIdle.notify_one();
    WorkerIsNeeded.wait(lock);
    /* We've been awakened.  Stop being idle and loop back to see what's to be done. */
    --IdleWorkerCount;
  }
  return result;
}
