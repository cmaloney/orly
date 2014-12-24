/* JHM Command line options */

#include <string>
#include <thread>
#include <vector>

#include <cmd/args.h>

namespace Jhm {

struct TOptions {
  // TODO(cmaloney): Move this constructor to a .cc to reduce header dependencies?
  TOptions() : WorkerCount(std::thread::hardware_concurrency()) {}
  bool PrintCmd = false;
  std::string Config = "debug";
  std::string ConfigMixin;
  std::string PrintTests;
  bool RunTests = false;
  bool VerboseTests = false;  // TODO(cmaloney): generalize to flags for tests.
  uint32_t WorkerCount;
  std::vector<std::string> Targets;
}; // TOptions

inline Cmd::TArgCollection<TOptions> GetOptions() {
  return {
    Cmd::Optional("print-cmd", &TOptions::PrintCmd, "Print commands when they are run"),
    Cmd::Optional({"config", "c"}, &TOptions::Config, "Build the software in the given configuration"),
    Cmd::Optional({"mixin", "m"}, &TOptions::ConfigMixin, "Configuration mixin"),
    Cmd::Optional("print-test", &TOptions::PrintTests, "Write a list of tests to the given filename"),
    Cmd::Optional("run-tests", &TOptions::RunTests, "Run the unit tests"),
    Cmd::Optional("verbose-tests", &TOptions::VerboseTests, "Run the tests in verbose mode"),
    Cmd::Optional("worker-count", &TOptions::WorkerCount, "Max number of commands to run at once"),
    Cmd::Required(&TOptions::Targets, Cmd::TRepetition::OneOrMore, "targets", "List of files to try to produce")};

}

} // Jhm