/* <tools/nycr/symbol/kind.h>

   A kind of token.

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
#include <unordered_set>

#include <base/no_copy_semantics.h>
#include <tools/nycr/symbol/name.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TBase;
      class TKeyword;
      class TLanguage;
      class TOperator;
      class TRule;

      /* TODO */
      class TKind {
        NO_COPY_SEMANTICS(TKind);
        public:

        /* TODO */
        class TAnyBase {
          NO_COPY_SEMANTICS(TAnyBase);
          public:

          /* TODO */
          typedef std::unordered_set<TKind *> TSubKinds;

          /* TODO */
          const TSubKinds &GetSubKinds() const {
            assert(this);
            return SubKinds;
          }

          protected:

          /* TODO */
          TAnyBase() {}

          /* TODO */
          virtual ~TAnyBase();

          /* TODO */
          virtual TBase *GetBase() = 0;

          /* TODO */
          virtual bool HasBase(const TBase *target) = 0;

          /* TODO */
          TSubKinds SubKinds;

          /* TODO */
          friend class TKind;

        };  // TBase

        /* TODO */
        class TVisitor {
          public:

          /* TODO */
          virtual ~TVisitor();

          /* TODO */
          virtual void operator()(const TBase *that) const = 0;

          /* TODO */
          virtual void operator()(const TKeyword *that) const = 0;

          /* TODO */
          virtual void operator()(const TLanguage *that) const = 0;

          /* TODO */
          virtual void operator()(const TOperator *that) const = 0;

          /* TODO */
          virtual void operator()(const TRule *that) const = 0;

          protected:

          /* TODO */
          TVisitor() {}

        };  // TVisitor

        /* TODO */
        virtual ~TKind();

        /* TODO */
        virtual void Accept(const TVisitor &visitor) const = 0;

        /* TODO */
        TBase *GetBase() const {
          assert(this);
          return Base ? Base->GetBase() : 0;
        }

        /* TODO */
        const TName &GetName() const {
          assert(this);
          return Name;
        }

        /* TODO */
        bool HasBase(const TBase *target) const {
          assert(this);
          return Base ? Base->HasBase(target) : false;
        }

        protected:

        /* TODO */
        TKind(const TName &name, TAnyBase *base);

        private:

        /* TODO */
        TName Name;

        /* TODO */
        TAnyBase *Base;

      };  // TKind

    }  // Symbol

  }  // Nycr

}  // Tools