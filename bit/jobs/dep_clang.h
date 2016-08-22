#pragma once

#include <string>
#include <unordered_set>

#include <bit/job.h>

namespace Bit {

namespace Jobs {

class TDep;

// TODO(cmaloney): Make output be returned / thrown rather than a return argument.
std::unordered_set<std::string> GetDepsUsingClang(TDep *dep, TJob::TOutput &output);
}
}
