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

#include <base/split.h>
#include <jhm/env.h>
#include <jhm/file.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace std::placeholders;
using namespace Util;

vector<string> TCompileCFamily::GetStandardArgs(TFile *input, bool is_cpp, const TEnv &env) {

  // Add options from configuration. Per-file config overrides global config
  vector<string> options;
  // TODO: Make file configuration automatically attach environment to the tail of it's list for lookups / fallback?
  if (!input->GetConfig().TryRead({"cmd", is_cpp ? "g++" : "gcc"}, options)) {
    env.GetConfig().TryRead({"cmd", is_cpp ? "g++" : "gcc"}, options);
  }

  const auto src_str = AsStr(*env.GetSrc());

  // Add the src and out directories as sources of includes.
  options.push_back("-I" + src_str);
  options.push_back("-I" + AsStr(*env.GetOut()));

  // Let the code know where the root of the tree was (So it can remove the SRC prefix if needed)
  options.push_back("-DSRC_ROOT=\"" + src_str + "/\"");
  return options;
}

static TRelPath GetOutputName(const TRelPath &input, bool is_cpp) {
  assert(input.Path.EndsWith({is_cpp ? "cc" : "c"}));
  return TRelPath(SwapExtension(TPath(input.Path), {"o"}));
}

static TOpt<TRelPath> GetInputName(const TRelPath &output, bool is_cpp) {
  if (output.Path.EndsWith({"o"})) {
    return TRelPath(SwapExtension(TPath(output.Path), {is_cpp ? "cc" : "c"}));
  } else {
    return TOpt<TRelPath>();
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

vector<string> TCompileCFamily::GetCmd() {
  assert(this);

  // Build up the gcc call
  // add output, input filenames
  // Tell GCC we're only compiling to a .o
  vector<string> cmd{IsCpp ? "clang++" : "clang", "-o" + GetSoleOutput()->GetPath(), GetInput()->GetPath(), "-c"};

  /* Add standard arguments */ {
    //TODO: Really need an insertion move here...
    //TODO: Trivial vector append (Can we add an operator+ for rvalue rhs?)
    auto std_args = GetStandardArgs(GetInput(), IsCpp, Env);
    for(auto &arg: std_args) {
      cmd.push_back(move(arg));
    }

  }

  return cmd;
}


TTimestamp TCompileCFamily::GetCmdTimestamp() const {
  static TTimestamp gcc_timestamp = GetTimestampSearchingPath("clang");
  static TTimestamp gpp_timestamp = GetTimestampSearchingPath("clang++");

  return IsCpp ? gpp_timestamp : gcc_timestamp;
}

bool TCompileCFamily::IsComplete() {
  assert(this);

  // Calculate the files which need to be linked against to make a binary with this file.
  // TODO: capture the negative (Does not exist / unproducable) .o information which is inherent in the final list as
  //       when those files come into existence, we need to recompute the list.

  // TODO: We needlessly jump to strings here. Really should be able to stash away TFile * within a TFile's config.
  TJson::TArray filtered_includes;
  for (const string &include : Need->GetConfig().Read<vector<string>>({"c++","include"})) {
    // TODO: We really only need the TRelPaths here, not jumping all the way to the file objects.
    TFile *include_file = Env.TryGetFileFromPath(include);
    if (include_file) {
      filtered_includes.push_back(AsStr(include_file->GetPath()));
    }
  }

  GetSoleOutput()->PushComputedConfig(
      TJson::TObject{{"c++", TJson::TObject{{"filtered_includes", move(filtered_includes)}}}});

  return true;
}

TCompileCFamily::TCompileCFamily(TEnv &env, TFile *in_file, bool is_cpp)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetRelPath(), is_cpp))}),
      Env(env),
      IsCpp(is_cpp),
      Need(Env.GetFile(TRelPath(AddExtension(TPath(GetInput()->GetRelPath().Path), {"dep"})))) {}
