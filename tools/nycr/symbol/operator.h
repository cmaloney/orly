/* <tools/nycr/symbol/operator.h>

   A leaf token with operator-like behavior.

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
#include <tools/nycr/symbol/atom.h>
#include <tools/nycr/symbol/prec_level.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TPrecLevel;

      /* TODO */
      class TOperator
          : public TAtom {
        public:

        /* TODO */
        enum TAssoc { Left, NonAssoc, Right };

        /* TODO */
        TOperator(
            const TName &name, TAnyBase *base, const std::string &pattern, const Base::TOpt<int> &pri, TPrecLevel *prec_level, TAssoc assoc)
            : TAtom(name, base, pattern, pri), PrecLevel(prec_level), Assoc(assoc) {
          assert(prec_level);
        }

        /* TODO */
        virtual void Accept(const TVisitor &visitor) const;

        /* TODO */
        TAssoc GetAssoc() const {
          assert(this);
          return Assoc;
        }

        /* TODO */
        size_t GetPrecLevelIdx() const {
          assert(this);
          return PrecLevel->GetIdx();
        }

        /* TODO */
        TPrecLevel *GetPrecLevel() const {
          assert(this);
          return PrecLevel;
        }

        private:

        /* TODO */
        TPrecLevel *PrecLevel;

        /* TODO */
        TAssoc Assoc;

      };  // TOperator

    }  // Symbol

  }  // Nycr

}  // Tools

