#include <bit/produce.h>

#include <base/not_implemented.h>
#include <bit/environment.h>
#include <bit/job_runner.h>

using namespace Bit;
using namespace std;

//NOTE: When a job is in the run queue, it absolutely must never be manipulated
// or have any methods called by the StatusTracker. Any/all calls are likely
// to result in data races.

class TStatusTracker {
  public:
  TStatusTracker(TEnvironment &environment) : Environment(environment) {}

  // Returns true IFF all needed files have been produced.
  bool IsDone() const;

  void AddNeeded(string target);

  void Advance(const TJobRunner::TResult &result, TJobRunner &runner);

  std::vector<TJob*> GetActiveJobs();

  TEnvironment &Environment;

  // TODO(cmaloney): on top of queued and done we need to be able to track things
  // that could be "optimistically checked"
  std::vector<TJob *> Queued;
  std::vector<TFileInfo *> Holding;
  std::vector<TFileInfo *> Done;
};

// Figure out the jobs which need to be run to create the given set of targets
// and run them to create the targets.
bool Bit::Produce(uint64_t worker_count, TEnvironment &environment, vector<string> Targets) {
  TJobRunner runner(worker_count);

  // Find jobs to produce the given files, process the results of those jobs
  // until there is either an error running a job or all the files have been
  // produced.
  TStatusTracker status_tracker(environment);

  // Add the targets as needed files. Queue the jobs which are implied by those
  // into the job runner to start the work queue.
  for(const auto &target: Targets) {
    status_tracker.AddNeeded(target);
  }

  // Manually add the jobs. Normally Advance will add jobs as needed in state
  // processing.
  for(TJob *job: status_tracker.GetActiveJobs()) {
    runner.Queue(job);
  }

  // Hand results of jobs to the status tracker, and have it figure out what
  // more to do.
  while(!status_tracker.IsDone() && runner.IsReady() && runner.HasMoreResults()) {
    status_tracker.Advance(runner.WaitForResult(), runner);
  }

  return status_tracker.IsDone();
}
