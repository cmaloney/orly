/* <tools/nycr/final.h>

   A declaration of a final of token.

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
#include <tools/nycr/kind.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TBase;

    /* TODO */
    class TFinal
        : public TKind {
      NO_COPY_SEMANTICS(TFinal);
      protected:

      /* TODO */
      TFinal(const Syntax::TName *name, const Syntax::TOptSuper *opt_super);

    };  // TFinal

    /* TODO */
    template <>
    struct TDecl::TInfo<TFinal> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
