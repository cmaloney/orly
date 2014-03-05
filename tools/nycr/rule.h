/* <tools/nycr/rule.h>

   A declaration of a rule of token.

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

#include <base/no_copy_semantics.h>
#include <tools/nycr/compound.h>
#include <tools/nycr/symbol/rule.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TBase;

    /* TODO */
    class TRule
        : public TCompound {
      NO_COPY_SEMANTICS(TRule);
      public:

      /* TODO */
      TRule(const Syntax::TRule *decl);

      /* TODO */
      virtual Symbol::TCompound *GetSymbolAsCompound() const;

      /* TODO */
      Symbol::TRule *GetSymbolAsRule() const {
        assert(this);
        assert(Symbol);
        return Symbol;
      }

      private:

      /* TODO */
      virtual bool Build(int pass);

      /* TODO */
      Symbol::TRule *Symbol;

    };  // TRule

    /* TODO */
    template <>
    struct TDecl::TInfo<TRule> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
