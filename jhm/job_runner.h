/* <jhm/job_runner.h>

   Manages a pool of subprocess running specific commands

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

#include <atomic>
#include <string>
#include <thread>
#include <tuple>

#include <base/class_traits.h>
#include <base/fd.h>
#include <base/subprocess.h>

#include <moodycamel/blockingconcurrentqueue.h>

namespace Jhm {

class TJob;

struct TJobRunner {
  struct TResult {
    MOVE_ONLY(TResult)
    TResult(TJob *job, int exit_code, Base::TFd &&stdout, Base::TFd &&stderr)
        : ExitCode(exit_code), Job(job), Stdout(std::move(stdout)), Stderr(std::move(stderr)) {}

    int ExitCode;
    TJob *Job;
    Base::TFd Stdout, Stderr;
  };

  using TRunnable = std::tuple<TJob *, std::vector<std::string>>;

  TJobRunner(uint64_t worker_count, bool print_cmd);
  ~TJobRunner();

  bool IsReady() const;

  /* Adds a job to the queue of jobs to run. */
  void Queue(TJob *job);

  /* Returns true iff the job runner is guaranteed to have more results in a future WaitForResults
   * call. */
  bool HasMoreResults() const;

  /* Grab a single result. */
  TResult WaitForResult();

  private:
  void ProcessQueue();
  void Shutdown();

  // Jobs in, results out.
  moodycamel::BlockingConcurrentQueue<TRunnable> ToRun;

  // TODO(cmaloney): The pointer indirection here is fugly.
  moodycamel::BlockingConcurrentQueue<std::unique_ptr<TResult>> Results;

  // Sits in the background and pumps io from subprocesses into this process.
  Base::TPump Pump;

  uint64_t InQueue = 0;
  std::atomic<uint64_t> BeingRun;

  // General parameters for the runner
  bool ExitWorker = false;
  const bool PrintCmd;

  // Background job runners
  std::vector<std::unique_ptr<std::thread>> JobRunners;
};
}
