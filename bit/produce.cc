#include <bit/produce.h>

#include <iomanip>
#include <iostream>
#include <stdexcept>

#include <base/not_implemented.h>
#include <base/split.h>
#include <base/thrower.h>
#include <bit/environment.h>
#include <bit/job_runner.h>
#include <bit/status_line.h>
#include <cmd/util.h>
#include <util/stl.h>

using namespace Bit;
using namespace Cmd;
using namespace std;
using namespace Util;

// NOTE: When a job is in the run queue, it absolutely must never be manipulated
// or have any methods called by the StatusTracker. Any/all calls are likely
// to result in data races.

class TStatusTracker {
  public:
  TStatusTracker(TEnvironment &environment) : Environment(environment) {}

  // Returns true IFF all needed files have been produced.
  bool IsDone() const { return All.size() == Done.size(); }
  bool IsBuildable(TFileInfo *file);

  // TODO(cmaloney): The recursion this does with IsBuildable needs to be worked out...
  bool TryAddNeeded(TFileInfo *file);

  void Advance(const TJobRunner::TResult &result, TJobRunner &runner);

  const std::vector<TJob *> &GetQueuedJobs() const { return Queued; }

  TJob *TryGetProducer(TFileInfo *file);

  TEnvironment &Environment;

  // TODO(cmaloney): on top of queued and done we need to be able to track things
  // that could be "optimistically checked"
  std::vector<TJob *> Queued;
  std::vector<TJob *> Holding;
  std::vector<TJob *> Done;
  std::vector<TJob *> All;
  std::unordered_map<TFileInfo *, TJob *> Producers;
};

bool TStatusTracker::IsBuildable(TFileInfo *file) {
  return file->IsComplete() || TryGetProducer(file);
}

// TODO(cmaloney): Make this an always succeed + throw on not able to produce
bool TStatusTracker::TryAddNeeded(TFileInfo *file) {
  if(file->IsComplete()) {
    return true;
  }

  // Try finding a producer of the file which is producable.
  TJob *job = TryGetProducer(file);
  if(!job) {
    // TODO(cmaloney): Capture this exception and wrap it with the job which needed this file
    // to be produced.
    THROWER(runtime_error) << "No known way to produce file '" << file << "'.";
  }

  // TODO(cmaloney): Add the producer to either the appropriate queue or the waiting queue.
  NOT_IMPLEMENTED();
}

TJob *TStatusTracker::TryGetProducer(TFileInfo *file) {
  // Check the cache
  auto elem = TryFind(Producers, file);
  if(elem) {
    return *elem;
  }

  // See if we can find a job from the set of all possible jobs which can produce
  // the given file which has a producible input.
  // Having more than one producible input is an unresolvable ambiguity.
  std::vector<TJob *> ProducibleJobs;
  for(TJob *job : Environment.GetPotentialJobsProducingFile(file)) {
    if(IsBuildable(job->GetInput())) {
      ProducibleJobs.push_back(job);
    }
  }

  // Ambiguous to produce. Cannot proceed.
  if(ProducibleJobs.size() > 1) {
    THROWER(runtime_error) << "Multiple producers for file '" << file
                           << "'. Producers: " << Base::Join(ProducibleJobs, ",");
  }

  // No producer. Store result in cache and return.
  if(ProducibleJobs.size() == 0) {
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
  for(TFileInfo *output : job->GetOutput()) {
    if(!Producers.emplace(output, job).second) {
      THROWER(runtime_error) << "Multiple producers for file '" << output
                             << "'. Producers: " << Producers[output] << ", " << job;
    }
    found_self |= file == output;
  }
  assert(found_self);

  return job;
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
