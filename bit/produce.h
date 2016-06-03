/* Tries to build the requested files in the given environment.

Manages file state and the queue to actually build things, continually
trying to get out of work / produce what the user requested. */

#pragma once

#include <string>
#include <vector>

namespace Bit {

class TEnvironment;

// Figure out the jobs which need to be run to create the given set of targets
// and run them to create the targets.
void DoProduce(uint64_t worker_count, TEnvironment &environment, std::vector<std::string> Targets, bool include_tests);

}  // Bit
