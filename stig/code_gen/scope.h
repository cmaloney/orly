/* <stig/code_gen/scope.h>

   TODO

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
#include <stig/code_gen/id.h>
#include <stig/code_gen/inline.h>
#include <stig/shared_enum.h>

namespace Stig {

  namespace Symbol {

    /* Forward Declaration */
    class TFunction;
  }

  namespace CodeGen {

    /* Forward Declaration */
    class TFunction;
    class TInterner;
    class TStmtBlock;

    class TIdScope {

      public:
      typedef std::shared_ptr<TIdScope> TPtr;

      /* Build a new IdScope from scratch. */
      static TPtr New();

      TId<TIdKind::Arg> NewArg();
      TId<TIdKind::Func> NewFunc();
      TId<TIdKind::Var> NewVar();

      private:
      TIdScope() {}
      TId<TIdKind::Arg>::TGen Arg;
      TId<TIdKind::Func>::TGen Func;
      TId<TIdKind::Var>::TGen Var;
    };

    class TCodeScope {
      NO_COPY_SEMANTICS(TCodeScope);
      public:

      typedef std::vector<std::pair<std::string, TInline::TPtr>> TAssertions;
      typedef std::vector<TInline::TPtr> TLocals;

      /* The id generator is created by top level functions. Lambda functions inside the top level functions don't have
         their own id generators as */
      TCodeScope(const TIdScope::TPtr &id_scope);

      ~TCodeScope();

      /* Add an assertion to the scope */
      void AddAssertion(const std::string &name, const TInline::TPtr &assertion);

      /* Makes a new id, and adds the given inline to our list of locals. */
      void AddLocal(const TInline::TPtr &inline_);

      TIdScope::TPtr GetIdScope() const;
      TInterner *GetInterner() const;
      const TLocals &GetLocals() const;

      TStmtBlock &GetStmts();
      const TStmtBlock &GetStmts() const;

      TId<TIdKind::Arg> NewArg();

      void WriteStart(TCppPrinter &out) const;

      private:

      /* All the assertions in the area conveniently gathered. */
      TAssertions Assertions;

      TIdScope::TPtr IdScope;

      TInterner *Interner;

      /* Common subexpressions that will need definitions. */
      TLocals Locals;

      /* Mutations from effectings contained within */
      std::unique_ptr<TStmtBlock> Stmts;
    }; // TCodeScope

  } // CodeGen

} // Stig