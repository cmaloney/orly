/* <orly/type/addr.h>

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

#include <orly/shared_enum.h>
#include <orly/type/managed_type.h>

namespace Orly {

  namespace Type {

    typedef std::vector<std::pair<TAddrDir, TType>> TAddrElems;

    class TAddr : public TInternedType<TAddr, TAddrElems> {
      NO_COPY(TAddr);
      public:

      typedef TAddrDir TDir;
      typedef TAddrElems TElems;

      static TType Get(const TElems &elems) {
        assert(&elems);
        return TInternedType::Get(elems);
      }
      const TElems &GetElems() const {
        assert(this);
        return std::get<0>(GetKey());
      }

      private:

      TAddr(const TElems &elems) : TInternedType(elems) {}
      virtual ~TAddr();

      virtual void Write(std::ostream &) const;

      friend class TInternedType;
    };  // TAddr

  }  // Type

}  // Orly

namespace std {

  template<>
  struct hash<Orly::Type::TAddrElems> {

    size_t operator()(const Orly::Type::TAddrElems &that) const {
      assert(&that);
      size_t result = 0;
      for (const auto &elem: that) {
        //TODO: Find a better hash
        result ^= Base::RotatedLeft(elem.second.GetHash(), 5 + (elem.first == Orly::TAddrDir::Asc ? 1 : 0));
      }
      return result;
    }

    typedef size_t result_type;
    typedef Orly::Type::TAddrElems argument_type;
  }; // hash<Orly::Type::TAddrElems>

} // std