#pragma once

namespace Bit {

// TODO(cmaloney): Allow adding arbitrary file types later, map from string of file type -> integer. Use integers everywhere.
enum class TFileType {
  Executable,
  Object,
  CSource,
  CppSource,
  DependencyFile,
  Unset
};

}
