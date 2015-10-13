#include <bit/produce.h>

#include <iomanip>
#include <iostream>

#include <base/not_implemented.h>
#include <bit/environment.h>
#include <bit/job_runner.h>
#include <bit/status_line.h>
#include <cmd/util.h>

using namespace Bit;
using namespace Cmd;
using namespace std;

// NOTE: When a job is in the run queue, it absolutely must never be manipulated
// or have any methods called by the StatusTracker. Any/all calls are likely
// to result in data races.

class TStatusTracker {
  public:
  TStatusTracker(TEnvironment &environment) : Environment(environment) {}

  // Returns true IFF all needed files have been produced.
  bool IsDone() const { return All.size() == Done.size(); }

  bool TryAddNeeded(TFileInfo *file);

  void Advance(const TJobRunner::TResult &result, TJobRunner &runner);

  const std::vector<TJob *> &GetQueuedJobs() const { return Queued; }

  TEnvironment &Environment;

  // TODO(cmaloney): on top of queued and done we need to be able to track things
  // that could be "optimistically checked"
  std::vector<TJob *> Queued;
  std::vector<TJob *> Holding;
  std::vector<TJob *> Done;
  std::vector<TJob *> All;
};

bool TStatusTracker::TryAddNeeded(TFileInfo *file) {
  if (file->IsComplete()) {
    return true;
  }

  // Try finding a producer of the file which is producable.

}

bool Bit::Produce(uint64_t worker_count, TEnvironment &environment, vector<string> Targets) {
  TJobRunner runner(worker_count);

  // Find jobs to produce the given files, process the results of those jobs
  // until there is either an error running a job or all the files have been
  // produced.
  TStatusTracker status_tracker(environment);

  // Add the targets as needed files. Queue the jobs which are implied by those
  // into the job runner to start the work queue.
  bool has_failure = false;
  for(const auto &target : Targets) {
    auto rel_path = environment.TryGetRelPath(target);
    if(rel_path) {
      auto file_info = environment.GetFileInfo(*rel_path);
      if(!status_tracker.TryAddNeeded(file_info)) {
        has_failure = true;
        cout << "ERROR: No known way to produce target " << quoted(target) << ".\n";
      }
    } else {
      has_failure = true;
      cout << "ERROR: Unable to locate target " << quoted(target)
           << ". File is not Not in src or out tree.\n";
    }
  }

  if(has_failure) {
    throw TErrorExit(1, "Couldn't resolve all specified targets to real files");
  }

  // Manually add the jobs. Normally Advance will add jobs as needed in state
  // processing.
  for(TJob *job : status_tracker.GetQueuedJobs()) {
    runner.Queue(job);
  }

  // Hand results of jobs to the status tracker, and have it figure out what
  // more to do.
  while(!status_tracker.IsDone() && runner.IsReady() && runner.HasMoreResults()) {
    status_tracker.Advance(runner.WaitForResult(), runner);
    TStatusLine() << '[' << status_tracker.Done.size() << '/' << status_tracker.All.size()
                  << "] waiting: " << status_tracker.Queued.size();
  }

  return status_tracker.IsDone();
}
