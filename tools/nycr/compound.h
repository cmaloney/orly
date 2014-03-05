/* <tools/nycr/compound.h>

   A declaration of a compound of token.

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

#include <vector>

#include <base/no_copy_semantics.h>
#include <tools/nycr/final.h>
#include <tools/nycr/operator.h>
#include <tools/nycr/symbol/compound.h>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TOperator;

    /* TODO */
    class TCompound
        : public TFinal {
      NO_COPY_SEMANTICS(TCompound);
      protected:

      /* TODO */
      TCompound(const Syntax::TName *name, const Syntax::TOptSuper *opt_super, const Syntax::TOptRhs *opt_rhs);

      /* TODO */
      virtual ~TCompound();

      /* TODO */
      void BuildMembers();

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      virtual Symbol::TCompound *GetSymbolAsCompound() const = 0;

      /* TODO */
      virtual Symbol::TKind *GetSymbolAsKind() const;

      /* TODO */
      Symbol::TOperator *TryGetOperator() const {
        assert(this);
        return Operator ? Operator->GetSymbolAsOperator() : 0;
      }

      private:

      /* TODO */
      class TMember {
        NO_COPY_SEMANTICS(TMember);
        public:

        /* TODO */
        virtual ~TMember() {}

        /* TODO */
        virtual void Build(Symbol::TCompound *compound) = 0;

        /* TODO */
        virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb) = 0;

        protected:

        /* TODO */
        TMember() {}

      };  // TMember

      /* TODO */
      class TErrorMember
          : public TMember {
        NO_COPY_SEMANTICS(TErrorMember);
        public:

        /* TODO */
        TErrorMember() {}

        /* TODO */
        virtual void Build(Symbol::TCompound *compound);

        /* TODO */
        virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      };  // TErrorMember

      /* TODO */
      class TMemberWithKind
          : public TMember {
        NO_COPY_SEMANTICS(TMemberWithKind);
        public:

        /* TODO */
        TMemberWithKind(const Syntax::TName *kind) : Kind(kind) {}

        /* TODO */
        virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

        /* TODO */
        Symbol::TKind *GetKind() const {
          assert(this);
          assert(Kind);
          return Kind->GetSymbolAsKind();
        }

        private:

        /* TODO */
        TRef<TKind> Kind;

      };  // TMemberWithKind

      /* TODO */
      class TAnonymousMember : public TMemberWithKind {
        NO_COPY_SEMANTICS(TAnonymousMember);
        public:

        /* TODO */
        TAnonymousMember(const Syntax::TAnonymousMember *member)
            : TMemberWithKind(member->GetName()) {}

        /* TODO */
        virtual void Build(Symbol::TCompound *compound);

      };  // TAnonymousMember

      /* TODO */
      class TNamedMember
          : public TMemberWithKind {
        NO_COPY_SEMANTICS(TNamedMember);
        public:

        /* TODO */
        TNamedMember(const Syntax::TNamedMember *member)
            : TMemberWithKind(member->GetKind()), Name(member->GetName()) {}

        /* TODO */
        virtual void Build(Symbol::TCompound *compound);

        private:

        /* TODO */
        const Syntax::TName *Name;

      };  // TNamedMember

      /* TODO */
      std::vector<TMember *> Members;

      /* TODO */
      TRef<TOperator> Operator;

    };  // TCompound

    /* TODO */
    template <>
    struct TDecl::TInfo<TCompound> {
      static const char *Name;
    };

  }  // Nycr

}  // Tools
