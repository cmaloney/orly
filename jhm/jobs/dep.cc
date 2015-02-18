/* <jhm/jobs/dep.cc>

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

#include <jhm/jobs/dep.h>

#include <jhm/env.h>
#include <jhm/file.h>
#include <jhm/jobs/compile_c_family.h>
#include <jhm/jobs/util.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace Util;

static TRelPath GetOutputName(const TRelPath &input) {
  return TRelPath(AddExtension(TPath(input.Path), {"dep"}));
}

static TOpt<TRelPath> GetInputName(const TRelPath &output) {
  if(output.Path.EndsWith({"dep"})) {
    return TRelPath(DropExtension(TPath(output.Path), 1));
  } else {
    return TOpt<TRelPath>();
  }
}

TJobProducer TDep::GetProducer() {
  return TJobProducer{"dep",
                      {{"dep"}},
                      GetInputName,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [](TEnv &env, TFile *in_file)
                          -> unique_ptr<TJob> { return unique_ptr<TDep>(new TDep(env, in_file)); }};
}

const char *TDep::GetName() { return "dependency_file"; }

const unordered_set<TFile *> TDep::GetNeeds() {
  assert(this);

  return Needs;
}

vector<string> TDep::GetCmd() {
  assert(this);

  vector<string> cmd{"make_dep_file", "--", GetInput()->GetPath(), GetSoleOutput()->GetPath()};

  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const auto &extensions = GetInput()->GetRelPath().Path.Extension;
  if(extensions.size() >= 1) {
    const string &ext = extensions.at(extensions.size() - 1);
    if(ext == "cc" || ext == "c") {
      if(ext == "cc") {
        // TODO(cmaloney): Make a helper utility in <jobs/compile_c_family.h> to get the command.
        cmd.push_back(Jhm::GetCmd<Tools::Cc>(Env.GetConfig()));
      } else {
        cmd.push_back(Jhm::GetCmd<Tools::C>(Env.GetConfig()));
      }
      // TODO: move array append
      for(auto &arg : TCompileCFamily::GetStandardArgs(GetInput(), ext == "cc", Env)) {
        cmd.push_back(move(arg));
      }
    }
  }
  return cmd;
}

TTimestamp TDep::GetCmdTimestamp() const {
  // TODO(cmaloney): This is definitely the wrong timestamp...
  return Jhm::GetCmdTimestamp<Tools::Cc>(Env.GetConfig());
}

bool TDep::IsComplete() {
  assert(this);

  bool needs_work = false;

  // Load the json file and see if there are any new things in it which aren't yet done (We have
  // work to do)
  // TODO: This should be a call to Parse()... But that constructs an istringstream...
  TJson deps = TJson::Read(AsStr(GetSoleOutput()->GetPath()).c_str());
  for(const TJson &elem : deps.GetArray()) {
    TFile *file = Env.TryGetFileFromPath(elem.GetString());
    if(file) {
      // Add to needs. If it's new in the Needs array, we aren't done yet.
      needs_work |= Needs.insert(file).second;
    }
  }

  // If we found everything, stash the info on the input file as computed config
  // TODO: In pushing this as strings, we effectively discard all the file lookups we've already
  // done
  if(!needs_work) {
    GetSoleOutput()->PushComputedConfig(
        TJson::TObject{{"c++", TJson::TObject{{"include", move(deps)}}}});
  }

  return !needs_work;
}

TDep::TDep(TEnv &env, TFile *in_file)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetRelPath()))}), Env(env) {}