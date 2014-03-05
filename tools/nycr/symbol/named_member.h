/* <tools/nycr/symbol/named_member.h>

   A compound member with a kind and with a specific name.

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
#include <tools/nycr/symbol/member_with_kind.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TNamedMember
          : public TMemberWithKind {
        NO_COPY_SEMANTICS(TNamedMember);
        public:

        /* TODO */
        TNamedMember(TCompound *compound, TKind *kind, const TName &name)
            : TMemberWithKind(kind), Name(name) {
          assert(compound);
          SetCompound(compound);
        }

        /* TODO */
        virtual ~TNamedMember();

        /* TODO */
        virtual const TName &GetName() const;

        /* TODO */
        virtual void WriteRhs(std::ostream &strm) const;

        /* TODO */
        virtual void WriteXml(std::ostream &strm) const;

        private:

        /* TODO */
        TName Name;

      };  // TNamedMember

    }  // Symbol

  }  // Nycr

}  // Tools
