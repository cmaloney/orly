/* <stig/symbol/given_param_def.h>

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

#include <memory>

#include <base/no_copy_semantics.h>
#include <stig/pos_range.h>
#include <stig/symbol/param_def.h>

namespace Stig {

  namespace Symbol {

    class TGivenParamDef
        : public TParamDef,
          public std::enable_shared_from_this<TGivenParamDef> {
      NO_COPY_SEMANTICS(TGivenParamDef);
      public:

      typedef std::shared_ptr<TGivenParamDef> TPtr;

      static TPtr New(
            const TFunction::TPtr &function,
            const std::string &name,
            const Type::TType &type,
            const TPosRange &pos_range);

      ~TGivenParamDef();

      void Accept(const TVisitor &visitor) const;

      private:

      TGivenParamDef(
          const TFunction::TPtr &function,
          const std::string &name,
          const Type::TType &type,
          const TPosRange &pos_range);

    };  // TGivenParamDef

  }  // Symbol

}  // Stig
