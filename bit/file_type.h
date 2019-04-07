#pragma once

namespace Bit {

// TODO(cmaloney): Allow adding arbitrary file types later, map from string of file type -> integer. Use integers everywhere.
enum class TFileType {
  Executable = 1,
  Object = 2,
  CSource = 3,
  CppSource = 4,
  DependencyFile = 5
};

}
