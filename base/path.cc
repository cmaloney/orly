/* <base/path.cc>

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

#include <base/path.h>

#include <algorithm>

#include <base/split.h>
#include <base/thrower.h>

using namespace std;
using namespace Base;

// TODO: Write an optimal variant for this and the length version. We don't need to construct
// strings.
TPath::TPath(const char *path) : TPath(string(path)) {}
TPath::TPath(const char *path, uint32_t len) : TPath(string(path, len)) {}
TPath::TPath(const TStr &name) : Namespace(SplitNamespace(name)) {
  string name_ext;
  if(Namespace.size() > 0) {
    name_ext = move(Namespace.back());
    Namespace.pop_back();
  }
  if(name_ext.size() > 0) {
    Split(".", name_ext, Extension);
    if(Extension.size() > 0) {
      Name = move(Extension.front());
      Extension.erase(Extension.begin());
    }

    if(Extension.size() == 0) {
      Extension = {""s};
    }
  }

  // Validate
  if(!*this) {
    THROW_ERROR(TPath::TInvalid) << name;
  }
}
TPath::TPath(TStrList ns, TStr name, TStrList extension)
    : Namespace(move(ns)), Name(move(name)), Extension(move(extension)) {
  if(!*this) {
    THROW_ERROR(TPath::TInvalid) << *this;
  }
}
TPath::TPath(TStrList ns_and_name, TStrList extension) : Extension(move(extension)) {
  if(!ns_and_name.empty()) {
    Name = move(ns_and_name.back());
    ns_and_name.pop_back();
    Namespace = move(ns_and_name);
  }

  if(!*this) {
    THROW_ERROR(TPath::TInvalid);
  }
}
TPath::TPath(const TStr &dir, TStr name, TStrList extension)
    : Namespace(SplitNamespace(dir)), Name(move(name)), Extension(move(extension)) {
  if(!*this) {
    // TODO: Print extension lists here in such a way that doesn't possibly hide errors in
    // individaul extensions.
    THROW_ERROR(TPath::TInvalid) << "dir: " << dir << " name: " << Name;
  }
}

bool TPath::EndsWith(const vector<string> &ext) const {
  if(Extension.size() < ext.size()) {
    return false;
  }
  return equal(ext.begin(), ext.end(), Extension.end() - ext.size());
}

vector<string> TPath::ToNamespaceIncludingName() const {
  vector<string> ns(Namespace);
  ns.push_back(Name);
  return ns;
}

TPath::operator bool() const {
  return IsValidNamespace(Namespace) && IsValidName(Name) && IsValidExtension(Extension);
}

bool TPath::operator==(const TPath &that) const {
  return Namespace == that.Namespace && Name == that.Name && Extension == that.Extension;
}

bool TPath::operator!=(const TPath &that) const { return !(*this == that); }

bool Base::IsValidName(const string &name) {
  // NOTE: Names can be zero length (ex: .foobar)
  return name.find_first_of("./") == string::npos;
}

bool Base::IsValidNamespace(const vector<string> &ns_list) {
  return all_of(ns_list.begin(), ns_list.end(), [](const string &ns) {
    // The IsValidName checks for '.' which implicitly checks '..' for us, making sure the path
    // isn't relative.
    return !ns.empty() && ns.find_first_of("/") == string::npos;
  });
}

bool Base::IsValidExtension(const vector<string> &extensions) {
  return all_of(extensions.begin(), extensions.end(), IsValidName);
}

vector<string> Base::SplitNamespace(const string &dir) {
  std::vector<string> ret;
  if(dir.size() <= 0) {
    return ret;
  }

  auto start = dir.front() == '/' ? 1 : 0;

  auto end = dir.size() - start;
  if(dir.size() > 1 && dir.back() == '/') {
    end -= 1;
  }

  Split("/", dir.substr(start, end), ret);
  return ret;
}

ostream &Base::WriteNamespace(ostream &out, const vector<string> &ns, bool leading_slash) {
  if(leading_slash) {
    out << '/';
  }

  out << Join(ns, '/');

  if(!ns.empty()) {
    out << '/';
  }
  return out;
}

ostream &Base::WriteExtension(ostream &out, const vector<string> &extension) {
  const auto ext_end = extension.end();

  for(auto it = extension.begin(); it != ext_end; ++it) {
    if(it == ext_end - 1 && it->empty()) {
      continue;
    }
    out << '.' << *it;
  }
  return out;
}

TPath Base::AddExtension(TPath &&that, const vector<string> &new_extension) {
  assert(that);
  that.Extension.insert(that.Extension.end(), new_extension.begin(), new_extension.end());
  return that;
}

TPath Base::DropExtension(TPath &&that, uint32_t count) {
  assert(that);
  assert(that.Extension.size() >= count);
  that.Extension.resize(that.Extension.size() - count);
  return that;
}

TPath Base::SwapExtension(TPath &&that, const vector<string> &new_extension) {
  assert(that);
  assert(that.Extension.size());
  that.Extension.pop_back();
  that.Extension.insert(that.Extension.end(), new_extension.begin(), new_extension.end());
  return that;
}

ostream &Base::operator<<(ostream &out, const TPath &path) {
  return WriteExtension(WriteNamespace(out, path.Namespace) << path.Name, path.Extension);
}

size_t hash<TPath>::operator()(const TPath &rel_path) const {
  // TODO(cmaloney): Switch to a more efficient hash generation than building
  // a string.
  return hash<std::string>()(AsStr(rel_path));
}