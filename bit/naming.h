#pragma once

#include <string>

#include <base/exception.h>

namespace Bit {

/* Represents the base of a bunch of files/folders that will be worked with together.

The Path contained inside must always start with '/' (be absolute) and should always
end with a slash */

EXCEPTION(TInvalidPath, std::runtime_error, nullptr);

// TODO(cmaloney): operator+ for joining paths?
struct TTree {

  TTree(std::string path);

  static TTree Find(std::string start_dir, const std::string &marker);

  const std::string Path;
}; // TTree

// AbsPath starts with '/'. Ends with a filename (not '/').
struct TAbsPath {
  TAbsPath(const TTree &tree, const std::string &rel_path);
  const std::string Path;
}; // TPath

// RelPath doesn't start with '/'. Ends with a filename (not '/').
struct TRelPath {
  TRelPath(const TTree &tree, const std::string &rel_path);
  const std::string Path;
}; // TPath

}; // Bit
