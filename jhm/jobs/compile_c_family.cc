/* <jhm/jobs/compile_c_family.cc>

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

#include <jhm/jobs/compile_c_family.h>

#include <sstream>

#include <base/split.h>
#include <jhm/env.h>
#include <jhm/file.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace std::placeholders;


void TCompileCFamily::AddStandardArgs(bool is_cpp, TEnv &env, std::ostream &out) {
  Join(" ", env.GetConfig().Read<vector<string>>(is_cpp ? "cmd.g++" : "cmd.gcc"), out);

  // Add the src and out directories as sources of includes.
  out << " -I" << env.GetSrc() << " -I" << env.GetOut()
      // Let the code know where the root of the tree was (So it can remove the SRC prefix if needed)
      << " -D'SRC_ROOT=\"" << env.GetSrc() << "/\"'";

}

static TRelPath GetOutputName(const TRelPath &input, bool is_cpp) {
  assert(EndsWith(input.GetName().GetExtensions(), {is_cpp ? "cc" : "c"}));
  return input.SwapLastExtension("o");
}

static TOpt<TRelPath> GetInputName(const TRelPath &output, bool is_cpp) {
  if (EndsWith(output.GetName().GetExtensions(), {"o"})) {
    return output.SwapLastExtension(is_cpp ? "cc" : "c");
  } else {
    return *TOpt<TRelPath>::Unknown;
  }
}

TJobProducer TCompileCFamily::GetCProducer() {
  return TJobProducer{
    "compile_c",
    {{"o"}},
    bind(GetInputName, _1, false),
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TJob>(new TCompileCFamily(env, in_file, false));
    }
  };
}

TJobProducer TCompileCFamily::GetCppProducer() {
  return TJobProducer{
    "compile_cpp",
    {{"o"}},
    bind(GetInputName, _1, true),
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TJob>(new TCompileCFamily(env, in_file, true));
    }
  };
}

const char *TCompileCFamily::GetName() {
  if (IsCpp) {
    return "compile_cpp";
  } else {
    return "compile_c";
  }
}

const unordered_set<TFile*> TCompileCFamily::GetNeeds() {
  assert(this);

  // Only thing needed is the dep file. The dep file by being done enusures
  // all includes exist for C/C++. Set is constructed in ctor so that we don't construct it all the time.
  return {Need};
}

string TCompileCFamily::GetCmd() {
  assert(this);

  // Build up the gcc call
  ostringstream oss;
  if (IsCpp) {
    oss << "g++";
  } else {
    oss << "gcc";
  }

  // add output, input filenames
  oss << " -o" << GetSoleOutput()->GetPath() << ' ';
  TFile *input = GetInput();
  if (input->IsSrc()) {
    oss << input->GetPath().GetRelPath();
  } else {
    oss << input->GetPath();
  }

  // Tell GCC we're only compiling to a .o
  oss << " -c ";

  // Add standard arguments
  AddStandardArgs(IsCpp, Env, oss);

  return oss.str();
}

bool TCompileCFamily::IsComplete() {
  assert(this);

  // Calculate the files which need to be linked against to make a binary with this file.
  // TODO: capture the negative (Does not exist / unproducable) .o information which is inherent in the final list as
  //       when those files come into existence, we need to recompute the list.

  // TODO: We needlessly jump to strings here. Really should be able to stash away TFile * within a TFile's config.
  TJson::TArray link_needs;
  for (const string &include : Need->GetConfig().Read<vector<string>>("c++.include")) {
    // TODO: We really only need the TRelPaths here, not jumping all the way to the file objects.
    TFile *include_file = Env.TryGetFileFromPath(include);
    if (include_file) {
      TFile *obj_file = Env.GetFile(include_file->GetPath().GetRelPath().SwapLastExtension("o"));
      if (Env.IsBuildable(obj_file)) {
        link_needs.push_back(obj_file->GetPath().AsStr());
      }
    }
  }

  GetSoleOutput()->PushComputedConfig(TJson::TObject{{"ld", TJson::TObject{{"link", move(link_needs)}}}});

  return true;
}

TCompileCFamily::TCompileCFamily(TEnv &env, TFile *in_file, bool is_cpp)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetPath().GetRelPath(), is_cpp))}),
      Env(env),
      IsCpp(is_cpp),
      Need(Env.GetFile(GetInput()->GetPath().GetRelPath().AddExtension({"dep"}))) {}