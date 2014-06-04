/* <jhm/jobs/c_dep.h>

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

#include <jhm/jobs/c_dep.h>

#include <base/assert_true.h>
#include <jhm/env.h>
#include <jhm/file.h>

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

const char *TCDep::GetName() {
  return "dependency_file";
}

const unordered_set<TFile*> TCDep::GetNeeds() {
  assert(this);
  return {GetInput()};
}

string TCDep::GetCmd() {
  assert(this);
  ostringstream oss;

  // TODO: Make this into a helper function to go from output set -> output file (Asserting only one)
  // TODO: Also add a helper for output set -> output directory
  string out_path;
  for(TFile *f: GetOutput()) {
    assert(out_path.size() == 0); // We should only have one output
    out_path = f->GetPath().AsStr();
  }

  oss << "make_dep_file " << GetInput()->GetPath() << ' ' << out_path;

  //TODO: Append arguments which would be passed to compiler
  //TODO: Add output directory
  return oss.str();
}


TJobProducer TCDep::GetProducer() {

  return TJobProducer{
    {{"dep"}},
    GetInputName,
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TCDep>(new TCDep(env, in_file));
    }
  };
}

TCDep::TCDep(TEnv &env, TFile *in_file)
    : TJob(in_file, {AssertTrue(env.TryFindFile(GetOutputName(in_file->GetPath().GetRelPath())))}) {}