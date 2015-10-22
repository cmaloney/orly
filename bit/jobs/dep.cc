/* <jhm/jobs/dep.cc>

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

#include <jhm/jobs/dep.h>

#include <fstream>
#include <ostream>

#include <bit/file_info.h>
#include <bit/jobs/dep_c.h>
#include <bit/jobs/util.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Job;
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

  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const auto &extensions = GetInput()->GetRelPath().Path.Extension;
  const string &ext = extensions.at(extensions.size() - 1);

  vector<string> cmd;
  // Only C, C++ are supported currently.
  assert(extensions.size() >= 1);
  assert(ext == "cc" || ext == "c");

  if(ext == "cc") {
    // TODO(cmaloney): Make a helper utility in <jobs/compile_c_family.h> to get the command.
    cmd.push_back(Bit::GetCmd<Tools::Cc>(Env.GetConfig()));
  } else {
    cmd.push_back(Bit::GetCmd<Tools::C>(Env.GetConfig()));
  }

  // TODO: move array append
  for(auto &arg : TCompileCFamily::GetStandardArgs(GetInput(), ext == "cc", Env)) {
    cmd.push_back(move(arg));
  }
  cmd.push_back("-M");
  cmd.push_back("-MG");
  cmd.push_back(GetInput()->GetPath());
  return cmd;
}

void TDep::ProcessOutput(TFd stdout, TFd) {
  NeedsWork = false;
  Deps = ParseDeps(ReadAll(move(stdout)));
  for (const auto &dep: Deps) {
    TFile *file = Env.TryGetFileFromPath(dep);
    if(file) {
      // Add to needs. If it's new in the Needs array, we aren't done yet.
      NeedsWork |= Needs.insert(file).second;
    }
  }
}


TJson ToJson(vector<string> &&that) {
  vector<TJson> json_elems(that.size());
  for(uint64_t i = 0; i < that.size(); ++i) {
    json_elems[i] = TJson(move(that[i]));
  }

  return TJson(move(json_elems));
}

bool TDep::IsComplete() {
  assert(this);

  if (NeedsWork) {
    return false;
  }

  // Everything has been found, save the information.
  auto deps_json = ToJson(move(Deps));

  // TODO(cmaloney): Writing the dep file doesn't actually provide us any
  // benefit, but is required by the work finder since it is a file expected to
  // exist.
  // TODO(cmaloney): Stream from C++ struct -> json rather than doing this
  // "copy into something that looks like JSON".
  /* out_file */ {
    ofstream out_file(GetSoleOutput()->GetPath());
    deps_json.Write(out_file);
  }

  // Stash the info on the input file as computed config
  // TODO: In pushing this as strings, we effectively discard all the file
  // lookups we've already
  // done
  GetSoleOutput()->PushComputedConfig(
      TJson::TObject{{"c++", TJson::TObject{{"include", move(deps_json)}}}});

  return true;
}

TDep::TDep(TEnv &env, TFile *in_file)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetRelPath()))}), Env(env) {}
