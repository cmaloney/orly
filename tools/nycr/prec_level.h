/* <tools/nycr/prec_level.h>

   A declaration of a precedence level.

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
#include <tools/nycr/decl.h>
#include <tools/nycr/symbol/prec_level.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TPrecLevel
        : public TDecl {
      NO_COPY_SEMANTICS(TPrecLevel);
      public:

      /* TODO */
      TPrecLevel(const Syntax::TPrecLevel *decl);

      /* TODO */
      Symbol::TPrecLevel *GetSymbol() const {
        assert(this);
        assert(Symbol);
        return Symbol;
      }

      private:

      /* TODO */
      virtual bool Build(int pass);

      size_t Idx;

      static size_t NextIdx;

      /* TODO */
      Symbol::TPrecLevel *Symbol;

    };  // TPrecLevel

    /* TODO */
    template <>
    struct TDecl::TInfo<TPrecLevel> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
