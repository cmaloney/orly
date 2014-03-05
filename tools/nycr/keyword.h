/* <tools/nycr/keyword.h>

   A declaration of a keyword of token.

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
#include <tools/nycr/atom.h>
#include <tools/nycr/symbol/keyword.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TKeyword
        : public TAtom {
      NO_COPY_SEMANTICS(TKeyword);
      public:

      /* TODO */
      TKeyword(const Syntax::TKeyword *decl);

      /* TODO */
      virtual Symbol::TAtom *GetSymbolAsAtom() const;

      /* TODO */
      Symbol::TKeyword *GetSymbolAsKeyword() const {
        assert(this);
        assert(Symbol);
        return Symbol;
      }

      private:

      /* TODO */
      virtual bool Build(int pass);

      /* TODO */
      Symbol::TKeyword *Symbol;

    };  // TKeyword

    /* TODO */
    template <>
    struct TDecl::TInfo<TKeyword> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
