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

#include <chrono>
#include <iostream>
#include <string>

#include <base/as_str.h>
#include <base/class_traits.h>
#include <base/fd.h>
#include <base/split.h>
#include <base/subprocess.h>
#include <base/worker_pool.h>

namespace Jhm {

class TJob;

class TJobRunner {
  NO_MOVE(TJobRunner);
  NO_COPY(TJobRunner);

  public:
  TJobRunner(uint64_t worker_count, bool print_cmd);
  ~TJobRunner();

  struct TResult {
    public:
    MOVE_ONLY(TResult);
    TResult(TJob *job,
            int exit_code,
            Base::TFd &&stdout,
            Base::TFd &&stderr,
            std::chrono::high_resolution_clock::duration run_time);

    int ExitCode;
    TJob *Job;
    Base::TFd Stdout, Stderr;
    std::chrono::high_resolution_clock::duration RunTime;
  };

  bool IsReady() const;

  /* Adds a job to the queue of jobs to run. */
  void Queue(TJob *job);

  /* Returns true iff the job runner is guaranteed to have more results in a future WaitForResults
   * call. */
  bool HasMoreResults() const;

  /* Grabs a batch of results from the queue runner. */
  std::vector<TResult> WaitForResults();

  private:
  // TODO(cmaloney): remove the need for a make_dep_file binary by changing this to be able to
  // operate
  // on a lambda.
  // As well as say "When you get back to main thread, here is the result of this computation"
  //   - Will save a __ton__ of I/O of writing a dep file to disk then reading it immediately.
  struct TTask {
    TResult operator()() const;

    Base::TPump *Pump;
    bool PrintCmd;
    TJob *Job;
    std::vector<std::string> Cmd;
  };

  // Processes tasks in the background.
  Base::TWorkerPool<TTask> WorkerPool;

  // Sits in the background and pumps io from subprocesses into this process.
  Base::TPump Pump;

  // General parameters for the runner
  const bool PrintCmd;
};
}
