/* <jhm/jobs/compile_c_family.cc>

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

#include <jhm/jobs/compile_c_family.h>

#include <base/split.h>
#include <jhm/env.h>
#include <jhm/file.h>
#include <jhm/jobs/util.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace std::placeholders;
using namespace Util;

vector<string> TCompileCFamily::GetStandardArgs(TFile *input, bool is_cc, const TEnv &env) {
  // Add options from configuration. Per-file config overrides global config
  vector<string> options;
  // TODO: Make file configuration automatically attach environment to the tail of it's list for
  // lookups / fallback?
  if(!input->GetConfig().TryRead({"options", is_cc ? "c++" : "c"}, options)) {
    env.GetConfig().TryRead({"options", is_cc ? "c++" : "c"}, options);
  }

  const auto src_str = AsStr(*env.GetSrc());

  // Add the src and out directories as sources of includes.
  options.push_back("-I" + src_str);
  options.push_back("-I" + AsStr(*env.GetOut()));

  // Let the code know where the root of the tree was (So it can remove the SRC prefix if needed)
  options.push_back("-DSRC_ROOT=\"" + src_str + "/\"");
  return options;
}

static TRelPath GetOutputName(const TRelPath &input, bool is_cc) {
  assert(input.Path.EndsWith({is_cc ? "cc" : "c"}));
  return TRelPath(SwapExtension(TPath(input.Path), {"o"}));
}

static TOpt<TRelPath> GetInputName(const TRelPath &output, bool is_cc) {
  if(output.Path.EndsWith({"o"})) {
    return TRelPath(SwapExtension(TPath(output.Path), {is_cc ? "cc" : "c"}));
  } else {
    return TOpt<TRelPath>();
  }
}

TJobProducer TCompileCFamily::GetCProducer() {
  return TJobProducer{"compile_c",
                      {{"o"}},
                      bind(GetInputName, _1, false),
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [](TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
    return unique_ptr<TJob>(new TCompileCFamily(env, in_file, false));
  }};
}

TJobProducer TCompileCFamily::GetCcProducer() {
  return TJobProducer{"compile_cc",
                      {{"o"}},
                      bind(GetInputName, _1, true),
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [](TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
    return unique_ptr<TJob>(new TCompileCFamily(env, in_file, true));
  }};
}

const char *TCompileCFamily::GetName() {
  if(IsCc) {
    return "compile_cc";
  } else {
    return "compile_c";
  }
}

const unordered_set<TFile *> TCompileCFamily::GetNeeds() {
  assert(this);

  // Only thing needed is the dep file. The dep file by being done enusures
  // all includes exist for C/C++. Set is constructed in ctor so that we don't construct it all the
  // time.
  return {Need};
}

vector<string> TCompileCFamily::GetCmd() {
  assert(this);

  // Build up the gcc call
  // add output, input filenames
  // Tell GCC we're only compiling to a .o
  vector<string> cmd{
      IsCc ? Jhm::GetCmd<Tools::Cc>(Env.GetConfig()) : Jhm::GetCmd<Tools::C>(Env.GetConfig()),
      "-o" + GetSoleOutput()->GetPath(),
      GetInput()->GetPath(),
      "-c"};

  /* Add standard arguments */ {
    // TODO: Really need an insertion move here...
    // TODO: Trivial vector append (Can we add an operator+ for rvalue rhs?)
    auto std_args = GetStandardArgs(GetInput(), IsCc, Env);
    for(auto &arg : std_args) {
      cmd.push_back(move(arg));
    }
  }

  return cmd;
}

TTimestamp TCompileCFamily::GetCmdTimestamp() const {
  return IsCc ? Jhm::GetCmdTimestamp<Tools::Cc>(Env.GetConfig())
               : Jhm::GetCmdTimestamp<Tools::C>(Env.GetConfig());
}

bool TCompileCFamily::IsComplete() {
  assert(this);

  // Calculate the files which need to be linked against to make a binary with this file.
  // TODO: We needlessly jump to strings here. Really should be able to stash away TFile * within a
  // TFile's config.
  TJson::TArray filtered_includes;
  for(const string &include : Need->GetConfig().Read<vector<string>>({"c++", "include"})) {
    // TODO: We really only need the TRelPaths here, not jumping all the way to the file objects.
    TFile *include_file = Env.TryGetFileFromPath(include);
    if(include_file) {
      filtered_includes.push_back(AsStr(include_file->GetPath()));
    }
  }

  GetSoleOutput()->PushComputedConfig(
      TJson::TObject{{"c++", TJson::TObject{{"filtered_includes", move(filtered_includes)}}}});

  return true;
}

TCompileCFamily::TCompileCFamily(TEnv &env, TFile *in_file, bool is_cc)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetRelPath(), is_cc))}),
      Env(env),
      IsCc(is_cc),
      Need(Env.GetFile(TRelPath(AddExtension(TPath(GetInput()->GetRelPath().Path), {"dep"})))) {}
