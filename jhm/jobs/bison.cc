/* <jhm/jobs/bison.cc>

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

#include <jhm/jobs/bison.h>

#include <jhm/env.h>
#include <jhm/file.h>
#include <jhm/jobs/util.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace Util;

const static vector<vector<string>> OutExtensions = {
  {"bison","cc"},
  {"bison", "hh"}
};

//TODO: this is duplicated / copied in both bison.cc and flex.cc
static TOpt<TRelPath> GetInputName(const TRelPath &output) {
  for (const auto &ext : OutExtensions) {
    if (output.Path.EndsWith(ext)) {
      return TRelPath(AddExtension(DropExtension(TPath(output.Path), ext.size()), {"y"}));
    }
  }
  return TOpt<TRelPath>();
}


TJobProducer TBison::GetProducer() {

  return TJobProducer{
    "bison",
    OutExtensions,
    GetInputName,
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TJob>(new TBison(env, in_file));
    }
  };
}

const char *TBison::GetName() {
  return "bison";
}

const unordered_set<TFile*> TBison::GetNeeds() {
  return unordered_set<TFile*>();
}

vector<string> TBison::GetCmd() {
  assert(this);
  TFile *primary_output = GetOutputWithExtension(GetOutput(), {"cc"});
  return vector<string>{"bison","-rall","-o" + primary_output->GetPath(), GetInput()->GetPath()};
}


TTimestamp TBison::GetCmdTimestamp() const {
  static TTimestamp timestamp = GetTimestampSearchingPath("bison");
  return timestamp;
}

bool TBison::IsComplete() {
  TFile *cc = GetOutputWithExtension(GetOutput(), {"cc"});
  cc->PushComputedConfig(
      TJson::TObject{{"cmd", TJson::TObject{{"g++", Env.GetConfig().GetEntry({"cmd", "bison", "g++"})}}}});
  return true;
}


TBison::TBison(TEnv &env, TFile *in_file)
    : TJob(in_file, GetOutputSet(OutExtensions, env, in_file->GetRelPath())), Env(env) {}