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

#include <fstream>

#include <base/split.h>
#include <jhm/env.h>
#include <jhm/file.h>
#include <jhm/jobs/compile_c_family.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;

static TRelPath GetOutputName(const TRelPath &input) {
  return input.AddExtension({"dep"});
}

static TOpt<TRelPath> GetInputName(const TRelPath &output) {
  if (EndsWith(output.GetName().GetExtensions(), {"dep"})) {
    return output.DropExtension(1);
  } else {
    return *TOpt<TRelPath>::Unknown;
  }
}

TJobProducer TDep::GetProducer() {

  return TJobProducer{
    "dep",
    {{"dep"}},
    GetInputName,
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TDep>(new TDep(env, in_file));
    }
  };
}

const char *TDep::GetName() {
  return "dependency_file";
}

const unordered_set<TFile*> TDep::GetNeeds() {
  assert(this);

  return Needs;
}

string TDep::GetCmd() {
  assert(this);
  ostringstream oss;

  // TODO: add a helper for output set -> output directory
  oss << "make_dep_file " << GetInput()->GetPath() << ' ' << GetSoleOutput()->GetPath().AsStr();

  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the compiler
  const auto &extensions = GetInput()->GetPath().GetRelPath().GetName().GetExtensions();
  if (extensions.size() >= 1) {
    const string &ext = extensions.at(extensions.size()-1);
    if (ext == "cc" || ext == "c") {
      oss << ' ';
      TCompileCFamily::AddStandardArgs(GetInput(), ext == "cc", Env, oss);
    }
  }

  return oss.str();
}

bool TDep::IsComplete() {
  assert(this);

  bool needs_work = false;

  // Load the json file and see if there are any new things in it which aren't yet done (We have work to do)
  // TODO: This should be a call to Parse()... But that constructs an istringstream...
  TJson deps = TJson::Read(GetSoleOutput()->GetPath().AsStr().c_str());
  deps.ForEachElem([this,&needs_work](const TJson &elem)->bool {
    TFile *file = Env.TryGetFileFromPath(elem.GetString());
    if (file) {
      // Add to needs. If it's new in the Needs array, we aren't done yet.
      needs_work |= Needs.insert(file).second;
    }
    return true;
  });

  // If we found everything, stash the info on the input file as computed config
  // TODO: In pushing this as strings, we effectively discard all the file lookups we've already done
  if (!needs_work) {
    GetSoleOutput()->PushComputedConfig(TJson::TObject{{"c++", TJson::TObject{{"include", move(deps)}}}});
  }

  return !needs_work;
}


TDep::TDep(TEnv &env, TFile *in_file)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetPath().GetRelPath()))}), Env(env) {}