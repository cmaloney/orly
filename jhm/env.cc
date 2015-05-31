/* <jhm/env.cc>

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <jhm/env.h>

#include <algorithm>
#include <cassert>
#include <iomanip>

#include <jhm/jobs/compile_c_family.h>
#include <jhm/jobs/dep.h>
#include <jhm/jobs/link.h>
#include <util/path.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace Util;

TJobFactory::TJobFactory(bool disable_default_jobs) {
  const std::unordered_map<std::string, TJobProducer(*)()> builtin_jobs = {
    {"dependency", &Job::TDep::GetProducer},
    {"compile_c", &Job::TCompileCFamily::GetCProducer},
    {"compile_cc", &Job::TCompileCFamily::GetCcProducer},
    {"link", &Job::TLink::GetProducer}
  };

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

Jhm::TTree TEnv::GetOutDirName(const TTree &src,
                               const string &config,
                               const string &config_mixin) {
  TTree out(src);
  out.Root.push_back(".jhm");
  out.Root.push_back("out");
  out.Root.push_back(config);
  if(config_mixin.size()) {
    out.Root.back() += '_' + config_mixin;
  }
  return out;
}

vector<string> GetConfigList(const TTree &src,
                             const string &config,
                             const string &config_mixin) {
  // project/config.jhm.mixin -> project/config.jhm -> project/core.jhm
  // NOTE: At least one mixin file must be present if a config mixin is specified.
  vector<string> ret;
  string src_str = AsStr(src);

  auto add_conf = [&config_mixin, &ret, &src_str](bool mixin, string path) {
    if(mixin && config_mixin.empty()) {
      return;
    }
    path = src_str + '/' + path + ".jhm";
    if(mixin) {
      path += "_mixin";
    }
    ret.push_back(move(path));
  };
  add_conf(false, config);
  add_conf(true, config_mixin);
  // TODO(cmaloney): This is probably the wrong ordering for the environment
  // configuration.
  add_conf(false, "/core");
  add_conf(false, '/' + config);
  add_conf(true, '/' + config_mixin);
  add_conf(false, "/environment");
  return ret;
}

TEnv::TEnv(const TTree &src,
           const string &config,
           const string &config_mixin,
           bool disable_default_jobs)
    : Src(src),
      Out(GetOutDirName(src, config, config_mixin)),
      Config(GetConfigList(src, config, config_mixin)),
      Jobs(disable_default_jobs) {
  if(config == "core") {
    THROWER(runtime_error)
        << "the config 'core' is reserved / special. Use a different config.";
  }

  // NOTE: Technically not a hard error. But usually indicates something went wrong.
  if(!Config.HasConfig()) {
    THROWER(runtime_error) << "No config file found for config " << quoted(config + ".jhm")
                               << ". At least one config looked for must exist";
  }

}

TFile *TEnv::GetFile(TRelPath name) {
  // If we've been found before, just return what was found.
  if(TFile *f = Files.TryGet(name)) {
    return f;
  }

  // If doesn't exist in src, must be in out / generated
  TPath src_path = Src.GetAbsPath(name);

  TJson file_conf(TJson::Object);

  string src_path_str = AsStr(src_path);
  // NOTE: It's a design decision that this can only come from src and can't be machine generated.
  string conf_path = src_path_str + ".jhm";

  if(ExistsPath(src_path_str.c_str())) {
    auto file = make_unique<TFile>(TRelPath(name), &Src, true, conf_path);
    return Files.Add(move(name), move(file));
  } else {
    TPath out_path = Out.GetAbsPath(name);
    auto file = make_unique<TFile>(TRelPath(name), &Out, false, conf_path);
    return Files.Add(move(name), move(file));
  }
}

bool TEnv::IsBuildable(TFile *file) const {
  assert(this);
  assert(Buildable);

  return Buildable(file);
}

bool TEnv::IsDone(TFile *file) const {
  assert(this);
  assert(Done);

  return Done(file);
}

void TEnv::SetFuncs(TFileCheckFunc &&buildable, TFileCheckFunc &&done) {
  assert(!Buildable);
  assert(!Done);
  assert(buildable);
  assert(done);

  Buildable = move(buildable);
  Done = move(done);
}

// TODO(cmaloney): Merge this function with GetFile.
TFile *TEnv::TryGetFileFromPath(const std::string &name) {
  assert(&name);
  assert(name.size());  // Name must be non-empty.

  auto it = PathLookupCache.find(name);
  if(it != PathLookupCache.end()) {
    return it->second;
  }

  TFile *result = nullptr;

  // If the name starts with a '/' it's an absolute filesystem path
  TPath path(name);
  if(name[0] == '/') {
    // Out is a subdirectory of src so check it first.
    if(Out.Contains(path)) {
      result = GetFile(Out.GetRelPath(move(path)));
    } else if(Src.Contains(path)) {
      // TODO(cmaloney): path shouldn't have a folder '.jhm' in it.
      result = GetFile(Src.GetRelPath(move(path)));
    } else {
      // File isn't in a known tree, so we can't possibly get it.
      result = nullptr;
    }
  } else {
    result = GetFile(TRelPath(move(path)));
  }

  PathLookupCache.insert(it, make_pair(name, result));
  return result;
}
