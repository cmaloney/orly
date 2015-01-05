/* <jhm/naming.cc>

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

#include <jhm/naming.h>

#include <iomanip>
#include <stdexcept>

#include <base/split.h>
#include <base/thrower.h>
#include <util/path.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace Util;

TTree TTree::Find(string start_dir, const char *marker) {
  bool found_marker = false;
  TTree res = TTree::Find(move(start_dir), marker, found_marker);
  if (!found_marker) {
    THROW_ERROR(runtime_error) << "Unable to find " << quoted(marker) << " indicating root of tree";
  }
  return res;
}

TTree TTree::Find(string working, const char *marker, bool &found_marker) {
  do {
    string test_dir = working + '/' + marker;
    if (ExistsPath(test_dir.c_str())) {
      found_marker = true;
      return TTree(working);
    }
    working.resize(working.rfind('/'));
  } while (working.size());
  return TTree(GetCwd());
}

TTree::TTree(vector<string> &&root) : Root(move(root)) {
  if(!IsValidNamespace(Root)) {
    THROW_ERROR(runtime_error) << "Invalid tree / namespace";
  }
}

TTree::TTree(const string &name) : Root(SplitNamespace(name)) {

  if (!IsValidNamespace(Root)) {
    THROW_ERROR(runtime_error) << "Invalid tree / namespace " << quoted(name);
  }
}

bool TTree::Contains(const Base::TPath &path) const {
  return Root.size() <= path.Namespace.size() && equal(Root.begin(), Root.end(), path.Namespace.begin());
}

TPath TTree::GetAbsPath(TPath path) const {
  path.Namespace.insert(path.Namespace.begin(), Root.begin(), Root.end());
  return path;
}

TPath TTree::GetAbsPath(TRelPath rel_path) const {
  return GetAbsPath(move(rel_path.Path));
}

TRelPath TTree::GetRelPath(TPath &&path) const {
  assert(Contains(path));

  path.Namespace.erase(path.Namespace.begin(), path.Namespace.begin() + Root.size());
  return TRelPath(move(path));
}

bool TRelPath::operator==(const TRelPath &that) const {
  return Path == that.Path;
}
bool TRelPath::operator!=(const TRelPath &that) const {
  return Path != that.Path;
}


ostream &Jhm::operator<<(ostream &out, const TRelPath &that) {
  return WriteExtension(WriteNamespace(out, that.Path.Namespace, false) << that.Path.Name, that.Path.Extension);
}

ostream &Jhm::operator<<(ostream &out, const TTree &that) {
  return out << '/' << Join(that.Root, '/');
}

size_t hash<TRelPath>::operator()(const TRelPath &rel_path) const {
  return std::hash<TPath>()(rel_path.Path);
}