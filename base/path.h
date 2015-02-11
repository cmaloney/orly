/* <base/path.h>

   Class to allow easy access and manipulation of various chunks of a filename.

   Constructors of the class guarantee that if they succeed / don't throw

   The utility functions provided which operate on a Path manipulating it __GUARANTEE__ that the
   resulting path is
   valid.

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

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <vector>

#include <base/class_traits.h>
#include <base/exception.h>

namespace Base {

/* Design notes:
    - Value type, because otherwise we write a lot of crap to get and manipulate specific members
    - Helper functions rather than members to make it easier to modify in place intelligently /
   composably.
    - Seperate name and namespace so '/foo/.baz' is representable without the last namespace being
   empty being a
      special condition
    - Extensions can be empty. Just means there are dots next to eachother. If the last extension is
   empty, it isn't
      prefixed with a '.' (The last extension being empty is used for executables)
*/

class TPath {
  public:
  using TStr = std::string;

  DEFINE_ERROR(TInvalid, std::runtime_error, "Invalid path")

  // TODO: A vector<string> which is contiguous / one array rather than array of pointers would
  // speed a lot of this
  // class up.
  using TStrList = std::vector<TStr>;

  /* Copy and move construction is legal. */
  DEFAULT_COPY(TPath);
  DEFAULT_MOVE(TPath);

  /* Construct off a provided string, parsing it into applicable chunks. TPath always ends up valid.
   */
  explicit TPath(const char *path);
  explicit TPath(const char *path, uint32_t len);
  explicit TPath(const TStr &name);

  /* Construct from a specific pre-parsed list of chunks. */
  TPath(TStrList ns, TStr name, TStrList extension);
  TPath(TStrList ns_and_name, TStrList extension);
  TPath(TStr name, TStrList extension);
  TPath(const TStr &dir, TStr name, TStrList extension);

  /* Check that the path is valid / legal. Returns false if it isn't */
  explicit operator bool() const;

  bool operator==(const TPath &that) const;
  bool operator!=(const TPath &that) const;

  bool EndsWith(const std::vector<std::string> &extension) const;

  std::vector<std::string> ToNamespaceIncludingName() const;

  std::ostream &Write(std::ostream &out) const;

  TStrList Namespace;
  TStr Name;
  TStrList Extension;
};

bool IsValidName(const std::string &name);
bool IsValidNamespace(const std::vector<std::string> &ns);
bool IsValidExtension(const std::vector<std::string> &extension);

std::vector<std::string> SplitNamespace(const std::string &dir);

std::ostream &WriteNamespace(std::ostream &out,
                             const std::vector<std::string> &ns,
                             bool leading_slash = true);

// NOTE: If last extension is empty, don't print a dot.
std::ostream &WriteExtension(std::ostream &out, const std::vector<std::string> &extension);

TPath AddExtension(TPath &&that, const std::vector<std::string> &new_extension);
TPath DropExtension(TPath &&that, uint32_t count);
TPath SwapExtension(TPath &&that, const std::vector<std::string> &new_extension);

std::ostream &operator<<(std::ostream &out, const TPath &path);
}  // Base

namespace std {
template <>
struct hash<Base::TPath> {
  size_t operator()(const Base::TPath &that) const;
};
}