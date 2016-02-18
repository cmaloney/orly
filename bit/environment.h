/* bit build environment.

Keeps track of all the files which exist, could exist lazily.

Determines whether or not files could exist by lazily seeing if there is a path
from a file which does exist which could be manipulated by known jobs to produce
the needed file. */
#pragma once

#include <base/class_traits.h>
#include <base/interner.h>
#include <base/opt.h>
#include <bit/file_environment.h>
#include <bit/file_info.h>
#include <bit/job.h>
#include <bit/job_producer.h>
#include <bit/naming.h>

namespace Bit {

class TEnvironment;

class TJobFactory {
  public:
  TJobFactory(const TJobConfig &job_config, const std::unordered_set<std::string> &jobs);

  std::unordered_set<TJob *> GetPotentialJobs(TEnvironment &env, TFileInfo *out_file, TFileType file_type);

  private:
  Base::TInterner<TJob::TId, TJob, TJob::TId::THash> Jobs;

  // NOTE: This is used purely for caching the reverse-lookups used by  GetPotentialJobs.
  // Entry in this map simply means a job exists, not that it's possible to get it's input.
  std::unordered_multimap<TFileInfo *, TJob *> JobsByOutput;

  std::vector<TJobProducer> JobProducers;
};

class TEnvironment {
  public:
  NO_COPY(TEnvironment)
  NO_MOVE(TEnvironment)

  TEnvironment(const TConfig &config, const TTree &src);

  // The file environment is thread-safe on all its methods. Always owned by
  // TEnvironment.
  TFileEnvironment Files;

  /* Get the FileInfo which contains compilation state information for a given
     relative path. Relative paths should first be resolved from by calling
     GetRelPath */
  TFileInfo *GetFileInfo(TRelPath name);

  /* Attempts to find the tree for the given file and return the relative path.
     If the path doesn't begin with `/` it is assumed to be a relative path.
     If the path doesn't belong to any known tree, None is reeturned. */
  Base::TOpt<TRelPath> TryGetRelPath(const std::string &path);

  /* Get the potential jobs which could produce a given file. Just because a job
     exists doesn't mean that it is producible (The input may not exist and may
     not be producible) */
  std::unordered_set<TJob *> GetPotentialJobsProducingFile(TFileInfo *file_info, TFileType file_type);


  private:
  TJobFactory Jobs;

  TTree Src, Out;
};
}  // Bit
