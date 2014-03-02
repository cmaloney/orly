/* <stig/rpc/type/addr.h> 

   A compound type with fields distinguished by ordinal.

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
#include <vector>

#include <stig/rpc/type.h>

namespace Stig {

  namespace Rpc {

    namespace Type {

      /* A compound type with fields distinguished by ordinal. */
      class TAddr final
          : public TType {
        public:

        /* A field is a sorting direction and a type. */
        using TField = std::pair<TDir, const TType *>;

        /* The type we use to contain our fields. */
        using TFields = std::vector<TField>;

        /* Start out with no fields. */
        TAddr() {}

        /* Destroys our fields, too. */
        virtual ~TAddr();

        /* Accept the visitor. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* Add a field to the compound. */
        void AddField(TDir dir, TPtr &&type);

        /* The fields of the compound. */
        const TFields &GetFields() const {
          assert(this);
          return Fields;
        }

        private:

        /* See accessor. */
        TFields Fields;

      };  // TAddr

    }  // Type

  }  // Rpc

}  // Stig

