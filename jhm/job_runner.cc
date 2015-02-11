/* <jhm/job_runner.cc>

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

#include <jhm/job_runner.h>

#include <jhm/file.h>
#include <jhm/job.h>

#include <base/subprocess.h>

#include <iostream>  // TODO(cmaloney): Less than ideal...

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace std::chrono;
using namespace Util;

TJobRunner::TResult::TResult(TJob *job,
                             int exit_code,
                             Base::TFd &&stdout,
                             Base::TFd &&stderr,
                             std::chrono::high_resolution_clock::duration run_time)
    : ExitCode(exit_code),
      Job(job),
      Stdout(std::move(stdout)),
      Stderr(std::move(stderr)),
      RunTime(run_time) {}

TJobRunner::TResult TJobRunner::TTask::operator()() const {
  if(PrintCmd) {
    std::cout << Base::AsStr(Base::Join(Cmd, ' ')) + '\n';
  }
  auto start = std::chrono::high_resolution_clock::now();
  auto subproc = Base::TSubprocess::New(*Pump, Cmd);
  int exit_code = subproc->Wait();
  auto duration = std::chrono::high_resolution_clock::now() - start;
  return TJobRunner::TResult(
      Job, exit_code, subproc->TakeStdOutFromChild(), subproc->TakeStdErrFromChild(), duration);
}

TJobRunner::TJobRunner(uint64_t worker_count, bool print_cmd)
    : WorkerPool(worker_count), PrintCmd(print_cmd) {}

bool TJobRunner::IsReady() const { return WorkerPool.IsReady(); }

void TJobRunner::Queue(TJob *job) {
  // Note: We call GetCmd() to make the string here because if we did it in the QueueRunner we'd
  // cross threads.
  WorkerPool.Queue({&Pump, PrintCmd, job, job->GetCmd()});
}

bool TJobRunner::HasMoreResults() const { return WorkerPool.HasMoreResults(); }

std::vector<TJobRunner::TResult> TJobRunner::WaitForResults() { return WorkerPool.Pop(); }
