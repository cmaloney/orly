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

#include <base/not_implemented.h>
#include <base/path_utils.h>
#include <jhm/jobs/compile_c_family.h>
#include <jhm/jobs/dep.h>
#include <jhm/jobs/link.h>

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
        job->AddOutput(out_file);
      } else {
        // Make the new job
        job = Jobs.Add(TJobDesc{in, producer.Name}, producer.MakeJob(env, in));
        if (job->HasUnknownOutputs()) {
          job->AddOutput(out_file);
        }
      }


      // TODO: InsertOrFail.
      ret.insert(job);

      // Fill cache
      bool found_self = false;
      for(TFile *job_out : job->GetOutput()) {
        // TODO: InsertOrFail.
        JobsByOutput.emplace(job_out, job);
        found_self |= (job_out == out_file);
      }
      assert(found_self); // Job must output the file it said it would.
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

TEnv::TEnv(const TAbsBase &root, const string &proj_name, const string &config, const string &config_mixin)
    : Root(root),
      Src('/' + Root.Get() + '/' + proj_name),
      Out(GetOutDirName('/' + root.Get(), proj_name, config, config_mixin)),
      Config(ReadConfig('/' + Src.Get() + '/' + config + ".jhm")) {

  // Load the configuation
  // root/src/config.jhm.mixin root/src/config.jhm -> root/config.jhm
  auto add_conf_if_exists = [this] (const string &filename, bool back) {
    if (ExistsPath(filename.c_str())) {
      if (back) {
        Config.PushBack(ReadConfig(filename));
      } else {
        Config.Push(ReadConfig(filename));
      }
    }
  };

  // Add the project root overrides if they exist (Comes after project config)
  add_conf_if_exists('/' + Root.Get() + '/' + config_mixin + ".jhm_mixin", true);
  add_conf_if_exists('/' + Root.Get() + '/' + config + ".jhm", true);

  // Add config mixin if it exists (Comes before project config)
  add_conf_if_exists('/' + Src.Get() + config_mixin + ".jhm_mixin", false);

  // TODO: Include trees (useful for multi-repo JHM)

  Jobs.Register(Job::TDep::GetProducer());
  Jobs.Register(Job::TCompileCFamily::GetCProducer());
  Jobs.Register(Job::TCompileCFamily::GetCppProducer());
  Jobs.Register(Job::TLink::GetProducer());
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
  string conf_path = TAbsPath(Src, name.AddExtension({"jhm"})).AsStr();
  if (ExistsPath(conf_path.c_str())) {
    file_conf = ReadConfig(conf_path);
  }

  if (ExistsPath(src_path.AsStr().c_str())) {
    return Files.Add(move(name), make_unique<TFile>(move(src_path), true, move(file_conf)));
  } else {
    TAbsPath out_path(Out, name);
    return Files.Add(move(name), make_unique<TFile>(move(out_path), false, move(file_conf)));
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