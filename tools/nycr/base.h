/* <tools/nycr/base.h>

   A declaration of an abstract token.

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

#include <base/no_copy_semantics.h>
#include <tools/nycr/kind.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TBase;

    /* TODO */
    class TBase
        : public TKind {
      NO_COPY_SEMANTICS(TBase);
      public:

      /* TODO */
      TBase(const Syntax::TBase *decl);

      /* TODO */
      Symbol::TBase *GetSymbolAsBase() const {
        assert(this);
        assert(Symbol);
        return Symbol;
      }

      /* TODO */
      virtual Symbol::TKind *GetSymbolAsKind() const;

      private:

      /* TODO */
      virtual bool Build(int pass);

      /* TODO */
      Symbol::TBase *Symbol;

    };  // TKind

    /* TODO */
    template <>
    struct TDecl::TInfo<TBase> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
