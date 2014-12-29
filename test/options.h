/* Unit test command line options. */
#pragma once
namespace Test {

struct TOptions {
  bool PrintTiming;
  bool Verbose;
};

inline const TArgs<TOptions> &GetArgs() {
  static TArgs<TOptions> Args{
    Optional({"verbose", "v"}, &TOptions::Verbose, "Show the results of unit tests, regardless of whether they pass or fail"),
  };

  return Args;
}


//TODO(cmaloney): Move to .cc
const TOptions &GetOptions() {
  return GetOptionsNonConst();
}


/* Used in the inital setting of options, the canonical store.*/
TOptions &GetOptionsNonConst() {
  static TOptions Options;
  return Options;
}
