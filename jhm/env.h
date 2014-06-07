/* <jhm/env.h>

   Keeps track of where inputs can come from, outputs can go, what translations we know about, configuration, etc.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <jhm/config.h>
#include <jhm/file.h>
#include <jhm/job.h>

namespace Jhm {

  template<typename TVal>
  using TSet = std::unordered_set<TVal>;

  // Simple memory management by making one object (The registry) own all the pointers/memory.
  template <typename TKey, typename TValue, typename Hash = std::hash<TKey>>
  class TInterner {

    public:
    TValue *Add(TKey &&key, std::unique_ptr<TValue> &&item) {
      //TODO: Add a new InsertOrFail which can handle this
      auto result = ManagedThings.emplace(std::move(key), std::move(item));
      assert(result.second);

      return result.first->second.get();
    }

    TValue *TryGet(const TKey &key) {
      auto it = ManagedThings.find(key);
      if (it != ManagedThings.end()) {
        return it->second.get();
      }
      return nullptr;
    }

    private:
    std::unordered_map<TKey, std::unique_ptr<TValue>, Hash> ManagedThings;
  };

  class TJobFactory {
    public:

    // NOTE: We don't just use a tuple because we want a specialized hash
    struct TJobDesc {
      TFile *f;
      const char *job_name;

      bool operator==(const TJobDesc &that) const noexcept {
        return f == that.f && job_name == that.job_name;
      }
    };

    struct TJobDescHash {
      size_t operator()(const TJobDesc &that) const noexcept {
        // TODO: Write a better hash...
        return size_t(that.f) + size_t(that.job_name);
      }

    };

    // A Job registration can verify if they can indeed make a given output file
    // A job registration is used to do longest tail matches and see if the job's input can be produced
    //NOTE: We generate a job object when it's input can be produced
    void Register(TJobProducer &&producer) {
      JobProducers.emplace_back(std::move(producer));
    }

    std::unordered_set<TJob *> GetPotentialJobs(TEnv &env, TFile *out_file);

    private:
    //TODO: Make a better hash function.
    TInterner<TJobDesc, TJob, TJobDescHash> Jobs;

    // NOTE: This is used purely for caching the reverse-lookups used by  GetPotentialJobs.
    // Entry in this map simply means a job exists, not that it's possible to get it's input.
    std::unordered_multimap<TFile *, TJob *> JobsByOutput;

    // TODO: We iterate over this a lot. In practice we go over a lot more than needed
    // Perf test using more concise data structures where we don't scan so much needlessly vs. this "one simple list" we
    // always iterate over all of.
    std::vector<TJobProducer> JobProducers;
  };

  class TEnv {
    public:
    NO_COPY(TEnv);
    NO_MOVE(TEnv);

    using TFileCheckFunc = std::function<bool (TFile*)>;

    static Jhm::TAbsBase GetOutDirName(const std::string &root,
                                     const std::string &proj_name,
                                     const std::string &config,
                                     const std::string &config_mixin);

    // NOTE: if proj_name is src. Output directory is just out.
    TEnv(const TAbsBase &root,
         const std::string &proj_name,
         const std::string &config,
         const std::string &config_mixin);

    // Add a job to the job
    TJob *Add(std::unique_ptr<TJob> &&job);
    TFile *Add(std::unique_ptr<TFile> &&file);

    const TConfig &GetConfig() {
      return Config;
    }
    const TAbsBase &GetSrc() const {
      return Src;
    }
    const TAbsBase &GetOut() const {
      return Out;
    }

    const TAbsBase &GetRoot() const {
      return Root;
    }

    std::unordered_set<TJob*> GetJobsProducingFile(TFile *file) {
      return Jobs.GetPotentialJobs(*this, file);
    }

    /* Finds the file / builds a correct TFile object for it.
       NOTE: Does absolutely nothing for testing if file is producable, needs to be built, etc. */
    TFile *GetFile(TRelPath name);

    bool IsBuildable(TFile *file) const;
    bool IsDone(TFile *file) const;
    void SetFuncs(TFileCheckFunc &&buildable, TFileCheckFunc &&done);

    /* Gets a file from the given path, which is either an absolute filesystem path (starts with '/') or relative within
       the project (a TRelPath) */
    TFile *TryGetFileFromPath(const std::string &name);

    private:
    TFileCheckFunc Buildable;
    TFileCheckFunc Done;

    TInterner<TRelPath, TFile> Files;

    TAbsBase Root;
    TAbsBase Src, Out;

    TConfig Config; // All the config files stacked up which we need.

    TJobFactory Jobs;
  };
}