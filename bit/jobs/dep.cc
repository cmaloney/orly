#include <bit/jobs/dep.h>

#include <chrono>
#include <fstream>

#include <base/json_util.h>
#include <base/thrower.h>
#include <bit/file_environment.h>
#include <bit/file_info.h>
#include <bit/jobs/compile_c_family.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

// TOOD(cmaloney): Technically this should all be utf-8...
static inline bool local_isgraph(uint8_t character) {
  return character > 32 && character < 127;
}

unordered_set<string> ParseDeps(const TCyclicBuffer &gcc_deps) {
  unordered_set<string> deps;

  bool in_tok = false;
  bool eaten_start = false;
  bool is_first_item = true;

  std::string buffer;
  buffer.reserve(1024);

  // Convert it to a happy format
  // Remove leading .o:
  // Remove leading source file (Comes right after the ':')
  // Grab each token as a string.
  // If the string is '\' then it's a linebreak GCC added...
  auto bytes_available = gcc_deps.GetBytesAvailable();
  for(uint32_t i = 0; i < bytes_available; ++i) {
    const uint8_t c = gcc_deps.GetByte(i);
    if(!eaten_start) {
      if(c == ':') {
        eaten_start = true;
      }
      continue;
    }

    if(in_tok) {
      // Still in token?
      if(local_isgraph(c)) {
        buffer.push_back(c);
        continue;
      }

      // Hit end of token. Grab token and submit.
      // NOTE: If token is '\', then discard (Indicates GCC's line continuation)
      if (buffer != "\\") {
        if(is_first_item) {
          is_first_item = false;
        } else {
          deps.emplace(buffer);
        }
      }
      buffer.clear();
      in_tok = false;
    } else {
      // Hit start of token?
      if(local_isgraph(c)) {
        buffer.push_back(c);
        in_tok = true;
      }
    }
  }

  return deps;
}

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
                      TFileType::Unset,
                      {TFileType::DependencyFile},
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

TJob::TOutput TDep::Run(TFileEnvironment *file_env) {
  TJob::TOutput result;

  // Figure out which dep program to run, the right arguments for it

  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const TRelPath &input_path = GetInput()->RelPath;

  // Only C, C++ are supported currently.
  assert(input_path.EndsWith(".cc") || input_path.EndsWith(".c"));

  vector<string> cmd = input_path.EndsWith(".cc") ? TCompileCFamily::GetBaseCppArgs()
                                                  : TCompileCFamily::GetBaseCArgs();

  cmd.push_back("-M");
  cmd.push_back("-MG");
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
    THROWER(std::overflow_error) << "Dependency list exceeded the max supported length of the stdout"
                                    " cyclic buffer used for streaming out of subprocesses and "
                                    "guaranteeing they don't eat all memory.";
  }

  auto deps = ParseDeps(*output.Subprocess.Output);
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
std::unordered_map<TFileInfo *, TJobConfig> TDep::GetOutputExtraData() const {
  return {{GetSoleOutput(), TJobConfig{{"dependencies", ToJson(Needs)}}}};
}

// TODO(cmaloney): Use the job config to get real arguments.
TDep::TDep(TMetadata &&metadata, const TJobConfig *) : TJob(move(metadata)) {}
