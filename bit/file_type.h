#pragma once

namespace Bit {

// TODO(cmaloney): Extension -> type guess map (and programmable per project, ex. .cc vs. .cpp, vs. cxx)
// TODO(cmaloney): Allow overriding / explicitly stating type next to any individual file.
// TODO(cmaloney): Add string mime-type which can be used if working with interpreters / externally
// specified jobs to map
// TODO(cmaloney): Allow adding arbitrary file types later, map from string of file type -> integer. Use integers everywhere.
enum class TFileType {
  Unset = 0,
  Executable = 1,
  Object = 2,
  CSource = 3,
  CppSource = 4,
  DependencyFile = 5
};

}
