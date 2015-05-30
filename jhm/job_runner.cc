/* <jhm/job_runner.cc>

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

#include <jhm/job_runner.h>

#include <jhm/file.h>
#include <jhm/job.h>

#include <iostream>  // TODO(cmaloney): Less than ideal...

using namespace Base;
using namespace Jhm;
using namespace std;

TJobRunner::TJobRunner(uint64_t worker_count, bool print_cmd) : BeingRun(0), PrintCmd(print_cmd) {
  JobRunners.reserve(worker_count);
  for(uint64_t i = 0; i < worker_count; ++i) {
    JobRunners.push_back(
        make_unique<thread>(bind(&TJobRunner::ProcessQueue, this)));
  }
}

TJobRunner::~TJobRunner() {
  Shutdown();
  for(auto &thread_ptr : JobRunners) {
    thread_ptr->join();
    thread_ptr.reset();
  }
}

bool TJobRunner::IsReady() const { return !ExitWorker; }

void TJobRunner::Queue(TJob *job) {
  // We use null jobs to indicate "end of queue" and terminate processing.
  // So passing in a null job is really bad.
  assert(job);

  // Note: We call GetCmd() to make the string here because if we done
  // in the QueueRunner we'd cross data ownership in threads.
  ++InQueue;
  ToRun.enqueue(make_tuple(job, job->GetCmd()));
}

bool TJobRunner::HasMoreResults() const { return (!ExitWorker ? InQueue : uint64_t(BeingRun)) > 0; }

TJobRunner::TResult TJobRunner::WaitForResult() {
  assert(HasMoreResults());

  std::unique_ptr<TResult> result_ptr;
  Results.wait_dequeue(result_ptr);
  --BeingRun;
  --InQueue;

  TResult result = std::move(*result_ptr.get());

  return result;
}

void TJobRunner::ProcessQueue() {
  // Pop a single job off the ToRun queue, run it, repeat ad-nauesum.
  // if
  TRunnable runnable;
  for(;;) {
    ToRun.wait_dequeue(runnable);

    // nullptr job indicates it is time to exit.
    if(!get<0>(runnable)) {
      assert(get<1>(runnable).size() == 0);
      break;
    }

    // The job is now being run and will definitely produce a result.
    ++BeingRun;

    // Run the job.
    Run(runnable);
    const auto &cmd = get<1>(runnable);
    if(PrintCmd) {
      // NOTE: We use '+' to make a new string (effectively as a back buffer), then a single
      // operation to write it out
      // This makes it so that the line never gets broken / split / etc. because of
      // threading.
      // TODO: Join in a way that makes the difference between ' ' and passing the arguments
      // as an array more obvious.
      cout << AsStr(Join(cmd, ' ')) + '\n';
    }

    auto subprocess = TSubprocess::New(Pump, cmd);
    int returncode = subprocess->Wait();
    Results.enqueue(std::make_unique<TResult>(get<0>(runnable), returncode, subprocess->TakeStdOutFromChild(),
                            subprocess->TakeStdErrFromChild()));
    // If the return code is non-zero exit all the workers
    if(returncode != 0) {
      Shutdown();
      break;
    }
  }
}

void TJobRunner::Shutdown() {
  ExitWorker = true;
  // Push one null job per worker onto the queue so that all the processors will
  // definitely see them.
  const auto null_job = make_tuple(nullptr, vector<string>());
  for(uint64_t i = 0; i < JobRunners.size(); ++i) {
    ToRun.enqueue(null_job);
  }
}
