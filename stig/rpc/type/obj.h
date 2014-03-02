/* <stig/rpc/type/obj.h> 

   A compound type with fields distinguished by name.

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
#include <string>
#include <utility>

#include <stig/rpc/type.h>

namespace Stig {

  namespace Rpc {

    namespace Type {

      /* A compound type with fields distinguished by ordinal. */
      class TObj final
          : public TType {
        public:

        /* The type we use to contain our fields. */
        using TFields = std::map<std::string, const TType *>;

        /* Start out with no fields. */
        TObj() {}

        /* Destroys our fields, too. */
        virtual ~TObj();

        /* Accept the visitor. */
        virtual void Accept(const TVisitor &visitor) const override;

        /* Add a field to the compound. */
        void AddField(std::string &&name, TPtr &&type);

        /* The fields of the compound. */
        const TFields &GetFields() const {
          assert(this);
          return Fields;
        }

        /* Return the type of the field with the given name, or null if we don't have a field by that name. */
        const TType *TryGetField(const std::string &name) const;

        private:

        /* See accessor. */
        TFields Fields;

      };  // TObj

    }  // Type

  }  // Rpc

}  // Stig

