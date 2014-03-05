/* <stig/package/name.h>

   TODO

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
#include <string>

#include <base/piece.h>
#include <jhm/naming.h>

namespace Stig {

  namespace Package {

    typedef Jhm::TNamespace TName;

    struct TVersionedName {
      static TVersionedName Parse(const Base::TPiece<const char> &name);

      bool operator==(const TVersionedName &that) const;
      Jhm::TRelPath GetSoRelPath() const;

      TName Name;
      uint64_t Version;
    }; // TVersionedName;

    std::ostream &operator<<(std::ostream &out, const TVersionedName &that);

  } // Package

} // Stig

namespace std {

  template <>
  struct hash<Stig::Package::TVersionedName> {

    typedef size_t result_type;
    typedef Stig::Package::TVersionedName argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      return that.Name.GetHash() ^ that.Version;
    }

  };  // hash<Stig::Package::TVersionedName>

}  // std
