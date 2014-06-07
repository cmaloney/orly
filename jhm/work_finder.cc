/* <jhm/work_finder.h>

   Finds and runs all jobs necessary to create the needed files.

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
#include <base/split.h>
#include <base/stl_utils.h>
#include <base/thrower.h>
#include <jhm/file.h>
#include <jhm/job.h>
#include <starsha/status_line.h>

using namespace Base;
using namespace Jhm;
using namespace std;

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
  if (res && job) {
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

    /* check input */ {
      TFile *file = job->GetInput();
      needed += AddNeededFile(file, job);
    }

    for (TFile *file : job->GetNeeds()) {
      // Add each file. If any need jobs to complete, the job isn't ready yet.
      needed += AddNeededFile(file, job);
    }
    if (needed) {
      // TODO: Lots of copies (Although they're tiny)
      InsertOrFail(Waiting, make_pair(job, needed));
    } else {
      // If we're about to run the job, ensure the output directories for it exist
      // TODO: Move this to a more logical place.
      for (TFile *out : job->GetOutput()) {
        EnsureDirExists(out->GetPath().AsStr().c_str(), true);
      }
      Runner.Queue(job);
      InsertOrFail(Running, job);
    }
  }
}

bool TWorkFinder::ProcessResult(TJobRunner::TResult &result) {
  // Job finished / no longer running
  EraseOrFail(Running, result.Job);

  if (result.ExitCode != 0) {
    // TODO: Test if stdout, stderr have text before pritning label and text for each.
    cout << "ERROR: " << result.Job << " returned " << result.ExitCode << '\n' << "STDOUT: \n";
    Base::EchoOutput(move(result.Stdout));
    cout << "STDERR: \n";
    Base::EchoOutput(move(result.Stderr));
    return true;
  }
  // Check if the job is actually complete or not
  if (!result.Job->IsComplete()) {
    Ready.push(result.Job);
    return false;
  }

  TJson::TArray job_output;
  for(TFile *f: result.Job->GetOutput()) {
    job_output.push_back(f->GetPath().AsStr());
  }


  TJson job_info(TJson::TObject{
      {"build_info", TJson::TObject{
        {"job", TJson::TObject{
          {"name", result.Job->GetName()},
          {"input", result.Job->GetInput()->GetPath().AsStr()},
          {"output", job_output}
        }}
      }}});

  for (TFile *out_file : result.Job->GetOutput()) {
    // Sanity check that all output files now exist.
    if (!ExistsPath(out_file->GetPath().AsStr().c_str())) {
      THROW_ERROR(logic_error) << "Job " << result.Job << " Didn't produce the output file '" << out_file
                               << " which it was supposed to yet returned successfully...";
    }

    // Attach to each output file it's build info
    out_file->PushComputedConfig(TJson(job_info));
  }

  // Update every job which was waiting on this job to be waiting on one less thing.
  // NOTE: If the job isn't waiting on anything, it gets pushed to the Ready queue.
  const auto range = ToFinish.equal_range(result.Job);
  if (range.first != range.second) {
    for(auto iter = range.first; iter != range.second; ++iter) {
      // Update each job which was waiting on this job's waiting entry. Queue in ReadyJobs if needed.
      TJob *job = iter->second;
      assert(job);
      auto &count = Waiting.at(job);
      --count;
      assert(count >= 0);
      if (count == 0) {
        // Move the job to ready, as it has nothing left it's waiting on.
        EraseOrFail(Waiting, job);
        Ready.push(job);
      }
    }
    // TODO: Assert this succeeds (It should be guaranteed to)
    ToFinish.erase(range.first, range.second);
  }

  InsertOrFail(Finished, result.Job);
  return false;
}

bool TWorkFinder::IsBuildable(TFile *file) {
  return file->IsSrc() || TryGetProducer(file);
}

bool TWorkFinder::IsFileDone(TFile *file) {
  return file->IsSrc() || IsDone(TryGetProducer(file));
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
      for (TFile *f : job->GetOutput()) {
        if (!Producers.emplace(f, job).second) {
          if (Producers.at(f) != job) {
            THROW_ERROR(runtime_error) << "Multiple producers for file "
                                       << f <<  ". Producers: " << Producers.at(f) << ", " << job;
          }
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
  bool has_failed = false;
  // Continue as long as the runner is returning us more results still, or we have some jobs ready to be processed.
  while (Runner.HasMoreResults() || (!Ready.empty() && Runner.IsReady())) {
    // Either queue the job to run or find what it's depending upon / waiting on, and queue those to run.
    ProcessReady();

    // Everything should have been emptied from the ready queue
    assert(Ready.empty());

    // Something should be in the running queue at this point. Or we're just waiting for more results from the runner.
    assert(!Running.empty() || Runner.HasMoreResults());

    // Sanity check
    // If we haven't failed, then the running, ready, and finished sets should make up the 'all' set.
    // If we've failed 1+ job, then the number failed will be missing.
    if (!has_failed) {
      Starsha::TStatusLine() << '[' << Finished.size() << '/' << All.size() << "]. Waiting for cores: ["
                             << Running.size() << "].";
      assert(Running.size() + Waiting.size() + Finished.size() == All.size());
    }

    //DEBUG: cout << Running.size() << " + " << Waiting.size() << " + " << Finished.size() << " == " << All.size() << endl;
    //DEBUG: Base::Join(", ", Running, cout); cout << '\n';

    // Wait for 1+ results from the job runner. Process every result returned.
    for(auto &result: Runner.WaitForResults()) {
      has_failed |= ProcessResult(result);
    }
  }

  // Clear any remaining status line
  Starsha::TStatusLine::Cleanup();

  return !has_failed;
  // Find the jobs with no dependencies,)
}

bool TWorkFinder::IsDone(TJob *job) const {
  return Finished.find(job) != Finished.end();
}

bool TWorkFinder::Queue(TJob *job) {
  // We know about the job. Just look up its status and exit.
  if (Contains(All, job)) {
    return !IsDone(job);
  }

  // New job, add it.
  InsertOrFail(All, job);

  // Queue the job if it isn't done
  if (IsDone(job)) {
    InsertOrFail(Finished, job);
    return false;
  } else {
    Ready.push(job);
    return true;
  }
}