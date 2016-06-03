#include <bit/produce.h>

#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include <base/not_implemented.h>
#include <base/split.h>
#include <base/thrower.h>
#include <bit/environment.h>
#include <bit/file_type.h>
#include <bit/job_runner.h>
#include <bit/status_line.h>
#include <cmd/util.h>
#include <util/path.h>
#include <util/stl.h>

using namespace Base;
using namespace Bit;
using namespace Cmd;
using namespace std;
using namespace Util;

// NOTE: When a job is in the run queue, it absolutely must never be manipulated
// or have any methods called by the StatusTracker. Any/all calls are likely
// to result in data races.

class TStatusTracker {
  public:
  TStatusTracker(TEnvironment &environment, uint64_t worker_count)
      : Environment(environment), Runner(worker_count, &environment.Files) {}

  bool HasFinishedAll() const { return All.size() == Done.size(); }

  bool IsBuildable(TFileInfo *file, TFileType file_type);

  // TODO(cmaloney): The recursion / relation between AddNeeded
  // and IsBuildable isn't very nice. Would be nice if we could lay it out more
  // cleanly esp. since every output is known in advance nowadays.
  // Returns true if the file isn't complete.
  bool AddNeeded(TFileInfo *file);

  // Try to advance all tracked jobs / files to completion. Exits when either
  // there is a failure / error or all jobs have completed. Call
  // HasFinishedAll() to check if all jobs have been completed. In the case of
  // error will print all the error state and then throw a TErrorExit exception
  // to terminate the program.
  void DoAdvance();

  private:
  // Queue the given job based on its state.
  // If the job is already being run, no-op (The jobs is running, will be checked based on its
  // output).
  // If all needs are complete, mark the job as running and queue it.
  // Since all needs aren't complete, queue the jobs needed to complete those
  // files.
  void QueueJob(TJob *job);

  // Try getting the job which produces the given file. Retunrs nullptr if it
  // can't get the job.
  TJob *TryGetProducer(TFileInfo *file, TFileType file_type);

  TEnvironment &Environment;

  // TODO(cmaloney): on top of queued and done we need to be able to track things
  // that could be "optimistically checked"
  std::unordered_set<TJob *> Running;
  std::unordered_set<TJob *> Done;
  std::unordered_set<TJob *> All;

  std::unordered_map<TJob *, std::unordered_set<TJob *>> WaitingForJob;
  std::unordered_map<TFileInfo *, TJob *> Producers;
  TJobRunner Runner;
};

bool TStatusTracker::IsBuildable(TFileInfo *file, TFileType file_type) {
  bool is_buildable = file->IsComplete() || TryGetProducer(file, file_type);
  file->SetIsBuildable(is_buildable);
  return is_buildable;
}

// TODO(cmaloney): Make this an always succeed + throw on not able to produce
bool TStatusTracker::AddNeeded(TFileInfo *file) {
  if (file->IsComplete()) {
    return false;
  }

  // Try finding a producer of the file which is producable.
  TJob *job = TryGetProducer(file, TFileType::Unset);
  if (!job) {
    // TODO(cmaloney): Capture this exception and wrap it with the job which needed this file
    // to be produced.
    THROWER(runtime_error) << "No known way to produce file '" << file->RelPath.Path << "'.";
  }

  QueueJob(job);
  return true;
}

