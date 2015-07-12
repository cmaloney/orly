#pragma once

#include <string>

namespace Bit {

// TODO(cmaloney): operator+ for joining paths?
struct TTree {

  TTree(const std::string &path);

  static TTree Find(std::string start_dir, std::string marker );

  const std::string Path;
}; // TTree

struct TAbsPath {
  TAbsPath(const TTree &tree, const std::string &rel_path);
  const std::string Path;
}; // TPath

}; // Bit
