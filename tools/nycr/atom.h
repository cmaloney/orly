/* <tools/nycr/atom.h>

   A declaration of a atom of token.

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

#include <base/no_copy_semantics.h>
#include <tools/nycr/final.h>
#include <tools/nycr/symbol/atom.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TAtom
        : public TFinal {
      NO_COPY_SEMANTICS(TAtom);
      protected:

      /* TODO */
      TAtom(const Syntax::TName *name, const Syntax::TOptSuper *opt_super, const Syntax::TPattern *pattern);

      /* TODO */
      const std::string &GetPatternText() const {
        assert(this);
        return PatternText;
      }

      /* TODO */
      int GetPri() const {
        assert(this);
        return Pri ? *Pri : 0;
      }

      /* TODO */
      virtual Symbol::TAtom *GetSymbolAsAtom() const = 0;

      /* TODO */
      virtual Symbol::TKind *GetSymbolAsKind() const;

      private:

      /* TODO */
      std::string PatternText;

      /* TODO */
      Base::TOpt<int> Pri;

    };  // TAtom

    /* TODO */
    template <>
    struct TDecl::TInfo<TAtom> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
