#include <bit/environment.h>

#include <algorithm>
#include <cassert>
#include <iomanip>

#include <base/not_implemented.h>
#include <util/stl.h>

// #include <bit/jobs/compile_c_family.h>
// #include <bit/jobs/dep.h>
// #include <bit/jobs/link.h>

using namespace Base;
using namespace Bit;
using namespace std;
using namespace Util;

template <typename TVal>
using TSet = unordered_set<TVal>;

TJobFactory::TJobFactory(const TJobConfig &job_config, const TSet<string> &jobs) {
  const unordered_map<string, TJobProducer (*)(const Base::TJson &job_config)> builtin_jobs = {
      //    {"dependency", &Job::TDep::GetProducer},
      //    {"compile_c", &Job::TCompileCFamily::GetCProducer},
      //    {"compile_cc", &Job::TCompileCFamily::GetCcProducer},
      //    {"link", &Job::TLink::GetProducer}
  };

  if(jobs.empty()) {
    // Enable all builtin jobs
    for(const auto &job : builtin_jobs) {
      // TODO(cmaloney): Shuold "get default" rather than force config section
      // into existence.
      JobProducers.emplace_back(job.second(move(job_config.at(job.first))));
    }
  } else {
    // TODO(cmaloney): Enable all specified jobs. If a job is unknown, try to
    // load it as a modular job. Modular jobs may be specified either in a text
    // format or as .so files which provide the 'GetProducer' function.
    NOT_IMPLEMENTED()
  }
}

TSet<TJob *> TJobFactory::GetPotentialJobs(TEnvironment &environment, TFileInfo *target_output) {
  TSet<TJob *> ret;

  // Check the cache
  // TODO(cmaloney): There should be a util that does this extraction shape (match -> set)
  auto range = JobsByOutput.equal_range(target_output);
  if(range.first != JobsByOutput.end()) {
    for_each(range.first, range.second, [&ret](const pair<TFileInfo *, TJob *> &pair) {
      // NOTE: job can be a nullptr, which is valid in the cache to indicate "no jobs exist".
      if(pair.second) {
        // TODO: InsertOrFail.
        ret.insert(pair.second);
      }
    });
    return ret;
  }

  // No cache found. Build it
  // Find and instantiate possibilities based on extension.
  // If no jobs are found, insert nullptr
  bool found = false;
  for(const auto &producer : JobProducers) {
    TOpt<TRelPath> opt_path = producer.TryGetInputName(target_output->RelPath);
    if(!opt_path) {
      continue;
    }
    TFileInfo *input = environment.GetFileInfo(*opt_path);
    if(!input) {
      continue;
    }

    found = true;

    TJob::TId job_id{&producer, input};
    TJob *job = Jobs.TryGet(job_id);

    // If the job hasn't been created, make it. The job might already exist if
    // another output found it first.
    if(!job) {
      // TODO(cmaloney): generalize this "copy across all things with lambda applied"
      TSet<TFileInfo *> output;
      for(const TRelPath &rel_path : producer.GetOutput(*opt_path)) {
        output.insert(environment.GetFileInfo(rel_path));
      }
      job = Jobs.Add(move(job_id), producer.MakeJob(&producer, input, move(output)));
    }

    // We better be produced by the job....
    assert(Contains(job->GetOutput(), target_output));

    // Add ourself to the cache map, as well as the set of jobs being returned.
    InsertOrFail(ret, job);
    JobsByOutput.emplace(target_output, job);
  }

  // If no entries have been added to cache, then insert a nullptr to indicate we did try
  if(!found) {
    JobsByOutput.emplace(target_output, nullptr);
  }

  return ret;
}
