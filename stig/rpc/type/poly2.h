/* <stig/rpc/type/poly2.h> 

   Arity-2 polymorphic types.

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

      /* The template for all arity-2 polymorphic types. */
      template <TKind Kind>
      class TPoly2 final
          : public TType {
        public:

        /* Take ownership of the type of our keys and the type of our values. */
        TPoly2(TPtr &&key, TPtr &&val)
            : Key(std::move(key)), Val(std::move(val)) {
          assert(Key);
          assert(Val);
        }

        /* Accept the visitor, defined below. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* The type of our keys.  Never null. */
        const TType *GetKey() const {
          assert(this);
          return Key.get();
        }

        /* The type of our values.  Never null. */
        const TType *GetVal() const {
          assert(this);
          return Val.get();
        }

        private:

        /* See accessors. */
        TPtr Key, Val;

      };  // TPoly2<TKind>

      /* Specializations. */
      using TDict = TPoly2<TKind::Dict>;

    }  // Type

  }  // Rpc

}  // Stig

