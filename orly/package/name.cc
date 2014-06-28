/* <orly/package/name.cc>

   Implements <orly/package/name.h>

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

#include <orly/package/name.h>

#include <iomanip>
#include <sstream>

#include <base/convert.h>
#include <base/path.h>

using namespace Base;
using namespace Orly::Package;
using namespace std;

//TODO: This should really live on piece somehow, or as some base algorithm.
static inline const char *find(char sep, const char *start, const char *limit) {
  assert(start);
  assert(limit);

  const int step = start <= limit ? 1 : -1;

  for(;start != limit; start += step) {
    if(*start == sep) {
      return start;
    }
  }
  return 0;
}

TName TName::Parse(const std::string &name) {
  TName res;
  Split("/", name, res.Name);

  if (!IsValidNamespace(res.Name)) {
    THROW_ERROR(std::runtime_error) << "Invalid package name " << std::quoted(name);
  }
  return res;
}

TVersionedName TVersionedName::Parse(const TPiece<const char> &name) {
  assert(&name);

  auto dot_pos = find('.', name.GetLimit() - 1, name.GetStart() - 1);
  if (!dot_pos) {
    //TODO: Excess copy here.
    string tmp;
    return {TName::Parse(Assign(tmp, name)), 0};
  }
  //TODO: Excess copy here
  string tmp;
  return {TName::Parse(Assign(tmp, TPiece<const char>(name.GetStart(), dot_pos))),
                        TConvertProxy(TPiece<const char>(dot_pos + 1, name.GetLimit()))};
}

bool TVersionedName::operator==(const TVersionedName &that) const {
  assert(this);
  assert(&that);

  return that.Version == Version && that.Name == Name;
}

Jhm::TRelPath TVersionedName::GetSoRelPath() const {
  assert(this);

  vector<string> name(Name.Name);
  name.pop_back();

  return Jhm::TRelPath(TPath(move(name), Name.Name.back(), {to_string(Version), "so"}));
}

ostream &Orly::Package::operator<<(ostream &out, const TVersionedName &that) {
  assert(&out);
  assert(&that);

  out << that.Name << '.' << that.Version;

  return out;
}

ostream &Orly::Package::operator<<(ostream &out, const TName &that) {
  return out << Join(that.Name, '/');
}