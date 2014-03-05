/* <stig/type/addr.cc>

   Implements <stig/type/addr.h>.

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

#include <stig/type/addr.h>

#include <base/split.h>

using namespace Stig::Type;

IMPL_INTERNED_TYPE(TAddr, TAddrElems);

TAddr::~TAddr() {}

void TAddr::Write(std::ostream &strm) const {
  assert(this);
  assert(&strm);

  #if 0
  strm << "Stig::Rt::TAddr<";
  Base::Join(", ", GetElems(), [](const std::pair<TAddrDir, TType> &elem, std::ostream &strm) {
    strm << "Stig::Rt::TAddrElem<";
    WriteCppType(strm, elem.first);
    strm << ", " << elem.second << '>';
  }, strm);
  strm << '>';
  #endif
  strm << "std::tuple<";
  Base::Join(", ", GetElems(), [](const std::pair<TAddrDir, TType> &elem, std::ostream &strm) {
    switch (elem.first) {
      case Stig::TAddrDir::Asc: {
        strm << elem.second;
        break;
      }
      case Stig::TAddrDir::Desc: {
        strm << "Stig::TDesc<" << elem.second << ">";
        break;
      }
    }
  }, strm);
  strm << '>';
}