// Command line options to bit.
#pragma once

#include <string>
#include <thread>
#include <vector>

namespace Bit {

struct TOptions {
  // TODO(cmaloney): Move this constructor to a .cc to reduce header dependencies?
  TOptions() : WorkerCount(std::thread::hardware_concurrency()) {}
  bool PrintCmd = false;
  std::vector<std::string> Mixins;
  std::vector<std::string> Targets;
  uint64_t WorkerCount;
};  // TOptions

}  // Bit
