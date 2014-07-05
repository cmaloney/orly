/* <orly/package/name.h>

   TODO

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

#include <cassert>
#include <ostream>
#include <string>

#include <base/hash.h>
#include <base/piece.h>
#include <jhm/naming.h>

namespace Orly {

  namespace Package {

    struct TName {
      static TName Parse(const std::string &name);

      bool operator==(const TName &that) const {
        return Name == that.Name;
      }
      bool operator!=(const TName &that) const {
        return Name != that.Name;
      }

      std::vector<std::string> Name;
    };

    struct TVersionedName {
      static TVersionedName Parse(const Base::TPiece<const char> &name);

      bool operator==(const TVersionedName &that) const;
      Jhm::TRelPath GetSoRelPath() const;

      TName Name;
      uint64_t Version;
    }; // TVersionedName;

    std::ostream &operator<<(std::ostream &out, const TVersionedName &that);

    //NOTE: I'd make this a operator<< but ADL looks throuhg the using.
    std::ostream &operator<<(std::ostream &out, const TName &that);

  } // Package

} // Orly

namespace std {

  template <>
  struct hash<Orly::Package::TName> {

    size_t operator()(const Orly::Package::TName &that) const {
      assert(&that);
      return Base::ChainHashes(that.Name);
    }

  };  // hash<Orly::Package::TVersionedName>

  template <>
  struct hash<Orly::Package::TVersionedName> {

    typedef size_t result_type;
    typedef Orly::Package::TVersionedName argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      return Base::ChainHashes(that.Name, that.Version);
    }

  };  // hash<Orly::Package::TVersionedName>

}  // std
