/* Print a nicely formatted help message. */
#pragma once

#include <cmd/args.h>

namespace Cmd {

  /* Internal print function which does the actual printing. */
  void PrintHelp(const char *program_name, const std::vector<const TArgInfo*> &args);

  /* Prints a help message for the given bundles of arguments.

     Gathers all the args critical parts for printing together then reformats
     and prints the options to stdout.

     Templates to convert the arg groups to their core / peel away the type
     specific argument parsing, then passing to simple functions / classes for
     the actual printing. */
  template <typename... TOptions>
  void PrintHelp(const char *program_name, const TOptions &...args);



  // Base case for recursively collecting ArgInfo structs.
  inline std::vector<const TArgInfo*> ExtractArgVector() {
    return std::vector<const TArgInfo*>();
  }

  // Recursively collect ArgInfo structs.
  template <typename TOptions, typename... TRem>
  std::vector<const TArgInfo*> ExtractArgVector(const TArgs<TOptions> &args, const TRem &...rem) {
    auto ret = ExtractArgVector(rem...);
    for (const auto &info: args.Info) {
      ret.push_back(&info);
    }
    return ret;
  }

  template <typename... TOptions>
  void PrintHelp(const char *program_name, const TOptions &...args) {
    PrintHelp(program_name, ExtractArgVector(args...));
  }

}