#include <bit/jobs/compile_c_family.h>

#include <chrono>

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
  assert(input.EndsWith(".o"));
  return {TRelPath(input.SwapExtension(IsCc ? ".cc" : ".c", ".o"))};
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

#include <base/not_implemented.h>
TJob::TOutput TCompileCFamily::Run() {

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
