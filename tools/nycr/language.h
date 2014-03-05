/* <tools/nycr/language.h>

   A declaration of a language of token.

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

#include <string>
#include <vector>

#include <base/no_copy_semantics.h>
#include <tools/nycr/compound.h>
#include <tools/nycr/symbol/language.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TBase;

    /* TODO */
    class TLanguage
        : public TCompound {
      NO_COPY_SEMANTICS(TLanguage);
      public:

      /* TODO */
      TLanguage(const Syntax::TLanguage *decl);

      /* TODO */
      virtual Symbol::TCompound *GetSymbolAsCompound() const;

      /* TODO */
      Symbol::TLanguage *GetSymbolAsLanguage() const {
        assert(this);
        assert(Symbol);
        return Symbol;
      }

      private:

      /* TODO */
      virtual bool Build(int pass);

      /* TODO */
      std::vector<Symbol::TName> Namespaces;

      /* TODO */
      Base::TOpt<int> Sr, Rr;

      /* TODO */
      Symbol::TLanguage *Symbol;

    };  // TLanguage

    /* TODO */
    template <>
    struct TDecl::TInfo<TLanguage> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
