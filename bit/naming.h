#pragma once

#include <string>

#include <base/exception.h>

namespace Bit {

/* Represents the base of a bunch of files/folders that will be worked with together.

The Path contained inside must always start with '/' (be absolute) and should always
end with a slash */

EXCEPTION(TInvalidPath, std::runtime_error, nullptr);

// TODO(cmaloney): operator+ for joining paths?
// Starts with a '/'. Ends with a directory name followed by '/'.
struct TTree {

  TTree(std::string path);

  static TTree Find(std::string start_dir, const std::string &marker);

  const std::string Path;
}; // TTree

// RelPath doesn't start with '/'. Ends with a filename (not '/').
struct TRelPath {
  explicit TRelPath(const std::string &rel_path);
  const std::string Path;

  bool operator==(const TRelPath &that) const;
}; // TPath

// AbsPath starts with '/'. Ends with a filename (not '/').
struct TAbsPath {
  explicit TAbsPath(const TTree &tree, const TRelPath &rel_path);
  const std::string Path;

  TAbsPath AddExtension(const char *extension);

  private:
  TAbsPath(std::string &&path);
}; // TPath

} // Bit

namespace std {
  template <> struct hash<Bit::TRelPath>
  {
    size_t operator()(const Bit::TRelPath &rel_path) const;
  };
}
