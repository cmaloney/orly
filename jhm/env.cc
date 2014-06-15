/* <jhm/env.cc>

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

#include <jhm/env.h>

#include <cassert>

#include <base/not_implemented.h>
#include <base/path_utils.h>
#include <jhm/jobs/bison.h>
#include <jhm/jobs/compile_c_family.h>
#include <jhm/jobs/dep.h>
#include <jhm/jobs/flex.h>
#include <jhm/jobs/link.h>
#include <jhm/jobs/nycr.h>
#include <jhm/timestamp.h>

using namespace Base;
using namespace Jhm;
using namespace std;

unordered_set<TJob *> TJobFactory::GetPotentialJobs(TEnv &env, TFile *out_file) {
  unordered_set<TJob *> ret;

  // Check the cache
  auto range = JobsByOutput.equal_range(out_file);
  if (range.first != JobsByOutput.end()) {
    for_each(range.first, range.second, [&ret](const pair<TFile*, TJob *> &pair) {
      // NOTE: job can be a nullptr, which is valid in the cache to indicate "no jobs exist".
      if (pair.second) {
        // TODO: InsertOrFail.
        ret.insert(pair.second);
      }
    });
  } else {
    // No cache found. Build it
    // Find and instantiate possibilities based on extension
    for(const auto &producer : JobProducers) {
      TOpt<TRelPath> opt_path = producer.TryGetInput(out_file->GetPath().GetRelPath());
      if(!opt_path) {
        continue;
      }
      TFile *in = env.GetFile(*opt_path);
      if (!in) {
        continue;
      }

      TJobDesc job_desc{in, producer.Name};
      TJob *job = Jobs.TryGet(job_desc);

      // We've found this job before, which means we're an additional unstated output of the job.
      if (job) {
        if (!Contains(job->GetOutput(), out_file)) {
          job->AddOutput(out_file);
        }
      } else {
        // Make the new job
        job = Jobs.Add(TJobDesc{in, producer.Name}, producer.MakeJob(env, in));
        if (job->HasUnknownOutputs()) {
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
    if (JobsByOutput.find(out_file) == JobsByOutput.end()) {
      JobsByOutput.emplace(out_file, nullptr);
    }
  }


  return ret;
}

TAbsBase TEnv::GetOutDirName(const string &root,
                                const string &proj_name,
                                const string &config,
                                const string &config_mixin) {
  string out = root + "/out";
  if (proj_name != "src") {
    out += '_' + proj_name;
  }
  out += '/' + config;
  if (config_mixin.size()) {
    out += '_' + config_mixin;
  }
  return TAbsBase(move(out));
}

vector<string> GetConfigList(const string &root,
                             const string &proj_name,
                             const string &config,
                             const string &config_mixin) {
  // root/project/config.jhm.mixin -> root/project/config.jhm -> root/config.jhm.mixin ->root/config.jhm
  //NOTE: At least one mixin file must be present if a config mixin is specified.
  vector<string> ret;
  ret.reserve(4);
  bool mixin_exists = false;
  auto add_mixin = [&mixin_exists,&ret,&config_mixin] (string mixin_path) {
    if (!config_mixin.empty())
    mixin_exists |= ExistsPath(mixin_path.c_str());
    ret.push_back(move(mixin_path));
  };
  add_mixin(root + '/' + config_mixin + ".jhm_mixin");
  ret.push_back(root + '/' + config + ".jhm");
  add_mixin(root + '/' + proj_name + '/' + config_mixin + ".jhm_mixin");
  ret.push_back(root + '/' + proj_name + '/' + config + ".jhm");

  return ret;
}

//TODO:
TEnv::TEnv(const TAbsBase &root, const string &proj_name, const string &config, const string &config_mixin)
    : Root(root),
      Src('/' + Root.Get() + '/' + proj_name),
      Out(GetOutDirName('/' + root.Get(), proj_name, config, config_mixin)),
      // TODO: Make the timestamp a property of TConfig
      Config(GetConfigList('/' + root.Get(), proj_name, config, config_mixin)) {

  // NOTE: Technically not a hard error. But usually indicates something went wrong.
  if (!Config.HasConfig()) {
    THROW_ERROR(runtime_error) << "No config file found for config " << quoted(config)
                               << ". At least one config looked for must exist";
  }
  //TODO: Assert the config stack contains at least one config
  /*
  // Load the configuation
  // TODO: Gracefully degrade on removal of a config.
  auto add_conf_if_exists = [this] (const string &filename, bool top) {
    if (ExistsPath(filename.c_str())) {
      Config.AddBase(TJson::Read(filename.c_str()), top);
      ConfigTimestamp = Newer(GetTimestamp(filename), ConfigTimestamp);
    }
  };

  // Add the project root overrides if they exist (Comes after project config)
  add_conf_if_exists('/' + Root.Get() + '/' + config_mixin + ".jhm_mixin", false);
  add_conf_if_exists('/' + Root.Get() + '/' + config + ".jhm", false);

  // Add config mixin if it exists (Comes before project config)
  add_conf_if_exists('/' + Src.Get() + config_mixin + ".jhm_mixin", true);

  // TODO: Include trees (useful for multi-repo JHM)
  */

  Jobs.Register(Job::TDep::GetProducer());
  Jobs.Register(Job::TCompileCFamily::GetCProducer());
  Jobs.Register(Job::TCompileCFamily::GetCppProducer());
  Jobs.Register(Job::TLink::GetProducer());
  Jobs.Register(Job::TNycr::GetProducer());
  Jobs.Register(Job::TNycrLang::GetProducer());
  Jobs.Register(Job::TFlex::GetProducer());
  Jobs.Register(Job::TBison::GetProducer());
}

TFile *TEnv::GetFile(TRelPath name) {

  // If we've been found before, just return what was found.
  if (TFile *f = Files.TryGet(name)) {
    return f;
  }


  // If doesn't exist in src, must be in out / generated
  TAbsPath src_path(Src, name);

  TJson file_conf(TJson::Object);

  // NOTE: It's a design decision that this can only come from src and can't be machine generated.
  string conf_path = src_path.AsStr() + ".jhm";

  if (ExistsPath(src_path.AsStr().c_str())) {
    return Files.Add(move(name), make_unique<TFile>(move(src_path), true, conf_path));
  } else {
    TAbsPath out_path(Out, name);
    return Files.Add(move(name), make_unique<TFile>(move(out_path), false, conf_path));
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

TFile *TEnv::TryGetFileFromPath(const std::string &name) {
  assert(&name);
  assert(name.size()); // Name must be non-empty.
  // If the name starts with a '/' it's an absolute filesystem path
  if(name[0] == '/') {
    if (Src.Contains(name)) {
      return GetFile(Src.GetAbsPath(name).GetRelPath());
    } else if (Out.Contains(name)) {
      return GetFile(Out.GetAbsPath(name).GetRelPath());
    } else {
      // File isn't in a known tree, so we can't possibly get it.
      return nullptr;
    }
  } else {
    return GetFile(TRelPath(name));
  }
}