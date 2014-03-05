/* <stig/shared_enum.h>

   Useful stuff that all things related to TAddrs want.

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

#pragma once

#include <cassert>
#include <ostream>

namespace Stig {

  //Note: Not an enum class because writing addrs out by hand gets wayyyy to long.
  enum TAddrDir { Asc, Desc };

  //Useful for comparison, using in hashes.
  constexpr int ToInt(TAddrDir dir) {
    return dir == TAddrDir::Asc ? 1 : 0;
  }

  constexpr int Compare(TAddrDir lhs, TAddrDir rhs) {
    return ToInt(lhs) - ToInt(rhs);
  }

  std::ostream &Write(std::ostream &strm, TAddrDir dir);
  std::ostream &WriteCppType(std::ostream &strm, TAddrDir dir);

  enum class TMutator { Add, And, Assign, Div, Exp, Intersection, Mod, Mult, Or, SymmetricDiff, Sub, Union, Xor };

  std::ostream &operator<<(std::ostream &strm, TAddrDir dir);

}  // Stig

namespace std {

  template <>
  struct hash<Stig::TAddrDir> {

    typedef size_t result_type;
    typedef Stig::TAddrDir argument_type;

    result_type operator()(const argument_type &that) {
      assert(&that);
      return Stig::ToInt(that);
    }

  };  // hash<Stig::TAddrDir>

}  // std