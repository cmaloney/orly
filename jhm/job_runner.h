/* <jhm/job_runner.h>

   Manages a pool of subprocess running specific commands

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

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <tuple>

#include <base/class_traits.h>
#include <base/fd.h>
#include <base/subprocess.h>

namespace Jhm {

  class TJob;

  class TJobRunner {
    NO_MOVE(TJobRunner);
    NO_COPY(TJobRunner);

    public:
    class TResult {
      public:
      MOVE_ONLY(TResult);
      TResult(TJob *job, int exit_code, Base::TFd &&stdout, Base::TFd &&stderr)
          : ExitCode(exit_code), Job(job), Stdout(stdout), Stderr(stderr) {}

      int ExitCode;
      TJob *Job;
      Base::TFd Stdout, Stderr;
    };

    using TResults = std::vector<TResult>;

    public:
    TJobRunner(uint64_t worker_count, bool print_cmd);
    ~TJobRunner();

    bool IsReady() const;

    /* Adds a job to the queue of jobs to run. */
    void Queue(TJob *job);

    /* Returns true iff the job runner is guaranteed to have more results in a future WaitForResults call. */
    bool HasMoreResults() const;

    /* Grabs a batch of results from the queue runner. */
    TResults WaitForResults();

    private:

    void ProcessQueue();

    std::mutex ToRunMutex;
    std::queue<std::tuple<TJob*, std::vector<std::string>>> ToRun;
    // NOTE: ExitWorker only goes from false -> true on shutdown
    std::atomic<bool> ExitWorker;
    // NOTE: MoreResults is true so long as we're either processing, or going to process more jobs.
    // Becomes false only when we're completely exhausted of jobs to process.
    std::atomic<bool> MoreResults;

    // NOTE: We have two bools for the MoreResults, because we want to buffer the answer until WaitForResults() takes
    // the last results.
    std::atomic<bool> MoreResultsOnceTaken;

    // Mutex protected results vector.
    std::mutex ResultsMutex;
    TResults Results;

    std::promise<bool> ResultsReady;
    std::atomic<bool> ResultsReadySet;

    // Fires anytime the client should re-check it's main loop for work to do (ExitWorkers = true, or new work queued)
    std::mutex HasWorkMutex;
    std::condition_variable HasWork;

    // Sits in the background and pumps io from subprocesses into this process.
    Base::TPump Pump;

    // General parameters for the runner
    const bool PrintCmd;
    const uint64_t WorkerCount;

    // The thread which sits in the background and runs the queue.
    std::thread QueueRunner;
  };
}
