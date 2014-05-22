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

using namespace std;
using namespace Base;
using namespace Jhm;

std::unordered_set<TJob *> TJobFactory::GetPotentialJobs(TFile *out_file) {
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
    for(auto &producer : JobProducers) {
      TFile *in = producer.TryGetInput(out_file->GetPath().GetRelPath());
      if (!in) {
        continue;
      }

      //TODO: FIXME
      TFile *in_2 = in;
      TJob *job = Jobs.Add(move(in_2), producer.MakeJob(in));

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
      Out(GetOutDirName(root.Get(), proj_name, config, config_mixin)) {

  // TODO: Include trees (useful for multi-repo JHM)

  /* TODO
  // Register all the compilation jobs/tasks we know about
  //NOTE: Each of these effectively builds up a theoretical path to creating the output file. The file is commited by calling "Register" on the root.
  Register("Compile C++", {"cc"}, {{"o"}}, [this](const TAbsPath &path) -> TJob * {
    return make_unique<TCFamilyJob>(*this, path);
    if (job->InputCanExist()) {
      Register(move(job));
      job->RegisterInAndOut()
    }
    auto in_f = make_unique<TGenericFile>(input_file)


    if(in_f->CanExist()) {
      Register(move(in_f));
    }
  });
  Register("Make C++ dependency file", {"cc"}, {{"cc", "dep"}}, [](const TAbsPath &path) -> TJob * { return new TCFamilyJob(path); });
  Register("C++ dependency file", {"cc","dep"}, [](const TAbsPath &path) -> TFile * { return new TJsonFile(path); })
  Register("C++ source file", {"cc"}, []())
  */
}

TFile *TEnv::TryFindFile(TRelPath name) {

  // If we've been found before, just return what was found.
  if (TFile *f = Files.TryGet(name)) {
    return f;
  }

  TAbsPath abs_path(Src, name);

  //NOTE: seperate line because we move / steal the contents of abs_path in the make_unique call.
  bool path_exists = ExistsPath(abs_path.AsStr().c_str());

  // If in the source tree, then make a placeholder file for that particular extension.
  return Files.Add(std::move(name), make_unique<TFile>(std::move(abs_path), path_exists));
}