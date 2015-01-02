/* Unit test command line options. */
#pragma once

#include <cmd/args.h>

namespace Test {

struct TOptions {
  bool Verbose;
};

inline const Cmd::TArgs<TOptions> &GetArgs() {
  static Cmd::TArgs<TOptions> Args{
    Cmd::Optional({"verbose", "v"}, &TOptions::Verbose, "Show the results of unit tests, regardless of whether they pass or fail"),
  };

  return Args;
}

// TODO(cmaloney): Move to .cc
/* Used in the inital setting of options, the canonical store.*/
inline TOptions &GetOptionsNonConst() {
  static TOptions Options;
  return Options;
}

// TODO(cmaloney): Move to .cc
inline const TOptions &GetOptions() {
  return GetOptionsNonConst();
}

}