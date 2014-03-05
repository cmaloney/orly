/* <stig/package/name.cc>

   Implements <stig/package/name.h>

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/package/name.h>

#include <sstream>

#include <base/convert.h>

using namespace Base;
using namespace Jhm;
using namespace Stig::Package;

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

TVersionedName TVersionedName::Parse(const TPiece<const char> &name) {
  assert(&name);

  auto dot_pos = find('.', name.GetLimit() - 1, name.GetStart() - 1);
  if (!dot_pos) {
    return TVersionedName{Jhm::TNamespace(name), 0};
  }
  return TVersionedName{Jhm::TNamespace(TPiece<const char>(name.GetStart(), dot_pos)),
      TConvertProxy(TPiece<const char>(dot_pos + 1, name.GetLimit()))};
}

bool TVersionedName::operator==(const TVersionedName &that) const {
  assert(this);
  assert(&that);

  return that.Version == Version && that.Name == Name;
}

TRelPath TVersionedName::GetSoRelPath() const {
  assert(this);

  std::ostringstream out;
  out << Version;
  std::string ext = out.str();
  out.str("");
  return Name.ToRelPath({ext,"so"});
}

std::ostream &Stig::Package::operator<<(std::ostream &out, const TVersionedName &that) {
  assert(&out);
  assert(&that);

  out << that.Name << '.' << that.Version;

  return out;
}