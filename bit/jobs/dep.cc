#include <bit/jobs/dep.h>

#include <chrono>
#include <fstream>

#include <base/thrower.h>
#include <bit/file_environment.h>
#include <bit/file_info.h>
#include <bit/jobs/dep_c.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

static unordered_set<TRelPath> GetOutputName(const TRelPath &input) {
  return {TRelPath(input.AddExtension(".dep"))};
}

static TOpt<TRelPath> TryGetInputName(const TRelPath &output) {
  auto result = output.TryRemoveExtension(".dep");
  if (result) {
    return *result;
  } else {
    return TOpt<TRelPath>();
  }
}

TJobProducer TDep::GetProducer(const TJobConfig &job_config) {
  return TJobProducer{"dependency_file",
                      {".dep"},
                      TryGetInputName,
                      GetOutputName,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [job_config](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TDep>(new TDep(std::move(metadata), &job_config));
                      }};
}

TJson ToJson(const unordered_set<TFileInfo *> &that) {
  vector<TJson> json_elems;
  json_elems.reserve(that.size());
  for(const auto &item: that) {
    json_elems.push_back(TJson(item->CmdPath));
  }

  return TJson(move(json_elems));
}

#include <iostream>

TJob::TOutput TDep::Run(TFileEnvironment *file_env) {
  TJob::TOutput result;

  // Figure out which dep program to run, the right arguments for it

  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const TRelPath &input_path = GetInput()->RelPath;

  vector<string> cmd;
  // Only C, C++ are supported currently.
  assert(input_path.EndsWith(".cc") || input_path.EndsWith(".c"));

  if (input_path.EndsWith(".cc")) {
    // TODO(cmaloney): Make a helper utility in <jobs/compile_c_family.h> to get the command.
    cmd.push_back("clang++");
    // cmd.push_back(Bit::GetCmd<Tools::Cc>(Env.GetConfig()));
  } else {
    cmd.push_back("clang");
    // cmd.push_back(Bit::GetCmd<Tools::C>(Env.GetConfig()));
  }

  // TODO: move array append
  // for (auto &arg : TCompileCFamily::GetStandardArgs(GetInput(), ext == "cc", Env)) {
  //  cmd.push_back(move(arg));
  //}
  cmd.push_back("-M");
  cmd.push_back("-MG");

  cmd.push_back("-std=c++14");
  cmd.push_back("-Wall");
  cmd.push_back("-Werror");
  cmd.push_back("-Wextra");
  cmd.push_back("-Wno-unused");
  cmd.push_back("-Wno-unused-result");
  cmd.push_back("-Wno-unused-parameter");
  cmd.push_back("-fcolor-diagnostics");
  cmd.push_back("-Qunused-arguments");

  cmd.push_back(GetInput()->CmdPath);

  TOutput output;

  output.Subprocess = Base::Subprocess::Run(cmd);

  // Process the output
  if (output.Subprocess.ExitCode != 0) {
    output.Result = TJob::TOutput::Error;
    return output;
  }

  output.Result = TJob::TOutput::CompleteIfNeeds;
  if (output.Subprocess.Output->HasOverflowed()) {
    THROWER(std::overflow_error) << "Dependency list exceeded stdout exceeded the max length of "
                                    "the cyclic buffer used for streaming out of subprocesses and "
                                    "guaranteeing they can't use infinite length buffers.";
  }

  auto deps = ParseDeps(output.Subprocess.Output->ToString());
  for(const auto &dep: deps) {
    auto rel_path = file_env->TryGetRelPath(dep);
    if (rel_path) {
      Needs.insert(file_env->GetFileInfo(*rel_path));
    }
  }

  // TODO(cmaloney): Only generate and write out when the job is complete.
  // TODO(cmaloney): Writing the dep file doesn't actually provide us any
  // benefit, but is required by the work finder since it is a file expected to
  // exist.
  // TODO(cmaloney): Stream from C++ struct -> json rather than doing this
  // "copy into something that looks like JSON".
  /* out_file */ {
    cout << AsStr("Writing deps file: ", GetSoleOutput()->CmdPath, "\n");
    auto deps_json = ToJson(Needs);
    ofstream out_file(GetSoleOutput()->CmdPath);
    out_file.exceptions(std::ifstream::failbit);
    deps_json.Write(out_file);
  }

  output.Needs.Mandatory = Needs;
  return output;
}

string TDep::GetConfigId() const {
  // TODO(cmaloney): Make stable.
  static const auto now = std::chrono::system_clock::to_time_t(chrono::system_clock::now());
  return AsStr(ctime(&now));
}

// TODO(cmaloney): Return the json which IsComplete used to do below
std::unordered_map<TFileInfo *, TJobConfig> TDep::GetOutputExtraData() const { return {}; }
#if 0
bool TDep::IsComplete() {
  assert(this);

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
#endif

// TODO(cmaloney): Use the job config to get real arguments.
TDep::TDep(TMetadata &&metadata, const TJobConfig *) : TJob(move(metadata)) {}