void TStatusTracker::DoAdvance() {
  while (Runner.HasMoreResults() && !HasFinishedAll()) {
    // Update current status
    TStatusLine() << '[' << Done.size() << '/' << All.size() << "] waiting: " << Running.size();

    // Get result out of runner
    TJobRunner::TResult result = Runner.WaitForResult();

    // These would only be unset if the result was moved from.
    assert(result.Output);
    assert(result.Job);

    // Job is no longer running
    EraseOrFail(Running, result.Job);

    const TJob::TOutput &output = *(result.Output);

    auto block_if_needs = [this, &result](const TJob::TNeeds &needs) {
      std::unordered_set<TJob *> blocking_jobs;

      auto add_blocking_not_complete = [this, &blocking_jobs](TFileInfo *file_info) {
        assert(file_info);

        if (AddNeeded(file_info)) {
          // TryGetProducer will always return true if AddNeeded returns true
          // then the file is not complete / being produced by something.
          TJob *producer = TryGetProducer(file_info, TFileType::Unset);
          assert(producer);
          blocking_jobs.insert(producer);
        }
      };

      for (TFileInfo *file_info : needs.Mandatory) {
        add_blocking_not_complete(file_info);
      }

      for (TFileInfo *file_info: needs.IfBuildable) {
        if (IsBuildable(file_info, TFileType::Unset)) {
          add_blocking_not_complete(file_info);
        }
      }

      // Mark this job as waiting for all the blocking jobs
      for (TJob *blocker : blocking_jobs) {
        // Default construct the waiting set if it isn't constructed and insert the
        // new waiting.
        WaitingForJob[blocker].insert(result.Job);
      }

      return !blocking_jobs.empty();
    };

    auto mark_complete = [this, &result, &output]() {
      // NOTE: this uses multiple loops over the same set to enusre each
      // operation completes fully / for each file before the next is started.

      // Sanity check that the output files actually all exist. If they don't
      // stop the runner, report an error on the job, and don't cache complete
      // any of the output since they may be corrupted.
      const auto &job_output = result.Job->GetOutput();
      for (TFileInfo *file_info : job_output) {
        if (!ExistsPath(file_info->CmdPath.c_str())) {
          cout << "ERROR: " << result.Job << " didn't produce the output file, " << file_info
               << ", it said it would but exited successfully. The job has a bug.\n"
               << "STDOUT:\n";
          output.Subprocess.Output->ReadAllFromWarnOverflow(STDOUT_FILENO);
          cout << "STDERR:\n";
          output.Subprocess.Error->ReadAllFromWarnOverflow(STDOUT_FILENO);

          // Mark runner as failed which will cause outer loop to exit.
          Runner.Shutdown();
          return;
        }
      }

      // Mark all the job and all output files of the job as complete, storing
      //  all the information needed to later cache complete the file if
      // nothing has changed.
      // TODO(cmaloney): Allow jobs to attach arbitrary computed config to
      // each output file.
      InsertOrFail(Done, result.Job);
      auto extra_file_data = result.Job->GetOutputExtraData();
      for (TFileInfo *file_info : job_output) {
        // NOTE: This implicitly constructs an empty TJobData if one isn't found.
        file_info->Complete(result.Job, move(extra_file_data[file_info]));
      }

      // DEBUG: cout << "Completed files [" << result.Job << ": " << Join(job_output, " ") << "\n";

      // If any jobs were waiting on the output files of this job, try queing
      // those jobs.
      // Gather all the jobs which got unblocked. Do this first / at once so
      // that we also remove the "WaitingForJob" entries before we call
      // QueueJob which will start waiting for job.
      std::unordered_set<TJob *> unblocked;
      auto it = WaitingForJob.find(result.Job);
      if (it != WaitingForJob.end()) {
        // DEBUG: std::cout << "Waiting, now unblocked: " << Join(it->second, " ") << "\n";
        unblocked = std::move(it->second);
        WaitingForJob.erase(it);
      }
      for (TJob *job : unblocked) {
        QueueJob(job);
      }
    };

    // Process the result
    switch (output.Result) {
      case TJob::TOutput::Error: {
        // Print the error output. Runner will naturally coalesce and DoAdvance
        // will terminate the process with an error.
        cout << "ERROR: " << result.Job << " exited with an error.\n"
             << "STDOUT:\n";
        output.Subprocess.Output->ReadAllFromWarnOverflow(STDOUT_FILENO);
        cout << "STDERR:\n";
        output.Subprocess.Error->ReadAllFromWarnOverflow(STDOUT_FILENO);
        break;
      }
      case TJob::TOutput::NewNeeds: {
        // Re-queue the job to be run once all its needs are completed. If all
        // the needs are already done, put it into the runner immediately
        if (!block_if_needs(output.Needs)) {
          // DEBUG: std::cout << "Should immediately re-queue" << result.Job << "\n";
          QueueJob(result.Job);
        } else {
          // DEBUG: std::cout << ":/ " << Join(output.Needs.Mandatory, " ") << "\n";

        }
        break;
      }
      case TJob::TOutput::CompleteIfNeeds: {
        if (!block_if_needs(output.Needs)) {
          mark_complete();
        }
        break;
      }
      case TJob::TOutput::Complete: {
        mark_complete();
        break;
      }
    }

    // TODO(cmaloney): If there is less work than parallelism in Runner,
    // schedule jobs which didn't have all their needs complete to have their
    // needs calculated again. This is excess work, but otherwise the cpu would
    // be sitting idle and by re-scheduling jobs early we can likely find more
    // jobs which need to be run to get to the eventual target.
    // NOTE: This requires that jobs write output files atomically (write them
    // elsewhere then move them into place). Otherwise some jobs might consume
    // partially constructed output.
  }

  // Runner exited with an error, exit the program.
  if (!Runner.IsReady()) {
    throw TErrorExit(1, "One or more build jobs reported failure. Details above.");
  }
}

