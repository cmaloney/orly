#include <bit/jobs/run_test.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include <base/as_str.h>
#include <bit/file_info.h>
#include <bit/job_producer.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

static unordered_set<TRelPath> GetOutputName(const TRelPath &input) {
  return {input.AddExtension(".out")};
}

static optional<TRelPath> TryGetInputName(const TRelPath &output) {
  return output.TryRemoveExtension(".out");
}

TJobProducer TRunTest::GetProducer(const TJobConfig &job_config) {
  return TJobProducer{"run_test",
                      TFileType::Executable,
                      {TFileType::Unset},
                      TryGetInputName,
                      GetOutputName,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [job_config](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TRunTest>(new TRunTest(std::move(metadata), &job_config));
                      }};
}

TJob::TOutput TRunTest::Run(TFileEnvironment *) {
  TJob::TOutput output;

  // TODO(cmaloney): Input should be an implicitly completed before any runs thing.
  if (!GetInput()->IsComplete()) {
    std::cout << "NOT done...\n";
    output.Result = TJob::TOutput::NewNeeds;
    output.Needs.Mandatory.insert(GetInput());
    return output;
  }
  // TODO(cmaloney): Stop spawning extra pumps in the Run() calls.
  output.Subprocess = Subprocess::Run({GetInput()->CmdPath});

  // TODO(cmaloney): Actually write the test stdout to the file.
  /* out_file */ {
    ofstream out_file(GetSoleOutput()->CmdPath);
    out_file.exceptions(std::ifstream::failbit);
    out_file << "FOOBAR";
  }

  // Process the output
  if (output.Subprocess.ExitCode != 0) {
    output.Result = TJob::TOutput::Error;
    return output;
  }
  output.Result = TJob::TOutput::Complete;
  return output;
}

string TRunTest::GetConfigId() const {
  // TODO(cmaloney): Make stable.
  static const auto now = std::chrono::system_clock::to_time_t(chrono::system_clock::now());
  return AsStr(ctime(&now));
}

std::unordered_map<TFileInfo *, TJobConfig> TRunTest::GetOutputExtraData() const { return {}; }

TRunTest::TRunTest(TMetadata &&metadata, const TJobConfig *) : TJob(move(metadata)) {}
