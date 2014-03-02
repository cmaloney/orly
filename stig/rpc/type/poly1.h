/* <stig/rpc/type/poly1.h> 

   Arity-1 polymorphic types.

   Copyright 2010-2014 Tagged
   
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
#include <utility>

#include <stig/rpc/type.h>

namespace Stig {

  namespace Rpc {

    namespace Type {

      /* The template for all arity-1 polymorphic types. */
      template <TKind Kind>
      class TPoly1 final
          : public TType {
        public:

        /* Take ownership of the type of our elements. */
        TPoly1(TPtr &&elem)
            : Elem(std::move(elem)) {
          assert(Elem);
        }

        /* Accept the visitor. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* The type of our elements.  Never null. */
        const TType *GetElem() const {
          assert(this);
          return Elem.get();
        }

        private:

        /* See accessor. */
        TPtr Elem;

      };  // TPoly1<TKind>

      /* Specializations. */
      using TList = TPoly1<TKind::List>;
      using TOpt  = TPoly1<TKind::Opt >;
      using TSet  = TPoly1<TKind::Set >;

    }  // Type

  }  // Rpc

}  // Stig

