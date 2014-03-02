/* <stig/rpc/var/mono.h> 

   Variants of monomorphic types.

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

#include <stig/rpc/var.h>

namespace Stig {

  namespace Rpc {

    namespace Var {

      /* The template for all variants of monomorphic types. */
      template <TKind Kind, typename TVal>
      class TMono final
          : public TVar {
        public:

        /* Allow the value to default-construct. */
        TMono() {}

        /* Take ownership of the value. */
        TMono(TVal &&val)
            : Val(std::move(val)) {}

        /* Accept the visitor. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* Our value, as a C++ native type. */
        const TVal &GetVal() const {
          assert(this);
          return Val;
        }

        private:

        /* See accessor. */
        TVal Val;

      };  // TMono<TKind>

      /* Specializations. */
      using TBool     = TMono<TKind::Bool,     Rpc::TBool    >;
      using TId       = TMono<TKind::Id,       Rpc::TId      >;
      using TInt      = TMono<TKind::Int,      Rpc::TInt     >;
      using TReal     = TMono<TKind::Real,     Rpc::TReal    >;
      using TStr      = TMono<TKind::Str,      Rpc::TStr     >;
      using TTimeDiff = TMono<TKind::TimeDiff, Rpc::TTimeDiff>;
      using TTimePnt  = TMono<TKind::TimePnt,  Rpc::TTimePnt >;

    }  // Var

  }  // Rpc

}  // Stig

