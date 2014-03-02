/* <stig/rpc/var/dict.h> 

   A variant dict.

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
#include <map>

#include <stig/rpc/var.h>
#include <stig/rpc/type.h>
#include <stig/rpc/var/cmp.h>

namespace Stig {

  namespace Rpc {

    namespace Var {

      /* A variant dict. */
      class TDict final
          : public TVar {
        public:

        /* The type of container we use to hold our elements. */
        using TElems = std::map<const TVar *, const TVar *, TCmp>;

        /* Take ownership of our key and value types. */
        TDict(TType::TPtr &&key_type, TType::TPtr &&val_type)
            : KeyType(std::move(key_type)), ValType(std::move(val_type)) {}

        /* Destroys our elements, too. */
        virtual ~TDict();

        /* Accept the visitor. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* Take ownership of the key-value pair.
           If they key is already in the dict, its previous value is discarded. */
        void AddElem(TPtr &&key, TPtr &&val);

        /* The type of key we contain. */
        const TType *GetKeyType() const {
          assert(this);
          return KeyType.get();
        }

        /* The elements of this container. */
        const TElems &GetElems() const {
          assert(this);
          return Elems;
        }

        /* The type of value we contain. */
        const TType *GetValType() const {
          assert(this);
          return ValType.get();
        }

        private:

        /* See accessors. */
        TType::TPtr KeyType, ValType;

        /* See accessor. */
        TElems Elems;

      };  // TDict

    }  // Var

  }  // Rpc

}  // Stig

