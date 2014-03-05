/* <stig/code_gen/context.h>

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

#include <stack>
#include <unordered_map>

#include <base/no_construction.h>
#include <stig/code_gen/inline.h>

namespace Stig {

  namespace Expr {
    class TExpr;
  }

  namespace CodeGen {

    class TCodeScope;
    class TFunction;
    class TInterner;
    class TScopeCtx;
    class TStmtBlock;

    //TODO: Make hold the active CodeScope as well as core sequences for implicit maps.
    /* NOTE: We call this Context even though it is a class because it is more or less a namespace. All it has are
             statics.*/
    class Context {
      NO_CONSTRUCTION(Context);
      public:

      typedef std::shared_ptr<const Expr::TExpr> TExprPtr;
      typedef std::unordered_map<TExprPtr, TInline::TPtr> TAliases;

      static TInterner &GetInterner();
      static TInline::TPtr GetLhs();
      static TInline::TPtr GetOptOverride(const TExprPtr &expr);
      static TFunction *GetNearestFunc();
      static TInline::TPtr GetRhs();
      static TCodeScope*   GetScope();
      static TInline::TPtr GetStart();
      static TStmtBlock*    GetStmtBlock();
      static TInline::TPtr GetThat();

      private:
      static std::stack<TInline::TPtr> Lhs, Rhs, Start, That;
      static std::stack<TFunction*> NearestFunc;
      static std::stack<TCodeScope*> Scope;
      static std::stack<TStmtBlock*> StmtBlock;

      /* A map of exprs which we have overriden to a specific inline. This happens primarily when we are doing an
         implicit map and we need to change the sequence printing to be an argument reference.*/
      static TAliases Overrides;

      //TODO: Implicit map args... There are a set of those...
      friend class TFilterCtx;
      friend class TFunctionCtx;
      friend class TMapCtx;
      friend class TReduceCtx;
      friend class TScopeCtx;
      friend class TSortCtx;
      friend class TStmtCtx;
      friend class TThatableCtx;
      friend class TWhileCtx;
    }; // TContext

    /* Contexts aggregated by other contexts*/
    class TScopeCtx {
      NO_COPY_SEMANTICS(TScopeCtx);
      public:

      TScopeCtx(TCodeScope *scope);
      ~TScopeCtx();

    }; // TScopeCtx

    class TFunctionCtx {
      NO_COPY_SEMANTICS(TFunctionCtx);
      public:

      TFunctionCtx(TFunction* func);
      ~TFunctionCtx();

      private:
      TScopeCtx Scope;
    }; // TFunctionCtx

    /* General independent contexts */
    class TThatableCtx {
      NO_COPY_SEMANTICS(TThatableCtx);
      public:

      TThatableCtx(const TInline::TPtr &that);
      ~TThatableCtx();
    }; //TAssertCtx

    class TFilterCtx {
      NO_COPY_SEMANTICS(TFilterCtx);
      public:

      TFilterCtx(const TInline::TPtr &that);
      ~TFilterCtx();

      private:
      TThatableCtx Thatable;
    }; // TFilterCtx

    class TMapCtx {
      NO_COPY_SEMANTICS(TMapCtx);
      public:
      TMapCtx(Context::TAliases &&aliases);
      ~TMapCtx();

      private:
      /* Stored because otherwise we don't know what to undo. */
      Context::TAliases Aliases;
    }; // TMapCtx

    class TReduceCtx {
      NO_COPY_SEMANTICS(TReduceCtx);
      public:

      TReduceCtx(const TInline::TPtr &carry, const TInline::TPtr &elem);
      ~TReduceCtx();
    }; // TReduceCtx

    class TSortCtx {
      NO_COPY_SEMANTICS(TSortCtx);
      public:

      TSortCtx(const TInline::TPtr &lhs, const TInline::TPtr &rhs);
      ~TSortCtx();
    }; // TSortCtx

    class TStmtCtx {
      NO_COPY_SEMANTICS(TStmtCtx);
      public:

      TStmtCtx(TStmtBlock *stmt_block);
      ~TStmtCtx();
    }; // TStmtCtx

    class TWhileCtx {
      NO_COPY_SEMANTICS(TWhileCtx);
      public:

      TWhileCtx(const TInline::TPtr &that);
      ~TWhileCtx();

      private:
      TThatableCtx Thatable;
    }; // TWhileCtx

  } // CodeGen

} // Stig