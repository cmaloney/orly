/* <stig/rpc/var/opt.h> 

   A variant optional.

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

#include <stig/rpc/var.h>
#include <stig/rpc/type.h>

namespace Stig {

  namespace Rpc {

    namespace Var {

      /* A variant optional. */
      class TOpt final
          : public TVar {
        public:

        /* Take ownership of our element type. */
        TOpt(TType::TPtr &&elem_type)
            : ElemType(std::move(elem_type)), Elem(nullptr) {}

        /* Destroys our element, too, if any. */
        virtual ~TOpt();

        /* Accept the visitor. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* Take ownership of the element.
           If we already had an element, it is replaced and destroyed. */
        void AddElem(TPtr &&elem);

        /* The type of element we contain. */
        const TType *GetElemType() const {
          assert(this);
          return ElemType.get();
        }

        /* Our element, if we have one; otherwise, null. */
        const TVar *TryGetElem() const {
          return Elem;
        }

        private:

        /* See accessor. */
        TType::TPtr ElemType;

        /* See accessor. */
        TVar *Elem;

      };  // TOpt

    }  // Var

  }  // Rpc

}  // Stig

