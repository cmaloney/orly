/* <stig/rpc/var/set.h> 

   A variant set.

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
#include <set>

#include <stig/rpc/var.h>
#include <stig/rpc/type.h>
#include <stig/rpc/var/cmp.h>

namespace Stig {

  namespace Rpc {

    namespace Var {

      /* A variant set. */
      class TSet final
          : public TVar {
        public:

        /* The type of container we use to hold our elements. */
        using TElems = std::set<const TVar *, TCmp>;

        /* Take ownership of our element type. */
        TSet(TType::TPtr &&elem_type)
            : ElemType(std::move(elem_type)) {}

        /* Destroys our elements, too. */
        virtual ~TSet();

        /* Accept the visitor. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* Take ownership of the element.
           If the value is already in the set, it is discarded. */
        void AddElem(TPtr &&elem);

        /* The type of element we contain. */
        const TType *GetElemType() const {
          assert(this);
          return ElemType.get();
        }

        /* The elements of this container. */
        const TElems &GetElems() const {
          assert(this);
          return Elems;
        }

        private:

        /* See accessor. */
        TType::TPtr ElemType;

        /* See accessor. */
        TElems Elems;

      };  // TSet

    }  // Var

  }  // Rpc

}  // Stig