// Queue the given job based on its state.
// If the job is already being run, no-op (The jobs is running, will be checked based on its
// output).
// If all needs are complete, mark the job as running and queue it.
// Since all needs aren't complete, queue the jobs needed to complete those
// files.
void TStatusTracker::QueueJob(TJob *job) {
  assert(job);
  // Make sure it's always in All.
  All.insert(job);

  if (Contains(Running, job)) {
    return;
  }

  InsertOrFail(Running, job);
  Runner.Queue(job);
}

TJob *TStatusTracker::TryGetProducer(TFileInfo *file, TFileType file_type) {
  // Check the cache
  auto elem = TryFind(Producers, file);
  if (elem) {
    return *elem;
  }

  // See if we can find a job from the set of all possible jobs which can produce
  // the given file which has a producible input.
  // Having more than one producible input is an unresolvable ambiguity.
  std::vector<TJob *> ProducibleJobs;
  for (TJob *job : Environment.GetPotentialJobsProducingFile(file, file_type)) {
    if (IsBuildable(job->GetInput(), job->GetJobProducer()->InType)) {
      ProducibleJobs.push_back(job);
    }
  }

  // Ambiguous to produce. Cannot proceed.
  if (ProducibleJobs.size() > 1) {
    THROWER(runtime_error) << "Multiple producers for file '" << file
                           << "'. Producers: " << Base::Join(ProducibleJobs, ",");
  }

  // No producer. Store result in cache and return.
  if (ProducibleJobs.size() == 0) {
    Producers[file] = nullptr;
    return nullptr;
  }

  assert(ProducibleJobs.size() == 1);
  TJob *job = ProducibleJobs[0];

  // Mark all outputs as producible by the job. As an extra safety throw if we
  // already have a producer of the file. If jobs declare all their possible
  // output types this should never fire because the previous ambiguous check
  // should have caught it.
  // Also perform a sanity check that the job actually output the requested file
  // since it said it would.
  bool found_self = false;
  for (TFileInfo *output : job->GetOutput()) {
    if (!Producers.emplace(output, job).second) {
      THROWER(runtime_error) << "Multiple producers for file '" << output
                             << "'. Producers: " << Producers[output] << ", " << job;
    }
    found_self |= file == output;
  }
  assert(found_self);

  return job;
}

void Bit::DoProduce(uint64_t worker_count, TEnvironment &environment, vector<string> Targets) {
  // Find jobs to produce the given files, process the results of those jobs
  // until there is either an error running a job or all the files have been
  // produced.
  TStatusTracker status_tracker(environment, worker_count);

  // Add the targets as needed files. Queue the jobs which are implied by those
  // into the job runner to start the work queue.
  bool has_failure = false;
  for (const auto &target : Targets) {
    auto rel_path = environment.TryGetRelPath(target);
    if (!rel_path) {
      has_failure = true;
      THROWER(runtime_error) << "Unable to locate target " << quoted(target)
                             << ". File is not Not in src or out tree.\n";
    }
    status_tracker.AddNeeded(environment.GetFileInfo(*rel_path));
  }

  if (has_failure) {
    throw TErrorExit(1, "Couldn't resolve all specified targets to real files");
  }

  // Have the status tracker try to advance to completion of all jobs it is
  // tracking.
  status_tracker.DoAdvance();
}
