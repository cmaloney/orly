#include <bit/jobs/dep_wrap_compiler.h>

#include <base/thrower.h>
#include <bit/file_environment.h>
#include <bit/job.h>
#include <bit/jobs/compile_c_family.h>
#include <bit/jobs/dep.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

// TOOD(cmaloney): Technically this should all be utf-8...
static inline bool local_isgraph(uint8_t character) { return character > 32 && character < 127; }

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
  for (uint32_t i = 0; i < bytes_available; ++i) {
    const uint8_t c = gcc_deps.GetByte(i);
    if (!eaten_start) {
      if (c == ':') {
        eaten_start = true;
      }
      continue;
    }

    if (in_tok) {
      // Still in token?
      if (local_isgraph(c)) {
        buffer.push_back(char(c));
        continue;
      }

      // Hit end of token. Grab token and submit.
      // NOTE: If token is '\', then discard (Indicates GCC's line continuation)
      if (buffer != "\\") {
        if (is_first_item) {
          is_first_item = false;
        } else {
          deps.insert(buffer);
        }
      }
      buffer.resize(0);
      in_tok = false;
    } else {
      // Hit start of token?
      if (local_isgraph(c)) {
        buffer.push_back(char(c));
        in_tok = true;
      }
    }
  }

  return deps;
}

unordered_set<string> Bit::Jobs::GetDepsWrappingCompiler(TDep *job, TJob::TOutput &output) {
  // Figure out which dep program to run, the right arguments for it

  // TODO(cmaloney): de-dupe this between here and dep_wrap_compiler.cc
  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const TRelPath &input_path = job->GetInput()->RelPath;

  // Only C, C++ are supported currently.
  assert(input_path.EndsWith(".cc") || input_path.EndsWith(".c"));

  vector<string> cmd = input_path.EndsWith(".cc") ? TCompileCFamily::GetBaseCppArgs()
                                                  : TCompileCFamily::GetBaseCArgs();

  cmd.push_back("-M");
  cmd.push_back("-MG");
  cmd.push_back(job->GetInput()->CmdPath);

  output.Subprocess = Base::Subprocess::Run(cmd);

  // Process the output if there wasn't an error
  if (output.Subprocess.ExitCode != 0) {
    output.Result = TJob::TOutput::Error;
    return {};
  }

  output.Result = TJob::TOutput::CompleteIfNeeds;
  if (output.Subprocess.Output->HasOverflowed()) {
    THROWER(std::overflow_error)
        << "Dependency list exceeded the max supported length of the stdout"
           " cyclic buffer used for streaming out of subprocesses and "
           "guaranteeing they don't eat all memory.";
  }

  return ParseDeps(*output.Subprocess.Output);
}
