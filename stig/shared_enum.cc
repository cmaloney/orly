/* <stig/shared_enum.cc>

   Implements <stig/shared_enum.cc>.

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

#include <stig/shared_enum.h>

#include <cassert>

#include <base/no_default_case.h>

using namespace Stig;

std::ostream &Stig::Write(std::ostream &strm, TAddrDir dir) {
  assert(&strm);
  switch (dir) {
    case TAddrDir::Asc: {
      strm << "asc";
      break;
    }
    case TAddrDir::Desc: {
      strm << "desc";
      break;
    }
    NO_DEFAULT_CASE;
  }
  return strm;
}


std::ostream &Stig::WriteCppType(std::ostream &strm, TAddrDir dir) {
  assert(&strm);
  strm << "Stig::TAddrDir::";
  switch (dir) {
    case TAddrDir::Asc: {
      strm << "Asc";
      break;
    }
    case TAddrDir::Desc: {
      strm << "Desc";
      break;
    }
    NO_DEFAULT_CASE;
  }
  return strm;
}

std::ostream &Stig::operator<<(std::ostream &strm, TAddrDir dir) {
  return Write(strm, dir);
}
