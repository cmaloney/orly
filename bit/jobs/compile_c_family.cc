#include <bit/jobs/compile_c_family.h>

#include <chrono>

#include <bit/file_environment.h>
#include <bit/job_producer.h>

using namespace Base;
using namespace Bit;
using namespace Jobs;
using namespace std;

// TODO(cmaloney): Get out args from config
#if 0
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

#endif

template <bool IsCc>
static unordered_set<TRelPath> GetOutputName(const TRelPath &input) {
  constexpr static const char *ext = IsCc ? ".cc" : ".c";
  assert(input.EndsWith(ext));
  return {TRelPath(input.SwapExtension(ext, ".o"))};
}

template <bool IsCc>
static TOpt<TRelPath> TryGetInputName(const TRelPath &output) {
  TOpt<TRelPath> result = output.TryRemoveExtension(".o");
  if (result) {
    return result->AddExtension(IsCc ? ".cc" : ".c");
  }
  return TOpt<TRelPath>();
}

TJobProducer TCompileCFamily::GetCProducer(const TJobConfig &job_config) {
  return TJobProducer{"compile_c",
                      {".c"},
                      TryGetInputName<false>,
                      GetOutputName<false>,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [job_config](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TCompileCFamily>(
                            new TCompileCFamily(std::move(metadata), &job_config, false));
                      }};
}

TJobProducer TCompileCFamily::GetCcProducer(const TJobConfig &job_config) {
  return TJobProducer{"compile_cc",
                      {".cc"},
                      TryGetInputName<true>,
                      GetOutputName<true>,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [job_config](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TCompileCFamily>(
                            new TCompileCFamily(std::move(metadata), &job_config, true));
                      }};
}

#include <base/split.h>
#include <iostream>

TJob::TOutput TCompileCFamily::Run(TFileEnvironment *file_environment) {
  TJob::TOutput output;
  output.Result = TJob::TOutput::NewNeeds;

  // TODO(cmaloney): The dep file is very much a "second mandatory input".
  if (!Needs) {
    Needs = file_environment->GetFileInfo(GetInput()->RelPath.AddExtension(".dep"));
    output.Needs.Mandatory = unordered_set<TFileInfo*>{Needs};
    return output;
  }

  // Since the dep file requires all the .h files in order to complete, if it is
  // complete everything this file touches / includes is complete. Compile!
  vector<string> cmd;
  if (IsCc) {
    cmd.push_back("clang++");
  } else {
    cmd.push_back("clang");
  }

  cmd.push_back("-c");

  // TODO(cmaloney): cmd.push_back(Bit::GetCmd<Tools::Cc>(Env.GetConfig()));
  cmd.push_back("-std=c++14");
  cmd.push_back("-Wall");
  cmd.push_back("-Werror");
  cmd.push_back("-Wextra");
  cmd.push_back("-Wno-unused");
  cmd.push_back("-Wno-unused-result");
  cmd.push_back("-Wno-unused-parameter");
  cmd.push_back("-fcolor-diagnostics");
  cmd.push_back("-Qunused-arguments");
  cmd.push_back("-I/home/firebird347/projects/jhm");
  cmd.push_back("-o");
  cmd.push_back(GetSoleOutput()->CmdPath);

  cmd.push_back(GetInput()->CmdPath);

  std::cout << Base::Join(cmd, " ") << std::endl;

  output.Subprocess = Base::Subprocess::Run(cmd);
  // Process the output
  if (output.Subprocess.ExitCode != 0) {
    output.Result = TJob::TOutput::Error;
    return output;
  }


  // Compilation successful!
  output.Result = TJob::TOutput::Complete;
  return output;
}

string TCompileCFamily::GetConfigId() const {
  // TODO(cmaloney): Make stable.
  static const auto now = std::chrono::system_clock::to_time_t(chrono::system_clock::now());
  return AsStr(ctime(&now));
}
std::unordered_map<TFileInfo *, TJobConfig> TCompileCFamily::GetOutputExtraData() const {
  return {};
}
TCompileCFamily::TCompileCFamily(TMetadata &&metadata, const TJobConfig *, bool is_cc)
    : TJob(move(metadata)), IsCc(is_cc) {}
