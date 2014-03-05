/* <stig/synth/type_def.h>

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

#include <cassert>

#include <stig/stig.package.cst.h>
#include <stig/type.h>
#include <stig/synth/scope_and_def.h>
#include <stig/synth/type.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TTypeDef
        : public TDef {
      public:

      /* TODO */
      TTypeDef(TScope *scope, const Package::Syntax::TTypeDef *type_def);

      /* TODO */
      virtual ~TTypeDef();

      /* TODO */
      const Type::TType &GetSymbolicType() const;

      private:

      /* TODO */
      virtual TAction Build(int pass);

      /* TODO */
      virtual void ForEachPred(int pass, const std::function<bool (TDef *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      TType *Type;

    };  // TTypeDef

    /* TODO */
    template <>
    struct TDef::TInfo<TTypeDef> {
      static const char *Name;
    };

  }  // Synth

}  // Stig
