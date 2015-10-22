#include <bit/environment.h>

#include <algorithm>
#include <cassert>
#include <iomanip>

#include <base/not_implemented.h>
#include <bit/naming.h>
#include <util/path.h>
#include <util/stl.h>

// #include <bit/jobs/compile_c_family.h>
#include <bit/jobs/dep.h>
// #include <bit/jobs/link.h>

using namespace Base;
using namespace Bit;
using namespace std;
using namespace Util;

template <typename TVal>
using TSet = unordered_set<TVal>;

TJobFactory::TJobFactory(const TJobConfig &job_config, const TSet<string> &jobs) {
  const unordered_map<string, TJobProducer (*)(const Base::TJson &job_config)> builtin_jobs = {
      {"dependency", &Job::TDep::GetProducer}
      // {"compile_c", &Job::TCompileCFamily::GetCProducer},
      // {"compile_cc", &Job::TCompileCFamily::GetCcProducer},
      // {"link", &Job::TLink::GetProducer}
  };

  if (jobs.empty()) {
    // Enable all builtin jobs
    for (const auto &job : builtin_jobs) {
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
      for (const TRelPath &rel_path : producer.GetOutput(*opt_path)) {
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
  if (!found) {
    JobsByOutput.emplace(target_output, nullptr);
  }

  return ret;
}

TEnvironment::TEnvironment(const TConfig &config, const TTree &src)
    : Jobs(config.JobConfig, config.Jobs), Src(src), Out(config.CacheDirectory) {}

TFileInfo *TEnvironment::GetFileInfo(TRelPath name) {
  TFileInfo *result = Files.TryGet(name);
  if (result) {
    return result;
  }

  // If the file exists in src, make it there. Otherwise it is a generated / out
  // tree file.
  TAbsPath src_abs_path = TAbsPath(Src, name);

  auto add_file = [&](std::string cmd_path, bool is_src) {
    // NOTE: It is an intentional design decision that the file configuration must
    // always be in src / never be calculated. If it could have been calculated,
    // than that should be attached by whatever job produces the file, or by running
    // a job like the dependency finder which makes a side file of additional needed
    // information.

    // NOTE: This is explicitly separate from the make_unique becausewe need to
    // guarantee the TRelPath copying happens before
    auto file_info =
        make_unique<TFileInfo>(TRelPath(name), std::move(cmd_path),
                               TFileConfig(src_abs_path.AddExtension(".bitconfig.json")), is_src);
    return Files.Add(TRelPath(name), std::move(file_info));
  };

  if (ExistsPath(src_abs_path.Path.c_str())) {
    return add_file(name.Path, true);
  } else {
    return add_file(TAbsPath(Out, name).Path, false);
  }
}

// TODO(cmaloney): Just implement GetPotentialJobs inline here...
std::unordered_set<TJob *> TEnvironment::GetPotentialJobsProducingFile(TFileInfo *file_info) {
  return Jobs.GetPotentialJobs(*this, file_info);
}

/* Attempts to find the tree for the given file and return the relative path.
   If the path doesn't begin with `/` it is assumed to be a relative path.
   If the path doesn't belong to any tree, the full path is given as the
   relative path. */
TOpt<TRelPath> TEnvironment::TryGetRelPath(const std::string &path) {
  // 0 length paths are illegal.
  assert(path.size() > 0);

  // Try getting out of the cache as a performance optimization.
  auto it = PathLookupCache.find(path);
  if (it != PathLookupCache.end()) {
    return it->second;
  }

  auto make_rel_remove_prefix = [&path](const TTree &tree) {
    return TRelPath(path.substr(tree.Path.size()));
  };

  // If it's already a relative path, just return it.
  if (path[0] != '/') {
    return TRelPath(path);
  }

  // Search for the tree which contains the path.
  // Out might be a subdirectory of src, so check it first.
  if (path.compare(0, Out.Path.length(), Out.Path) == 0) {
    return make_rel_remove_prefix(Out);
  } else if (path.compare(0, Src.Path.length(), Src.Path) == 0) {
    return make_rel_remove_prefix(Src);
  }

  // Tree not known. Can't make a relative path.
  return TNone();
}
