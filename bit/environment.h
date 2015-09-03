/* bit build environment.

Keeps track of all the files which exist, could exist lazily.

Determines whether or not files could exist by lazily seeing if there is a path
from a file which does exist which could be manipulated by known jobs to produce
the needed file. */

#include <base/class_traits.h>
#include <bit/file_info.h>
#include <bit/job.h>
#include <bit/job_producer.h>
#include <bit/naming.h>

namespace Bit {

class TEnvironment;

// Simple memory management by making one object (The registry) own all the pointers/memory.
template <typename TKey, typename TValue, typename Hash = std::hash<TKey>>
class TInterner {
  public:
  TValue *Add(TKey &&key, std::unique_ptr<TValue> &&item) {
    // TODO: Add a new InsertOrFail which can handle this
    auto result = ManagedThings.emplace(std::move(key), std::move(item));
    assert(result.second);

    return result.first->second.get();
  }

  TValue *TryGet(const TKey &key) {
    auto it = ManagedThings.find(key);
    if(it != ManagedThings.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  private:
  std::unordered_map<TKey, std::unique_ptr<TValue>, Hash> ManagedThings;
};

class TJobFactory {
  public:
  TJobFactory(const TJobConfig &job_config, const std::unordered_set<std::string> &jobs);

  std::unordered_set<TJob *> GetPotentialJobs(TEnvironment &env, TFileInfo *out_file);

  private:
  TInterner<TJob::TId, TJob, TJob::TId::THash> Jobs;

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

  /* Get the FileInfo which contains compilation state information for a given
     relative path. Relative paths should first be resolved from by calling
     GetRelPath */
  TFileInfo *GetFileInfo(TRelPath name);

  /* Attempts to find the tree for the given file and return the relative path.
     If the path doesn't begin with `/` it is assumed to be a relative path.
     If the path doesn't belong to any tree, the full path is given as the
     relative path. */
  TRelPath GetRelPath(const std::string &path);

  private:
  std::unordered_map<std::string, TFileInfo *> PathLookupCache;
  TInterner<TRelPath, TFileInfo> Files;
  TJobFactory Jobs;

  TTree Src, Out;
};
}  // Bit
