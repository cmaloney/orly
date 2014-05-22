/* <jhm/work_finder.h>

   Finds and then runs all the work needed to get out all needed files.

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

#include <jhm/work_finder.h>

#include <iostream>
#include <stdexcept>

#include <base/path_utils.h>
#include <base/stl_utils.h>
#include <base/subprocess.h>
#include <base/thrower.h>
#include <jhm/file.h>
#include <jhm/job.h>

using namespace Base;
using namespace Jhm;
using namespace std;

TJobRunner::TJobRunner(uint32_t worker_count)
    : WorkerCount(worker_count), QueueRunner(bind(&TJobRunner::Worker, this)) {}

TJobRunner::~TJobRunner() {
  ExitWorker = true;
  NewWork.notify_one();
  QueueRunner.join();
}

bool TJobRunner::HasWork() const {
  //TODO: Does this need the lock?
  lock_guard<mutex> lock(ToRunMutex);
  return !ToRun.empty();
}

void TJobRunner::Queue(TJob *job) {
  lock_guard<mutex> lock(ToRunMutex);
  ToRun.push(job);
  NewWork.notify_one();
}

TJobRunner::TResults TJobRunner::WaitForResults() {
  /* cv */ {
    unique_lock<mutex> lock(NewResultsMutex);
    NewResults.wait(lock);
  }

  lock_guard<mutex> lock(ResultsMutex);
  // Move out the results.
  TResults ret = move(Results);

  return ret;
}

void TJobRunner::Worker() {
  //NOTE: we keep two maps because we want to go from subprocess -> job. We also only get to know the first pid to return...
  unordered_map<int, TJob*> PidMap;
  unordered_map<TJob*, unique_ptr<TSubprocess>> Running;
  //NOTE: We wait for running to empty always whe emptying the queue.
  while(!ExitWorker || !Running.empty()) {
    auto free_cores = WorkerCount - Running.size();
    // Queue more work until we're out of cores (Unless we've been told to stop)
    // If there's no work to queue and nothing is running, wait for something to finish.
    if (!ExitWorker) {
      if (free_cores == WorkerCount) {
        Working = false; // we're out of work
        NewResults.notify_all();
        unique_lock<mutex> lock(NewWorkMutex);
        NewWork.wait(lock);
      }

      //NOTE: This has a seperate ExitWorker test because NewWork fires when ExitWorker goes from false -> true.
      if (free_cores > 0 && !ExitWorker) {
        Working = true;
        lock_guard<mutex> lock(ToRunMutex);
        while (free_cores > 0 && !ToRun.empty()) {
          TJob *job = Pop(ToRun);
          auto subproc = TSubprocess::New(pump, job->GetCmd().c_str());
          PidMap[subproc->GetChildId()] = job;
          auto res = Running.emplace(job, move(subproc));
          assert(res.second);
        }
      }
    }

    // Wait for work to complete
    if (!Running.empty()) {
      auto pid = TSubprocess::WaitAll();
      TJob *job = PidMap.at(pid);
      EraseOrFail(PidMap, pid);
      auto &subproc = Running.at(job);
      //NOTE: the subproc.Wait() here shouldn't hang, since we already found it to have terminated in the WaitAll()
      auto returncode = subproc->Wait();
      /* lock */ {
        lock_guard<mutex> lock(ResultsMutex);
        Results.emplace_back(job, returncode, subproc->TakeStdOutFromChild(), subproc->TakeStdErrFromChild());
        NewResults.notify_all();
      }
      EraseOrFail(Running, job);

      // If we exited successfully, all output files should now exist.
      // TODO: Write the check that jobs shouldn't have made/updated output.
      if (returncode == 0) {
        // Sanity check all outputs exist for job
        for(const TFile *out_file : job->GetOutput()) {

          if (!ExistsPath(out_file->GetPath().AsStr().c_str())) {
            THROW_ERROR(logic_error) << "Job " << job << " Didn't produce the output file '" << out_file
                                     << " which it was supposed to yet returned successfully...";
          }
        }
      }
    }
  }

  Working = false;

  NewResults.notify_all();

  assert(PidMap.empty());
  assert(Running.empty());
}

bool TWorkFinder::AddNeededFile(TFile *file, TJob *job) {
  // If the file both doesn't exist and isn't buildable, error.
  if (file->IsSrc()) {
    return false;
  }

  // Ensure the file has a producer
  TJob *producer = TryGetProducer(file);
  if (!producer) {
    THROW_ERROR(runtime_error) << "No known way to produce file " << file;
  }

  // Queue the producer job as necessary to finish.
  bool res = Queue(producer);
  if (res) {
    //TODO: Lots of copies (Although they're tiny)
    ToFinish.emplace(producer, job);
  }
  return res;
}

