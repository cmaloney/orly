/* Unit test command line options. */
#pragma once

#include <cmd/args.h>

namespace Test {

struct TOptions {
  bool Verbose;
};

// TODO(cmaloney): Move to .cc
/* Used in the inital setting of options, the canonical store.*/
inline TOptions &GetOptionsNonConst() {
  static TOptions Options;
  return Options;
}

// TODO(cmaloney): Move to .cc
inline const TOptions &GetOptions() { return GetOptionsNonConst(); }
}