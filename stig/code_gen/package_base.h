/* <stig/code_gen/package_base.h>

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

#include <unordered_map>

#include <base/hash.h>
#include <base/uuid.h>

#include <stig/symbol/package.h>
#include <stig/type.h>

namespace Stig {

  namespace CodeGen {

    /* Dependency injection for package to expose enough context to build functions. */
    namespace L0 {

      /* TODO */
      class TPackage {
        NO_COPY_SEMANTICS(TPackage);
        public:

        /* TODO */
        typedef std::unordered_map<Base::TUuid, std::pair<Stig::Type::TType, Stig::Type::TType>> TAddrMap;
        typedef std::unordered_map<std::pair<Stig::Type::TType, Stig::Type::TType>, Base::TUuid> TRevAddrMap;

        /* TODO */
        virtual ~TPackage() {}

        /* TODO */
        inline const Base::TUuid &GetIndexIdFor(const Type::TType &addr, const Type::TType &val) const {
          assert(this);
          auto pos = ReverseAddrMap.find(std::make_pair(addr, val));
          assert(pos!= ReverseAddrMap.end());
          return pos->second;
        }

        /* TODO */
        inline const TAddrMap &GetAddrMap() const {
          assert(this);
          return AddrMap;
        }

        /* TODO */
        inline const TRevAddrMap &GetReverseAddrMap() const {
          assert(this);
          return ReverseAddrMap;
        }

        /* TODO */
        inline const Jhm::TNamespace &GetNamespace() const {
          assert(this);
          assert(Symbol);
          return Symbol->GetNamespace();
        }

        protected:

        /* TODO */
        TPackage(const Symbol::TPackage::TPtr &package) : Symbol(package) {}

        /* TODO */
        Symbol::TPackage::TPtr Symbol;

        /* TODO */
        TAddrMap AddrMap;

        /* TODO */
        TRevAddrMap ReverseAddrMap;

      };  // TPackage

    }  // L0

  }  // CodeGen

}  // Stig