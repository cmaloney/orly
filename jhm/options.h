/* JHM Command line options */
#pragma once

#include <string>
#include <thread>
#include <vector>

namespace Jhm {

struct TOptions {
  // TODO(cmaloney): Move this constructor to a .cc to reduce header dependencies?
  TOptions() : WorkerCount(std::thread::hardware_concurrency()) {}
  bool PrintCmd = false;
  std::string Config = "debug";
  std::string ConfigMixins;
  std::string PrintTests;
  bool DisableDefaultJobs = false;
  bool RunTests = false;
  bool VerboseTests = false;  // TODO(cmaloney): generalize to flags for tests.
  uint64_t WorkerCount;
  std::vector<std::string> Targets;
};  // TOptions

}  // Jhm
