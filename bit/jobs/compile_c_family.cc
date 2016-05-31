#include <bit/jobs/compile_c_family.h>

#include <chrono>

#include <base/json_util.h>
#include <base/not_implemented.h>
#include <bit/file_environment.h>
#include <bit/job_producer.h>
#include <util/stl.h>

using namespace Base;
using namespace Bit;
using namespace Jobs;
using namespace std;
using namespace Util;

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
template<bool IsCc>
vector<string> ExtractBaseArgs(const TJobConfig *job_config) {
  static TOpt<vector<string>> Args;

  if (job_config) {
    assert(!Args);
    std::string section_name = IsCc ? "compile_cc" : "compile_c";
    Args = vector<string>{IsCc ? "clang++" : "clang"};

    auto it = job_config->find(section_name);
    if (it != job_config->end()) {
      Append(*Args, ExtractOptional<vector<string>>(it->second, {"flags"}));
    } else {
      Args.MakeKnown();
    }
  }

  return *Args;
}
#pragma clang diagnostic pop

vector<string> TCompileCFamily::GetBaseCArgs() {
  return ExtractBaseArgs<false>(nullptr);
}

vector<string> TCompileCFamily::GetBaseCppArgs() {
  return ExtractBaseArgs<true>(nullptr);
}

TJobProducer TCompileCFamily::GetCProducer(const TJobConfig &job_config) {
  ExtractBaseArgs<false>(&job_config);
  return TJobProducer{"compile_c",
                      TFileType::CSource,
                      {TFileType::Object},
                      TryGetInputName<false>,
                      GetOutputName<false>,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TCompileCFamily>(
                            new TCompileCFamily(std::move(metadata), false));
                      }};
}

TJobProducer TCompileCFamily::GetCcProducer(const TJobConfig &job_config) {
  ExtractBaseArgs<true>(&job_config);
  return TJobProducer{"compile_cc",
                      TFileType::CppSource,
                      {TFileType::Object},
                      TryGetInputName<true>,
                      GetOutputName<true>,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TCompileCFamily>(
                            new TCompileCFamily(std::move(metadata), true));
                      }};
}

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
  vector<string> cmd = IsCc ? GetBaseCppArgs() : GetBaseCArgs();

  cmd.push_back("-c");
  cmd.push_back("-o");
  cmd.push_back(GetSoleOutput()->CmdPath);

  cmd.push_back(GetInput()->CmdPath);

  output.Subprocess = Subprocess::Run(cmd);
  // Process the output
  if (output.Subprocess.ExitCode != 0) {
    output.Result = TJob::TOutput::Error;
    return output;
  }

  // TODO(cmaloney): This is doing a ton of excess computation converting back into fileinfo objects.
  for (const auto &dep: Needs->GetCompleteConfig()->JobConfig.at("dependencies").GetArray()) {
    TOpt<TRelPath> dep_rel_path = file_environment->TryGetRelPath(dep.GetString());
    if (dep_rel_path && dep_rel_path->EndsWith(".h")) {
      TFileInfo *link_file = file_environment->GetFileInfo(dep_rel_path->SwapExtension(".h", ".o"));
      LinkNeeds.push_back(link_file->CmdPath);
    } else {
      NOT_IMPLEMENTED_S("C/C++ deps which don't end in .h");
    }
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

  // Add Link needs of the .o
  return {{GetSoleOutput(), TJobConfig{{"try_link", move(LinkNeeds)}}}};
}

TCompileCFamily::TCompileCFamily(TMetadata &&metadata, bool is_cc)
    : TJob(move(metadata)), IsCc(is_cc) {}
