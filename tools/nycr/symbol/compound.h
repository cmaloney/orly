/* <tools/nycr/symbol/compound.h>

   A production-based token.

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
#include <map>
#include <ostream>
#include <vector>

#include <base/no_copy_semantics.h>
#include <tools/nycr/symbol/final.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TOperator;

      /* TODO */
      class TCompound
          : public TFinal {
        NO_COPY_SEMANTICS(TCompound);
        public:

        /* TODO */
        class TMember {
          public:

          /* TODO */
          virtual ~TMember();

          /* TODO */
          TCompound *GetCompound() const {
            assert(this);
            return Compound;
          }

          /* TODO */
          virtual const TName &GetName() const = 0;

          /* TODO */
          virtual const TKind *TryGetKind() const = 0;

          /* TODO */
          virtual void WriteRhs(std::ostream &strm) const = 0;

          /* TODO */
          virtual void WriteXml(std::ostream &strm) const = 0;

          protected:

          /* TODO */
          TMember()
              : Compound(0) {}

          /* TODO */
          void SetCompound(TCompound *compound);

          private:

          /* TODO */
          TCompound *Compound;

        };  // TMember

        /* TODO */
        typedef std::map<TName, TMember *> TMembersByName;

        /* TODO */
        typedef std::vector<TMember *> TMembersInOrder;

        /* TODO */
        const TMembersByName &GetMembersByName() const {
          assert(this);
          return MembersByName;
        }

        /* TODO */
        const TMembersInOrder &GetMembersInOrder() const {
          assert(this);
          return MembersInOrder;
        }

        /* TODO */
        void SetOperator(TOperator *oper) {
          assert(this);
          Operator = oper;
        }

        /* TODO */
        TOperator *TryGetOperator() const {
          assert(this);
          return Operator;
        }

        protected:

        /* TODO */
        TCompound(const TName &name, TAnyBase *base)
            : TFinal(name, base), Operator(0) {}

        private:

        /* TODO */
        void OnJoin(TMember *member);

        /* TODO */
        void OnPart(TMember *member);

        /* TODO */
        TMembersByName MembersByName;

        /* TODO */
        TMembersInOrder MembersInOrder;

        /* TODO */
        TOperator *Operator;

      };  // TCompound

    }  // Symbol

  }  // Nycr

}  // Tools
