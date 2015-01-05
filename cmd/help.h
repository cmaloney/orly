/* Print a nicely formatted help message. */
#pragma once

#include <algorithm>
#include <numeric>

#include <cmd/args.h>

namespace Cmd {

/* Internal print function which does the actual printing. */
void PrintHelp(const char *program_name, const std::vector<const TArgInfo *> &args);

/* Prints a help message for the given bundles of arguments.

   Gathers all the args critical parts for printing together then reformats
   and prints the options to stdout.

   Templates to convert the arg groups to their core / peel away the type
   specific argument parsing, then passing to simple functions / classes for
   the actual printing. */
template <typename... TOptions>
void PrintHelp(const char *program_name, const TOptions &... args);

/* Collect the argument descriptions from a list of argument bundles */
template <typename... TOptions>
auto ExtractArgVector(const TArgs<TOptions> &... args) {
  std::vector<const TArgInfo *> result;

  // Calculate how big the final vector should be.
  std::initializer_list<size_t> sizes = {args.Info.size()...};
  result.reserve(std::accumulate(std::begin(sizes), std::end(sizes), 0u));

  // Gather the arguments into the result vector.
  // The initializer_list has a guaranteed evaluation order when expanded into.
  // This allows us to expand the grabs without recursion which is more
  // efficient.
  std::initializer_list<int>({[&result](const auto &arg_group) {
    std::for_each(std::begin(arg_group.Info),
                  std::end(arg_group.Info),
                  [&result](const auto &arg_info) { result.push_back(&arg_info); });
    return 0;
  }(args)...});
  return result;
}
// Base case for recursively collecting ArgInfo structs.
inline std::vector<const TArgInfo *> ExtractArgVector() { return std::vector<const TArgInfo *>(); }

// Recursively collect ArgInfo structs.
template <typename TOptions, typename... TRem>
std::vector<const TArgInfo *> ExtractArgVector(const TArgs<TOptions> &args, const TRem &... rem) {
  auto ret = ExtractArgVector(rem...);
  for(const auto &info : args.Info) {
    ret.push_back(&info);
  }
  return ret;
}

template <typename... TOptions>
void PrintHelp(const char *program_name, const TOptions &... args) {
  PrintHelp(program_name, ExtractArgVector(args...));
}
}