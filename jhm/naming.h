/* <jhm/naming.h>

   Classes to make it hard(er) to get wrong the tree + relative path model JHM works on.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <base/path.h>

namespace Jhm {

class TRelPath;

// Trees contain files in relative paths.
class TTree {
  public:
  DEFAULT_MOVE(TTree)
  DEFAULT_COPY(TTree)

  /* Locate a tree by finding a designated marker name. If the marker isn't found in the first case
     we throw
     an exception. In the second case, we use the current working directory. */
  static TTree Find(std::string start_dir, const char *marker);
  static TTree Find(std::string start_dir, const char *marker, bool &found_marker);

  TTree() = default;

  TTree(std::vector<std::string> &&root);
  TTree(const std::string &path);

  // Returns true iff the path is prefixed with our root.
  bool Contains(const Base::TPath &path) const;

  Base::TPath GetAbsPath(Base::TPath path) const;
  Base::TPath GetAbsPath(TRelPath rel_path) const;

  /* Get a relative path from a string which is an absolute path.
     NOTE:
        Assumes path is absolute (starts with '/')
        Assumes tree contains path. */
  TRelPath GetRelPath(Base::TPath &&path) const;

  TTree AddDir(std::string &dir) const;

  std::vector<std::string> Root;
};

// TODO: Cope with prefixes (lib) specially?
// Relative paths
class TRelPath {
  public:
  DEFAULT_MOVE(TRelPath)
  DEFAULT_COPY(TRelPath)

  /* Construct off a path which isn't prefixed with a tree. Explicit because the programmer should
     really pay
     attention when using this constructor. */
  explicit TRelPath(Base::TPath &&path) : Path(path) {}

  bool operator==(const Jhm::TRelPath &that) const;
  bool operator!=(const Jhm::TRelPath &that) const;

  Base::TPath Path;
};

std::ostream &operator<<(std::ostream &out, const TRelPath &that);
std::ostream &operator<<(std::ostream &out, const TTree &that);
}

namespace std {
template <>
struct hash<Jhm::TRelPath> {
  size_t operator()(const Jhm::TRelPath &that) const;
};

} // Base
