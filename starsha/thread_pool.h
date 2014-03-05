/* <starsha/thread_pool.h>

   TODO

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

#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <base/no_copy_semantics.h>

namespace Starsha {

  /* TODO */
  class TThreadPool {
    NO_COPY_SEMANTICS(TThreadPool);
    public:

    /* TODO */
    typedef std::pair<std::string, std::function<void ()>> TJob;

    /* TODO */
    TThreadPool() {}

    /* TODO */
    explicit TThreadPool(size_t worker_count) {
      Start(worker_count);
    }

    /* TODO */
    ~TThreadPool();

    /* TODO */
    bool ForEachErrorMsg(const std::function<bool (const std::string &job, const std::string &msg)> &cb);

    /* TODO */
    void Push(TJob &&job);

    /* TODO */
    void Start(size_t worker_count);

    /* TODO */
    void Stop();

    /* TODO */
    void WaitUntilIdle();

    private:

    /* TODO */
    typedef std::pair<std::string, std::string> TErrorMsg;

    /* TODO */
    void WorkerMain();

    /* TODO */
    bool WorkerTryPop(TJob &job, const TErrorMsg *prev_error_msg);

    /* TODO */
    size_t IdleWorkerCount, StopFlag;

    /* TODO */
    std::mutex Mutex;

    /* TODO */
    std::condition_variable WorkerIsNeeded, WorkerIsIdle;

    /* TODO */
    std::queue<TJob> JobQueue;

    /* TODO */
    std::vector<std::shared_ptr<std::thread>> Workers;

    /* TODO */
    std::queue<TErrorMsg> ErrorMsgQueue;

  };  // TThreadPool

}  // Starsha
