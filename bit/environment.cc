#include <bit/environment.h>

#include <algorithm>
#include <cassert>
#include <iomanip>

#include <base/not_implemented.h>
#include <bit/jobs/compile_c_family.h>
#include <bit/jobs/dep.h>
#include <bit/jobs/link.h>
#include <bit/naming.h>
#include <util/path.h>
#include <util/stl.h>

using namespace Base;
using namespace Bit;
using namespace std;
using namespace Util;

template <typename TVal>
using TSet = unordered_set<TVal>;

TJobFactory::TJobFactory(const TJobConfig &job_config, const TSet<string> &jobs) {
  const unordered_map<string, TJobProducer (*)(const TJobConfig &)> builtin_jobs = {
      {"dependency", &Jobs::TDep::GetProducer},
      {"compile_c", &Jobs::TCompileCFamily::GetCProducer},
      {"compile_cc", &Jobs::TCompileCFamily::GetCcProducer},
      {"link", &Jobs::TLink::GetProducer}};

  if (jobs.empty()) {
    // Enable all builtin jobs
    for (const auto &job : builtin_jobs) {
      // TODO(cmaloney): Should "get default" rather than force config section
      // into existence.
      JobProducers.emplace_back(job.second(job_config));
    }
  } else {
    // TODO(cmaloney): Enable all specified jobs. If a job is unknown, try to
    // load it as a modular job. Modular jobs may be specified either in a text
    // format or as .so files which provide the 'GetProducer' function.
    NOT_IMPLEMENTED()
  }
}

TSet<TJob *> TJobFactory::GetPotentialJobs(TEnvironment &environment,
                                           TFileInfo *target_output,
                                           TFileType file_type) {
  TSet<TJob *> ret;

  // TODO(cmaloney): Reject things in the cache with the wrong file_type. GetPotentialJobs should
  // return the right jobs for the given TFileType
  // Check the cache
  // TODO(cmaloney): There should be a util that does this extraction shape (match -> set)
  auto range = JobsByOutput.equal_range(target_output);
  if (range.first != JobsByOutput.end()) {
    for_each(range.first, range.second, [&ret](const pair<TFileInfo *, TJob *> &pair) {
      // NOTE: job can be a nullptr, which is valid in the cache to indicate "no jobs exist".
      if (pair.second) {
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
  for (const auto &producer : JobProducers) {
    // If there is a file_type set, use it to filter possible jobs.
    if (file_type != TFileType::Unset) {
      if (!Contains(producer.OutTypes, file_type)) {
        continue;
      }
    }
    TOpt<TRelPath> opt_path = producer.TryGetInputName(target_output->RelPath);
    if (!opt_path) {
      continue;
    }
    TFileInfo *input = environment.GetFileInfo(*opt_path);
    if (!input) {
      continue;
    }

    found = true;

    TJob::TId job_id{&producer, input};
    TJob *job = Jobs.TryGet(job_id);

    // If the job hasn't been created, make it. The job might already exist if
    // another output found it first.
    if (!job) {
      // TODO(cmaloney): generalize this "copy across all things with lambda applied"
      TSet<TFileInfo *> output;
      for (const TRelPath &rel_path : producer.GetOutputName(*opt_path)) {
        output.insert(environment.GetFileInfo(rel_path));
      }
      job =
          Jobs.Add(move(job_id), producer.MakeJob(TJob::TMetadata{&producer, input, move(output)}));
    }

    // We better be produced by the job....
    assert(Contains(job->GetOutput(), target_output));

    // Add ourself to the cache map, as well as the set of jobs being returned.
    InsertOrFail(ret, job);
    JobsByOutput.emplace(target_output, job);
  }

  // If no entries have been added to cache, then insert a nullptr to indicate we did try
  if (!found) {
    JobsByOutput.emplace(target_output, nullptr);
  }

  return ret;
}

TEnvironment::TEnvironment(const TConfig &config, const TTree &src)
    : Files(src, config.CacheDirectory),
      Jobs(config.JobConfig, config.Jobs),
      Src(src),
      Out(config.CacheDirectory) {}

TFileInfo *TEnvironment::GetFileInfo(TRelPath name) { return Files.GetFileInfo(name); }

// TODO(cmaloney): Just implement GetPotentialJobs inline here...
std::unordered_set<TJob *> TEnvironment::GetPotentialJobsProducingFile(TFileInfo *file_info,
                                                                       TFileType file_type) {
  return Jobs.GetPotentialJobs(*this, file_info, file_type);
}

/* Attempts to find the tree for the given file and return the relative path.
   If the path doesn't begin with `/` it is assumed to be a relative path.
   If the path doesn't belong to any tree, the full path is given as the
   relative path. */
TOpt<TRelPath> TEnvironment::TryGetRelPath(const std::string &path) {
  return Files.TryGetRelPath(path);
}
