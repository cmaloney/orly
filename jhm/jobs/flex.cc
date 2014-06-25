/* <jhm/jobs/flex.cc>

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

#include <jhm/jobs/flex.h>

#include <sstream>

#include <jhm/env.h>
#include <jhm/file.h>
#include <jhm/jobs/util.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace std::placeholders;

const static vector<TExtension> OutExtensions = {
  {"flex","cc"},
};

//TODO: this is duplicated / copied in both bison.cc and flex.cc
static TOpt<TRelPath> GetInputName(const TRelPath &output) {
  for (const auto &ext : OutExtensions) {
    if (EndsWith(output.GetName().GetExtensions(), ext)) {
      return output.DropExtension(ext.size()).AddExtension({"l"});
    }
  }
  return TOpt<TRelPath>();
}

TJobProducer TFlex::GetProducer() {

  return TJobProducer{
    "flex",
    OutExtensions,
    GetInputName,
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TFlex>(new TFlex(env, in_file));
    }
  };
}

const char *TFlex::GetName() {
  return "flex";
}

const unordered_set<TFile*> TFlex::GetNeeds() {
  return unordered_set<TFile*>();
}

string TFlex::GetCmd() {
  assert(this);
  ostringstream oss;
  oss << "flex -o " << GetSoleOutput()->GetPath() << ' ' << GetInput()->GetPath();

  return oss.str();
}

bool TFlex::IsComplete() {
  GetSoleOutput()->PushComputedConfig(
      TJson::TObject{{"cmd", TJson::TObject{{"g++", Env.GetConfig().GetEntry({"cmd","flex","g++"})}}}});
  return true;
}


TFlex::TFlex(TEnv &env, TFile *in_file)
    : TJob(in_file, GetOutputSet(OutExtensions, env, in_file->GetPath().GetRelPath())), Env(env) {}