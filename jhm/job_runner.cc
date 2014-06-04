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

#include <jhm/job.h>

#include <iostream>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace Util;

TJobRunner::TJobRunner(uint32_t worker_count, bool print_cmd)
    : ExitWorker(false),
      MoreResults(false),
      MoreResultsOnceTaken(false),
      ResultsReadySet(false),
      PrintCmd(print_cmd),
      WorkerCount(worker_count),
      QueueRunner(bind(&TJobRunner::ProcessQueue, this)) {}

TJobRunner::~TJobRunner() {
  ExitWorker = true;
  HasWork.notify_all();
  QueueRunner.join();
}

bool TJobRunner::IsReady() const {
  return !ExitWorker;
}

void TJobRunner::Queue(TJob *job) {
  lock_guard<mutex> lock(ToRunMutex);
  // Note: We call GetCmd() to make the string here because if we did it in the QueueRunner we'd cross threads.
  ToRun.emplace(job, job->GetCmd());
  HasWork.notify_all();

  if (!ExitWorker) {
    MoreResultsOnceTaken = true;
    MoreResults = true;
  }
}

bool TJobRunner::HasMoreResults() const {
  return MoreResults;
}

TJobRunner::TResults TJobRunner::WaitForResults() {
  assert(HasMoreResults());

  // Wait for results to become available
  auto future = ResultsReady.get_future();
  future.wait();

  lock_guard<mutex> lock(ResultsMutex);

  // Reset the promise
  ResultsReady = promise<bool>();
  ResultsReadySet = false;

  bool temp = MoreResultsOnceTaken;
  MoreResults = temp;

  // Move out the reults set while under the lock (We do this as an explicit step to ensure it happens under the lock)
  TResults ret = move(Results);

  // Return the results via NRVO.
  return ret;
}

void TJobRunner::ProcessQueue() {
  // Set of jobs currently being run
  unordered_map<int, TJob*> PidMap;
  unordered_map<TJob*, unique_ptr<TSubprocess>> Running;

  while(!ExitWorker || Running.size() > 0) {
    // Only queue work if we aren't in the process of spinning down
    if (!ExitWorker) {
      // If we're not fully booked, do more.
      auto to_start = WorkerCount - Running.size();
      if (to_start) {
        /* queue work until ToRun is empty */ {
          lock_guard<std::mutex> lock(ToRunMutex);
          while(Running.size() < WorkerCount && !ToRun.empty()) {
            // Get the job at the front of the queue
            TJob *job = nullptr;
            vector<string> cmd;
            tie(job, cmd) = Pop(ToRun);

            if (PrintCmd) {
              // NOTE: We use '+' to make a new string (effectively as a back buffer), then a single operation to write it out
              // This makes it so that the line never gets broken / split / etc. because of threading.
              //TODO: Join in a way that makes the difference between ' ' and passing the arguments
              // as an array more obvious.
              cout << AsStr(Join(cmd, ' ')) + '\n';
            }

            // Run the job
            auto subproc = TSubprocess::New(Pump, cmd);
            PidMap[subproc->GetChildId()] = job;
            auto res = Running.emplace(job, move(subproc));
            assert(res.second);
            MoreResultsOnceTaken = true;
            MoreResults = true;
          }
        }
      }

    }
    // If there's nothing running, that means we're out of work. Wait for work to come our way
    // TODO: Could the Queue notify_all ever miss waking this up?
    if(Running.size() == 0) {
      assert(!MoreResultsOnceTaken);
      unique_lock<mutex> lock(HasWorkMutex);
      HasWork.wait(lock);
    } else {
      // Wait for results from the queued jobs.
      auto pid = TSubprocess::WaitAll();
      TJob *job = PidMap.at(pid);
      EraseOrFail(PidMap, pid);
      auto &subproc = Running.at(job);
      // NOTE: the Wait() here should return instantly since we know the process already terminated by way of WaitAll()
      auto returncode = subproc->Wait();
      if (returncode != 0) {
        ExitWorker = true;
      }
      /* lock for results set */ {
        lock_guard<mutex> lock(ResultsMutex);
        // If this is our last result, mark it so.
        if (Running.size() == 1) {
          if (ExitWorker) {
            MoreResultsOnceTaken = false;
          } else {
            lock_guard<mutex> lock(ToRunMutex);
            if (ToRun.empty()) {
              MoreResultsOnceTaken = false;
            }
          }
        }
        Results.emplace_back(job, returncode, subproc->TakeStdOutFromChild(), subproc->TakeStdErrFromChild());
        if(!ResultsReadySet) {
          ResultsReadySet = true;
          ResultsReady.set_value(true);
        }
      }
      EraseOrFail(Running, job);
    }
  }
}