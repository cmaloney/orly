#include <bit/environment.h>

#include <algorithm>
#include <cassert>
#include <iomanip>

#include <jhm/jobs/compile_c_family.h>
#include <jhm/jobs/dep.h>
#include <jhm/jobs/link.h>

using namespace Base;
using namespace Bit;
using namespace std;

TJobFactory::TJobFactory(const TJobConfig &job_config, const std::unordered_set<std::string> &jobs) {
  const std::unordered_map<std::string, TJobProducer(*)()> builtin_jobs = {
    {"dependency", &Job::TDep::GetProducer},
    {"compile_c", &Job::TCompileCFamily::GetCProducer},
    {"compile_cc", &Job::TCompileCFamily::GetCcProducer},
    {"link", &Job::TLink::GetProducer}
  };

  if (Jobs.empty()) {
    // Enable all builtin jobs
    for(const auto &job: builtin_jobs) {
      Register(job.second());
    }
  } else {
    // Enable all specified jobs. If a job is unknown, try to load it as a
    // shared library.
  }
}

TJobFactory::TJobFactory(bool disable_default_jobs) {

  if (!disable_default_jobs) {
    for (const auto &job: builtin_jobs) {
      Register(job.second());
    }
  }
}

unordered_set<TJob *> TJobFactory::GetPotentialJobs(TEnv &env, TFile *out_file) {
  unordered_set<TJob *> ret;

  // Check the cache
  auto range = JobsByOutput.equal_range(out_file);
  if(range.first != JobsByOutput.end()) {
    for_each(range.first, range.second, [&ret](const pair<TFile *, TJob *> &pair) {
      // NOTE: job can be a nullptr, which is valid in the cache to indicate "no jobs exist".
      if(pair.second) {
        // TODO: InsertOrFail.
        ret.insert(pair.second);
      }
    });
  } else {
    // No cache found. Build it
    // Find and instantiate possibilities based on extension
    for(const auto &producer : JobProducers) {
      TOpt<TRelPath> opt_path = producer.TryGetInputName(out_file->GetRelPath());
      if(!opt_path) {
        continue;
      }
      TFile *in = env.GetFile(*opt_path);
      if(!in) {
        continue;
      }

      TJobDesc job_desc{in, producer.Name};
      TJob *job = Jobs.TryGet(job_desc);

      // We've found this job before, which means we're an additional unstated output of the job.
      if(job) {
        if(!Contains(job->GetOutput(), out_file)) {
          job->AddOutput(out_file);
        }
      } else {
        // Make the new job
        job = Jobs.Add(TJobDesc{in, producer.Name}, producer.MakeJob(env, in));
        if(job->HasUnknownOutputs()) {
          job->AddOutput(out_file);
        }
      }
      // We better be produced by the job....
      assert(Contains(job->GetOutput(), out_file));

      // Add ourself to the cache map, as well as the set of jobs being returned.
      // TODO: InsertOrFail.
      ret.insert(job);
      JobsByOutput.emplace(out_file, job);
    }

    // If no entries have been added to cache, then insert a nullptr to indicate we did try
    if(JobsByOutput.find(out_file) == JobsByOutput.end()) {
      JobsByOutput.emplace(out_file, nullptr);
    }
  }

  return ret;
}