void TWorkFinder::ProcessReady() {
      while (!Ready.empty()) {
      // For each ReadyJob, see if there are any deps we now need.
      uint64_t needed = 0;

      TJob *job = Pop(Ready);

      for (TFile *file : job->GetNeeds()) {
        // Add each file. If any need jobs to complete, the job isn't ready yet.
        needed += AddNeededFile(file, job);
      }
      if (needed) {
        //TODO: Lots of copies (Although they're tiny)
        InsertOrFail(Waiting, make_pair(job, needed));
      } else {
        Runner.Queue(job);
      }
    }
}

void TWorkFinder::ProcessResult(TJobRunner::TResult &result) {
  if (result.ExitCode != 0) {
    cout << "ERROR: " << result.Job << " returned " << result.ExitCode << '\n' << "STDOUT: \n";
    Base::EchoOutput(move(result.Stdout));
    cout << "\n\nSTDERR: \n";
    Base::EchoOutput(move(result.Stderr));
  } else {
    const auto &range = ToFinish.equal_range(result.Job);
    for(auto iter = range.first; iter != range.second; ++iter) {
      // Update each job which was waiting on this job's waiting entry. Queue in ReadyJobs if needed.
      TJob *job = iter->second;
      auto &count = Waiting.at(job);
      --count;
      assert(count >= 0);
      if (count == 0) {
        // Move the job to waiting, as it has nothing left it's waiting on.
        EraseOrFail(Waiting, job);
        Ready.push(job);
      }
    }

    // TODO: Assert this succeeds (It should be guaranteed to)
    ToFinish.erase(range.first, range.second);
  }
}

bool TWorkFinder::IsBuildable(TFile *file) {
  return file->IsSrc() || TryGetProducer(file);
}

TJob *TWorkFinder::TryGetProducer(TFile *file) {
  if (file->IsSrc()) {
    return nullptr;
  }

  /* If there's already a producer, don't search. */ {
    auto elem = TryFind(Producers, file);
    if(elem) {
      return *elem;
    }
  }

  TJob *found_producer = nullptr;
  for (TJob *job: GetJobsProducingFile(file)) {
    TFile *input_file = job->GetInput();

    if (IsBuildable(input_file)) {
      bool found_self = false;
      //TODO: Collect / hold errors at this loop, re throw after last iteration.
      for(TFile *f: job->GetOutput()) {
        if(!Producers.emplace(f, job).second) {
          THROW_ERROR(runtime_error) << "Multiple producers for file" << file;
        }
        found_self |= f == file;
      }
      assert(found_self && "Make sure we found ourselves");

      // Note that we found a valid producer so we can return the producer.
      found_producer = job;
    }
  }

  // Check that we actually found a valid producer
  // TODO: Could
  return found_producer;
}

bool TWorkFinder::FinishAll() {
  // Try running each job, process OnCmdComplete callbacks. Each time a job looks ready, ask what it needs as deps.
  // If it doesn't need any more dependencies, queue it as an OnCmdComplete.

  // As long as there is work to do, or the subprocess runner has more results for us.
  while (!ToFinish.empty() && (Runner.HasWork() || !Ready.empty())) {
    ProcessReady();

    // Everything should have been emptied from the ready queue
    assert(Ready.empty());

    // If nothing has been queued by here as valid/good work, there's an issue / we'll wait forever...
    assert(Runner.HasWork());

    // Sanity check Ready, Finished, Waiting.
    // Ready + Waiting + Finished == All
    assert(Ready.size() + Waiting.size() + Finished.size() == All.size());
    // TODO: Assert Ready, Waiting, and Finished don't share any items.

    // TODO: The subprocess runner needs to automatically stop queueing things on error.
    // TODO: We should make WaitForResults just return the result set for us.
    // Wait for 1+ of the subprocesses to return results we can process
    while (Runner.IsWorking()) {
      for(auto &result: Runner.WaitForResults()) {
        ProcessResult(result);
      }
    }
  }

  return Runner.GetNumberJobsFailed() > 0;
  // Find the jobs with no dependencies,)
}

bool TWorkFinder::IsDone(TJob *job) const {
  assert(All.find(job) != All.end());

  return Finished.find(job) != Finished.end();
}

bool TWorkFinder::Queue(TJob *job) {
  // We know about the job. Just look up its status and exit.
  if (Contains(All, job)) {
    return IsDone(job);
  }

  // New job, add it.
  InsertOrFail(All, job);

  // Queue the job if it isn't done
  if (IsDone(job)) {
    InsertOrFail(Finished, job);
    return true;
  } else {
    Ready.push(job);
    return false;
  }
}