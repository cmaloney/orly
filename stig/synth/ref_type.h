/* <stig/synth/ref_type.h>

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

#include <stig/stig.package.cst.h>
#include <stig/synth/type.h>
#include <stig/synth/type_def.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TRefType
        : public TType {
      NO_COPY_SEMANTICS(TRefType);
      public:

      /* TODO */
      TRefType(const Package::Syntax::TRefType *ref_type);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      /* TODO */
      virtual Type::TType ComputeSymbolicType() const;

      /* TODO */
      TDef::TRef<TTypeDef> TypeDef;

    };  // TType

  }  // Synth

}  // Stig
