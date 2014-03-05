/* <tools/nycr/symbol/base.h>

   A base token.

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

#include <tools/nycr/symbol/kind.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TBase
          : public TKind, public TKind::TAnyBase {
        NO_COPY_SEMANTICS(TBase);
        public:

        /* TODO */
        TBase(const TName &name, TKind::TAnyBase *base = 0)
            : TKind(name, base) {}

        /* TODO */
        virtual void Accept(const TVisitor &visitor) const;

        private:

        /* TODO */
        virtual TBase *GetBase();

        /* TODO */
        virtual bool HasBase(const TBase *target);

      };  // TBase

    }  // Symbol

  }  // Nycr

}  // Tools
