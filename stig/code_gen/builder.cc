/* <stig/code_gen/builder.cc>

   Implements <stig/code_gen/builder.h>

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

#include <stig/code_gen/builder.h>

#include <base/not_implemented.h>
#include <base/time_maps.h>
#include <stig/code_gen/binary_scoped_rhs.h>
#include <stig/code_gen/collated_by.h>
#include <stig/code_gen/collected_by.h>
#include <stig/code_gen/context.h>
#include <stig/code_gen/effect.h>
#include <stig/code_gen/filter.h>
#include <stig/code_gen/interner.h>
#include <stig/code_gen/implicit_func.h>
#include <stig/code_gen/if_else.h>
#include <stig/code_gen/map.h>
#include <stig/code_gen/match.h>
#include <stig/code_gen/obj.h>
#include <stig/code_gen/reduce.h>
#include <stig/code_gen/skip.h>
#include <stig/code_gen/split.h>
#include <stig/code_gen/symbol_func.h>
#include <stig/code_gen/take.h>
#include <stig/code_gen/while.h>
#include <stig/error.h>
#include <stig/expr.h>
#include <stig/expr/util.h>
#include <stig/expr/visitor.h>
#include <stig/expr/walker.h>
#include <stig/symbol/stmt/new_and_delete.h>
#include <stig/symbol/stmt/if.h>
#include <stig/symbol/stmt/mutate.h>
#include <stig/symbol/any_function.h>
#include <stig/symbol/built_in_function.h>
#include <stig/symbol/function.h>
#include <stig/symbol/given_param_def.h>
#include <stig/symbol/result_def.h>
#include <stig/type.h>
#include <stig/type/unwrap.h>

using namespace std;
using namespace Stig;
using namespace Stig::CodeGen;

bool IsCoreSeq(const Expr::TExpr::TPtr &expr) {
  class TVisitor : public Expr::TExpr::TVisitor {
    public:

    ~TVisitor() noexcept {}
    TVisitor(bool &res) : Res(res) {}

    //NOTE: Lhs, Rhs, that are all more or less refs with magic names.
    virtual void operator()(const Expr::TAdd           *) const {}
    virtual void operator()(const Expr::TAddr          *) const {}
    virtual void operator()(const Expr::TAddrMember    *) const {}
    virtual void operator()(const Expr::TAddrOf        *) const {}
    virtual void operator()(const Expr::TAnd           *) const {}
    virtual void operator()(const Expr::TAndThen       *) const {}
    virtual void operator()(const Expr::TAs            *) const {}
    virtual void operator()(const Expr::TAssert        *) const {}
    virtual void operator()(const Expr::TCeiling       *) const {}
    virtual void operator()(const Expr::TCollatedBy    *) const { Res = true; }
    virtual void operator()(const Expr::TCollectedBy   *) const {}
    virtual void operator()(const Expr::TDict          *) const {}
    virtual void operator()(const Expr::TDiv           *) const {}
    virtual void operator()(const Expr::TEffect        *) const {}
    virtual void operator()(const Expr::TEqEq          *) const {}
    virtual void operator()(const Expr::TExp           *) const {}
    virtual void operator()(const Expr::TFilter        *) const { Res = true; }
    virtual void operator()(const Expr::TFloor         *) const {}
    virtual void operator()(const Expr::TFree          *) const {}
    virtual void operator()(const Expr::TFunctionApp   *func_app) const {
      //If the function itself returns seq w/o an implicit map, then Res = true, else Res = false.
      //Implicit map on sequence arg is handled by the core sequence finder, which will visit all the args.
      Res = func_app->GetFunction()->GetReturnType().Is<Type::TSeq>();
    }
    virtual void operator()(const Expr::TGt            *) const {}
    virtual void operator()(const Expr::TGtEq          *) const {}
    virtual void operator()(const Expr::TExists        *) const {}
    virtual void operator()(const Expr::TIfElse        *if_else) const {
      // For ifs, if the predicate is a sequence that means we __MUST__ at least
      // be mapped on that sequence and the if __MUST__ not be a core sequence for
      // code generation to work properly (The if needs to get mapped). If both the
      // predicate and the true and false cases are sequences we cannot compile it at this
      // juncture. If all three are the same core sequence it might work by pure chance.
      //
      // If the true and false cases return sequences, the if itself acts as a core sequence
      // because it does not need to be mapped. It simply returns the lhs and rhs.
      if(if_else->GetPredicate()->GetType().Is<Type::TSeq>()) {
        Res = false;
      } else if (if_else->GetTrue()->GetType().Is<Type::TSeq>() ||
                 if_else->GetFalse()->GetType().Is<Type::TSeq>()) {
        Res = true;
      }
    }
    virtual void operator()(const Expr::TIn            *) const {}
    virtual void operator()(const Expr::TIntersection  *) const {}
    virtual void operator()(const Expr::TIsEmpty       *) const {}
    virtual void operator()(const Expr::TIsKnown       *) const {}
    virtual void operator()(const Expr::TIsKnownExpr   *) const {}
    virtual void operator()(const Expr::TIsUnknown     *) const {}
    virtual void operator()(const Expr::TKeys          *) const { Res = true; }
    virtual void operator()(const Expr::TKnown         *) const {}
    virtual void operator()(const Expr::TLengthOf      *) const {}
    virtual void operator()(const Expr::TLhs           *) const { Res = true; }
    virtual void operator()(const Expr::TList          *) const {}
    virtual void operator()(const Expr::TLiteral       *) const {}
    virtual void operator()(const Expr::TLog           *) const {}
    virtual void operator()(const Expr::TLog2          *) const {}
    virtual void operator()(const Expr::TLog10         *) const {}
    virtual void operator()(const Expr::TLt            *) const {}
    virtual void operator()(const Expr::TLtEq          *) const {}
    virtual void operator()(const Expr::TMatch         *) const { Res = true; }
    virtual void operator()(const Expr::TMod           *) const {}
    virtual void operator()(const Expr::TMult          *) const {}
    virtual void operator()(const Expr::TNeq           *) const {}
    virtual void operator()(const Expr::TNegative      *) const {}
    virtual void operator()(const Expr::TNot           *) const {}
    virtual void operator()(const Expr::TNow           *) const {}
    virtual void operator()(const Expr::TObj           *) const {}
    virtual void operator()(const Expr::TObjMember     *) const {}
    virtual void operator()(const Expr::TOr            *) const {}
    virtual void operator()(const Expr::TOrElse        *) const {}
    virtual void operator()(const Expr::TPositive      *) const {}
    virtual void operator()(const Expr::TRange         *) const { Res = true; }
    virtual void operator()(const Expr::TRead          *) const {}
    // For now, all reduce statements are core sequences. Technically, sometines it could contain a core sequence which
    // is still recognizable as its original form at the end of the reduce.
    virtual void operator()(const Expr::TReduce        *) const { Res = true; }
    virtual void operator()(const Expr::TRef           *) const { Res = true; }
    virtual void operator()(const Expr::TReverseOf     *) const {}
    virtual void operator()(const Expr::TRhs           *) const { Res = true; }
    virtual void operator()(const Expr::TSessionId     *) const {}
    virtual void operator()(const Expr::TSequenceOf    *) const { Res = true; }
    virtual void operator()(const Expr::TSet           *) const {}
    virtual void operator()(const Expr::TSkip          *) const { Res = true; }
    virtual void operator()(const Expr::TSlice         *) const {}
    virtual void operator()(const Expr::TSort          *) const {}
    virtual void operator()(const Expr::TSplit         *) const { Res = true; }
    virtual void operator()(const Expr::TStart         *) const { Res = true; }
    virtual void operator()(const Expr::TSub           *) const {}
    virtual void operator()(const Expr::TSymmetricDiff *) const {}
    virtual void operator()(const Expr::TTake          *) const { Res = true; }
    virtual void operator()(const Expr::TThat          *) const { Res = true; }
    virtual void operator()(const Expr::TTimeObj       *) const {}
    virtual void operator()(const Expr::TToLower       *) const {}
    virtual void operator()(const Expr::TToUpper       *) const {}
    virtual void operator()(const Expr::TUnion         *) const {}
    virtual void operator()(const Expr::TUnknown       *) const {}
    virtual void operator()(const Expr::TUserId        *) const {}
    virtual void operator()(const Expr::TWhere         *) const {}
    virtual void operator()(const Expr::TWhile         *) const { Res = true; }
    virtual void operator()(const Expr::TXor           *) const {}

    private:
    bool &Res;
  };  // TExpr::TVisitor

  //If the return type isn't a sequence, it can never be a core sequence. Having this out here saves us from having
  //to check it in cases such as TRef, TReduce where they are a core sequence if the return type is sequence, but not
  //if the return type isn't.
  if(!expr->GetType().Is<Type::TSeq>()) {
    return false;
  }

  bool res = false;
  expr->Accept(TVisitor(res));
  return res;
}

TInline::TPtr BuildOptInline(const L0::TPackage *package, const Expr::TExpr::TPtr &expr) {
  assert(&expr);

  if (expr) {
    return BuildInline(package, expr, false);
  }
  return TInline::TPtr();
}

// Forward declaration
void Build(const L0::TPackage *package, const Symbol::Stmt::TStmtBlock::TPtr &stmts);

void Build(const L0::TPackage *package, const Symbol::Stmt::TStmt::TPtr &stmt) {
  assert(&stmt);
  assert(stmt);

  class TVisitor : public Symbol::Stmt::TStmt::TVisitor {
    NO_COPY_SEMANTICS(TVisitor);
    public:

    TVisitor(const L0::TPackage *package) : Package(package) {}

    virtual void operator()(const Symbol::Stmt::TDelete *that) const {
      Context::GetStmtBlock()->AddDelete(Package, BuildInline(Package, that->GetStmtArg()->GetExpr(), false), that->GetValueType());
    }
    virtual void operator()(const Symbol::Stmt::TIf *that) const {
      TIf::TPredicatedBlocks predicated_blocks;
      //Build all the if and else_if clauses
      for(auto &it: that->GetIfClauses()) {
        if(it->GetExpr()->GetType().Is<Type::TSeq>()) {
          //TODO: Embed a PosRange in a NOT_IMPLEMENTED
          //NOT_IMPLEMENTED_S(that->GetPosRange(), "Sequences in effecting blocks")
          NOT_IMPLEMENTED_S("Sequences in effecting blocks")
        }
        TPtr<TPredicatedBlock> pred_block(new TPredicatedBlock(BuildInline(Package, it->GetExpr(), false)));
        TStmtCtx stmt_ctx(&pred_block->GetStmts());
        Build(Package, it->GetStmtBlock());
        predicated_blocks.push_back(pred_block);
      }
      TPtr<TIf> if_(new TIf(Package, std::move(predicated_blocks)));

      //Build the else clause, if necessary.
      if(that->GetOptElseClause()) {
        if_->SetOptElseClause();
        TStmtCtx stmt_ctx(&if_->GetElseClause());
        Build(Package, that->GetOptElseClause()->GetStmtBlock());
      }
      Context::GetStmtBlock()->Add(if_);
    }
    virtual void operator()(const Symbol::Stmt::TMutate *that) const {
      if(that->GetLhs()->GetExpr()->GetType().Is<Type::TSeq>() || that->GetRhs()->GetExpr()->GetType().Is<Type::TSeq>()) {
        //TODO: Embed a PosRange in a NOT_IMPLEMENTED
        NOT_IMPLEMENTED_S("Sequences in effecting blocks");
      }
      Context::GetStmtBlock()->Add(Package, BuildInline(Package, that->GetLhs()->GetExpr(), true), that->GetMutator(),
          Build(Package, that->GetRhs()->GetExpr(), false));
    }
    virtual void operator()(const Symbol::Stmt::TNew *that) const {
      if(that->GetLhs()->GetExpr()->GetType().Is<Type::TSeq>() || that->GetRhs()->GetExpr()->GetType().Is<Type::TSeq>()) {
        //TODO: Embed a PosRange in a NOT_IMPLEMENTED
        NOT_IMPLEMENTED_S("Sequences in effecting blocks");
      }
      Context::GetStmtBlock()->AddNew(Package, BuildInline(Package, that->GetLhs()->GetExpr(), true), Build(Package, that->GetRhs()->GetExpr(), false));
    }

    private:

    const L0::TPackage *Package;

  };

  stmt->Accept(TVisitor(package));
}

void Build(const L0::TPackage *package, const Symbol::Stmt::TStmtBlock::TPtr &stmts)  {
  assert(&stmts);
  assert(stmts);

  for(auto &stmt: stmts->GetStmts()) {
    Build(package, stmt);
  }
}

//TODO: Unit test specific inlines passing through the builder.
//TODO: Containers containing only mutables should keep the mutables mutability.
/* Build builds an inline from any expression. This is what is used inside of things like implicit maps so that we can
   build the map function. This function should be used anywhere where it is not valid to introduce a sequence. If
   it is valid to introduce a sequence, such as in filter's sequence argument, then the function BuildInline should be
   used. This allows us to use the same visitor to generate code for the implicit function needed for a map as well
   as code for general use with only a single expr visitor. */
TInline::TPtr Stig::CodeGen::Build(const L0::TPackage *package, const Expr::TExpr::TPtr &expr, bool keep_mutable, bool keep_sequence) {
  assert(&expr);
  assert(expr);

  class TVisitor : public Expr::TExpr::TVisitor {
    NO_COPY_SEMANTICS(TVisitor);
    public:

    ~TVisitor() noexcept {}

    //Note:
    /* NOTES:
        We cache scope->GetInterner() as we use it a lot.
        We taker ret_type because we may have unwrapped the sequence which means that the return type of the stig expr
            is a lie. */
    TVisitor(const L0::TPackage *package, TInline::TPtr& res, Type::TType ret_type)
        : Interner(Context::GetInterner()), Res(res), ReturnType(ret_type), Package(package) {}

    virtual void operator()(const Expr::TAdd *that) const { Binary(Package, TBinary::Add, that); }
    virtual void operator()(const Expr::TAddr *that) const {
      TAddrContainer elems;
      for(auto &it: that->GetMembers()) {
        //Addr elements can't be mutable.
        elems.push_back(make_pair(it.first, Build(Package, it.second, false)));
      }

      Res = Interner.GetAddrCtor(Package, ReturnType, move(elems));
    }
    virtual void operator()(const Expr::TAddrMember  *that) const {
      Res = Interner.GetAddrMember(Package, ReturnType, Build(Package, that->GetExpr(), true), that->GetIndex());
    }
    virtual void operator()(const Expr::TAddrOf *that) const { Unary(Package, TUnary::AddressOf, that); }
    virtual void operator()(const Expr::TAnd *that) const { Binary(Package, TBinary::And, that); }
    virtual void operator()(const Expr::TAndThen *that) const {
      Res = TBinaryScopedRhs::New(Package, ReturnType, TBinaryScopedRhs::AndThen, Build(Package, that->GetLhs(), false), that->GetRhs());
    }
    //TODO: Cast should be able to maintain mutability of list elements, as well as when doing the no-op cast.
    virtual void operator()(const Expr::TAs *that) const {
      if(that->GetExpr()->GetType().Is<Type::TSeq>()) {
        Res = Interner.GetUnary(Package, ReturnType, TUnary::Cast, BuildInline(Package, that->GetExpr(), true));
      } else {
        Unary(Package, TUnary::Cast, that);
      }
    }
    virtual void operator()(const Expr::TAssert *that) const {
      //TODO: Assertion predicates?
      //TODO: Report file name with assertion failures.
      //Pass the expression through, setting it as the "that" context, and making it a local (Since context that doesn't common subexpression eliminate automatically ATM).
      Res = Build(Package, that->GetExpr(), true);
      Context::GetScope()->AddLocal(Res);
      TThatableCtx ctx(Res);

      //Build the individual asserts
      for(auto &it: that->GetAssertCases()) {
        string name = it->GetExpr()->GetPosRange().AsStr();
        if(it->HasName()) {
          name += " " + it->GetName();
        }

        //TODO: We could really move the name here...
        Context::GetScope()->AddAssertion(name, Build(Package, it->GetExpr(), false));
      }
    }
    virtual void operator()(const Expr::TCeiling *that) const { Unary(Package, TUnary::Ceiling, that); }
    virtual void operator()(const Expr::TCollatedBy *that) const {
      auto reduce_func = TImplicitFunc::New(
          Package,
          TImplicitFunc::TCause::Reduce,
          that->GetReduce()->GetType(),
          {{"carry", that->GetStart()->GetType()}, {"that", that->GetThatType()}},
          that->GetReduce(),
          true);
      auto having_func = TImplicitFunc::New(
          Package,
          TImplicitFunc::TCause::While,
          that->GetHaving()->GetType(),
          {{"that", that->GetThatType()}},
          that->GetHaving(),
          false);
      /* Context */ {
        TReduceCtx ctx(reduce_func->GetArg("carry"), reduce_func->GetArg("that"));
        reduce_func->Build();
      }  // Context
      /* Context */ {
        TWhileCtx ctx(having_func->GetArg("that"));
        having_func->Build();
      }  // Context
      Res = TCollatedBy::New(
              Package,
              ReturnType,
              BuildInline(Package, that->GetSeq(), false),
              Build(Package, that->GetStart()->GetExpr(), false),
              reduce_func,
              having_func);
    }
    virtual void operator()(const Expr::TCollectedBy *that) const {
      auto collect_func = TImplicitFunc::New(
          Package,
          TImplicitFunc::TCause::Collect,
          that->GetRhs()->GetType(),
          {{"lhs", that->GetLhsRhsType()}, {"rhs", that->GetLhsRhsType()}},
          that->GetRhs(),
          false);
      /* Context */ {
        TSortCtx ctx(collect_func->GetArg("lhs"), collect_func->GetArg("rhs"));
        collect_func->Build();
      }  // Context
      Res = TCollectedBy::New(
              Package,
              ReturnType,
              BuildInline(Package, that->GetLhs(), false),
              collect_func);
    }
    virtual void operator()(const Expr::TDict *that) const {
      TDictContainer elems;
      for(auto &it: that->GetMembers()) {
        //Things in dicts can't be mutable.
        elems.insert(make_pair(Build(Package, it.first, false), Build(Package, it.second, false)));
      }
      Res = TBasicCtor<TDictContainer>::TPtr(new TBasicCtor<TDictContainer>(Package, ReturnType, move(elems)));
    }
    virtual void operator()(const Expr::TDiv *that) const { Binary(Package, TBinary::Div, that); }
    virtual void operator()(const Expr::TEffect *that) const {
      //NOTE: The effecting gets attached to the nearest CodeScope. The expression the effect is genned and returned here.
      Res = Build(Package, that->GetExpr(), true);
      auto non_mutable = Build(Package, that->GetExpr(), false);
      Context::GetScope()->AddLocal(Res); // Make 'that' a local so it is guaranteed to appear first.

      TThatableCtx ctx(non_mutable);
      ::Build(Package, that->GetStmtBlock());
    }
    virtual void operator()(const Expr::TEqEq *that) const { Binary(Package, TBinary::EqEq, that); }
    virtual void operator()(const Expr::TExists *that) const {
      #if 0
      Unary(Package, TUnary::Exists, that);
      #endif
      Res = Interner.GetExists(Package, ReturnType, that->GetValueType(), BuildInline(Package, that->GetExpr(), false));
    }
    virtual void operator()(const Expr::TExp *that) const { Binary(Package, TBinary::Exponent, that); }
    virtual void operator()(const Expr::TFilter *that) const {
      auto seq = BuildInline(Package, that->GetLhs(), false);

      //TODO: Filling in the "that" arg then using GetArg to get it back is more than sort of fugly.
      //TODO: Common sub expression elimination for the filter func.
      TImplicitFunc::TPtr filter_func = TImplicitFunc::New(Package, TImplicitFunc::TCause::Filter,
        Type::TBool::Get(), {{"that", Type::UnwrapSequence(that->GetLhs()->GetType())}}, that->GetRhs(), false);
      /* Function Context */ {
        TFilterCtx ctx(filter_func->GetArg("that"));
        filter_func->Build();
      }

      Res = TFilter::New(Package, ReturnType, seq, filter_func);
    }

    virtual void operator()(const Expr::TFloor *that) const { Unary(Package, TUnary::Floor, that); }
    virtual void operator()(const Expr::TFree *that) const {
      Res = Interner.GetTypedLeaf(Package, TTypedLeaf::Free, that->GetType(), that->GetDir());
    }
    virtual void operator()(const Expr::TFunctionApp *that) const {
      class TVisitor
          : public Symbol::TAnyFunction::TVisitor {
        NO_COPY_SEMANTICS(TVisitor);
        public:
        TVisitor(TInterner &interner, TInline::TPtr &res, const Expr::TFunctionApp::TFunctionAppArgMap &function_app_args, const L0::TPackage *package)
            : Interner(interner), Res(res), FunctionAppArgs(function_app_args), Package(package) {}
        virtual void operator()(const Symbol::TFunction *that) const {
          auto func = TSymbolFunc::Find(that);
          TCall::TArgs args;
          for (auto &arg: FunctionAppArgs) {
            // TODO: The whole using string to look up source arg in the code gen argument map is more than slightly fugly.
            //       Should really have a way to jump straight from func app arg to param def that caused it and thus type.
            // TODO: We need to check if the arg has a greater sequence arity than the parameter, and if so, it is an implicit
            //       map on the arg.
            TInline::TPtr arg_inline;
            auto param_type = func->GetArg(arg.first)->GetReturnType();
            if (param_type.Is<Type::TSeq>()) {
              arg_inline = BuildInline(Package, arg.second->GetExpr(), false);
            } else {
              arg_inline = Build(Package, arg.second->GetExpr(), param_type.Is<Type::TMutable>());
            }
            assert(arg_inline);
            args.push_back(arg_inline);
          }
          Res = Interner.GetCall(Package, func, args);
        }
        virtual void operator()(const Symbol::TBuiltInFunction *that) const {
          TBuiltInCall::TArguments args;
          for(auto &arg: FunctionAppArgs) {
            args.insert({arg.first, Build(Package, arg.second->GetExpr(), false)});
          }
          Res = Interner.GetBuiltInCall(Package, that->GetPtr(), std::move(args));
        }
        private:
        TInterner &Interner;
        TInline::TPtr &Res;
        const Expr::TFunctionApp::TFunctionAppArgMap &FunctionAppArgs;
        const L0::TPackage *Package;
      };  // TVisitor
      that->GetFunction()->Accept(TVisitor(Interner, Res, that->GetFunctionAppArgs(), Package));
    }
    virtual void operator()(const Expr::TGt *that) const { Binary(Package, TBinary::Gt, that); }
    virtual void operator()(const Expr::TGtEq *that) const { Binary(Package, TBinary::GtEq, that); }
    virtual void operator()(const Expr::TIfElse *that) const {
      //TODO: Intern if_else.
      Res = TIfElse::New(Package, ReturnType, that->GetTrue(), Build(Package, that->GetPredicate(), false) , that->GetFalse());
    }
    virtual void operator()(const Expr::TIn *that) const { Binary(Package, TBinary::In, that); }
    virtual void operator()(const Expr::TIntersection *that) const { Binary(Package, TBinary::Intersection, that); }
    virtual void operator()(const Expr::TIsEmpty *that) const { Unary(Package, TUnary::IsEmpty, that); }
    virtual void operator()(const Expr::TIsKnown *that) const { Unary(Package, TUnary::IsKnown, that); }
    virtual void operator()(const Expr::TIsKnownExpr *that) const { Binary(Package, TBinary::IsKnownExpr, that); }
    virtual void operator()(const Expr::TIsUnknown *that) const { Unary(Package, TUnary::IsUnknown, that); }
    virtual void operator()(const Expr::TKeys *that) const {
      TKeys::TAddrElems elems;
      for(auto &it: that->GetMembers()) {
        elems.push_back(make_pair(it.first, Build(Package, it.second, false)));
      }
      Res = Interner.GetKeys(Package, that->GetType(), that->GetValueType(), move(elems));
    }
    virtual void operator()(const Expr::TKnown *that) const { Unary(Package, TUnary::Known, that); }
    virtual void operator()(const Expr::TLengthOf *that) const { Unary(Package, TUnary::LengthOf, that); }
    virtual void operator()(const Expr::TLhs *) const { Res = Context::GetLhs(); }
    virtual void operator()(const Expr::TList *that) const {
      TListContainer elems;
      for(auto &it: that->GetExprs()) {
        elems.push_back(Build(Package, it, false));
      }

      Res = Interner.GetListCtor(Package, ReturnType, move(elems));
    }
    virtual void operator()(const Expr::TLiteral *that) const { Res = Interner.GetLiteral(Package, that->GetVal()); }
    virtual void operator()(const Expr::TLog *that) const { Unary(Package, TUnary::Log, that); }
    virtual void operator()(const Expr::TLog2 *that) const { Unary(Package, TUnary::Log2, that); }
    virtual void operator()(const Expr::TLog10 *that) const { Unary(Package, TUnary::Log10, that); }
    virtual void operator()(const Expr::TLt *that) const { Binary(Package, TBinary::Lt, that); }
    virtual void operator()(const Expr::TLtEq *that) const { Binary(Package, TBinary::LtEq, that); }
    virtual void operator()(const Expr::TMatch *that) const {
      Res = TMatch::New(Package, Build(Package, that->GetLhs(), false), Build(Package, that->GetRhs(), false)); }
    virtual void operator()(const Expr::TMod *that) const { Binary(Package, TBinary::Modulo, that); }
    virtual void operator()(const Expr::TMult *that) const { Binary(Package, TBinary::Mult, that); }
    virtual void operator()(const Expr::TNeq *that) const { Binary(Package, TBinary::Neq, that); }
    virtual void operator()(const Expr::TNegative *that) const { Unary(Package, TUnary::Negative, that); }
    virtual void operator()(const Expr::TNot *that) const { Unary(Package, TUnary::Not, that); }
    virtual void operator()(const Expr::TNow *) const { Res = Interner.GetContextVar(Package, TContextVar::Now); }
    virtual void operator()(const Expr::TObj *that) const {
      TObjCtor::TArgs args;
      for(auto &it: that->GetMembers()) {
        args.insert(make_pair(it.first, Build(Package, it.second, false)));
      }

      //TODO: Intern this, has the same problem TDict does.
      Res = TObjCtor::TPtr(new TObjCtor(Package, ReturnType, move(args)));
    }
    virtual void operator()(const Expr::TObjMember *that) const {
      Res = Interner.GetObjMember(Package, ReturnType, Build(Package, that->GetExpr(), true), that->GetName());
    }
    virtual void operator()(const Expr::TOr *that) const { Binary(Package, TBinary::Or, that); }
    virtual void operator()(const Expr::TOrElse *that) const {
      Res = TBinaryScopedRhs::New(Package, ReturnType, TBinaryScopedRhs::OrElse, Build(Package, that->GetLhs(), false), that->GetRhs());
    }
    virtual void operator()(const Expr::TPositive *that) const { Res = Build(Package, that->GetExpr(), true); }
    virtual void operator()(const Expr::TRange *that) const {
      //TODO: Naming dissonance of members / parameters
      Res = Interner.GetRange(Package, Build(Package, that->GetStart(), false),
          BuildOptInline(Package, that->GetOptEnd()), BuildOptInline(Package, that->GetOptStride()),
          that->HasEndIncluded());
    }
    virtual void operator()(const Expr::TRead *that) const { Unary(Package, TUnary::Read, that); }
    virtual void operator()(const Expr::TReduce *that) const {
      // Build function
      TImplicitFunc::TPtr func = TImplicitFunc::New(Package, TImplicitFunc::TCause::Reduce, that->GetRhs()->GetType(),
          {{"carry", that->GetStart()->GetType()}, {"that", that->GetThatType()}}, that->GetRhs(), true);
      /* Reduce ctx */ {
        TReduceCtx ctx(func->GetArg("carry"), func->GetArg("that"));
        func->Build();
      }

      //TODO: Intern this (Functions aren't interned, so doing it now doesn't make sense.
      Res = TReduce::New(Package, ReturnType, BuildInline(Package, that->GetLhs(), false),
          Build(Package, that->GetStart()->GetExpr(), false), func);
    }
    virtual void operator()(const Expr::TRef *that) const {
      class TVisitor : public Symbol::TDef::TVisitor {
        public:
        TVisitor(TInterner &interner, TInline::TPtr &res, const L0::TPackage *package) : Interner(interner), Res(res), Package(package) {}
        virtual void operator()(const Symbol::TGivenParamDef *that) const {
          Res = TSymbolFunc::Find(that->GetFunction().get())->GetArg(that->GetName());
        }
        virtual void operator()(const Symbol::TResultDef *that) const {
          class TVisitor
              : public Symbol::TAnyFunction::TVisitor {
            NO_COPY_SEMANTICS(TVisitor);
            public:
            TVisitor(TInterner &interner, TInline::TPtr &res, const L0::TPackage *package)
                : Interner(interner), Res(res), Package(package) {}
            virtual void operator()(const Symbol::TFunction *that) const {
              Res = Interner.GetCall(Package, TSymbolFunc::Find(that), TCall::TArgs {});
            }
            virtual void operator()(const Symbol::TBuiltInFunction *) const {
              // NOTE: if we introduce a zero parameter built in function, this needs to change
              throw Base::TImpossibleError(HERE);
            }
            TInterner &Interner;
            TInline::TPtr &Res;
            const L0::TPackage *Package;
          };  // TVisitor
          that->GetFunction()->Accept(TVisitor(Interner, Res, Package));
        }
        private:
        TInterner &Interner;
        TInline::TPtr &Res;
        const L0::TPackage *Package;
      }; // TVisitor
      that->GetDef()->Accept(TVisitor(Interner, Res, Package));
    }
    virtual void operator()(const Expr::TReverseOf *that) const { Unary(Package, TUnary::ReverseOf, that); }
    virtual void operator()(const Expr::TRhs *) const { Res = Context::GetRhs(); }
    virtual void operator()(const Expr::TSet *that) const {
      TSetContainer elems;
      for(auto &it: that->GetExprs()) {
        elems.insert(Build(Package, it, false));
      }

      Res = TBasicCtor<TSetContainer>::TPtr(new TBasicCtor<TSetContainer>(Package, ReturnType, move(elems)));

      //TODO: Requires comparison of sets: Res = Interner.GetSetCtor(ReturnType, move(elems));
    }
    virtual void operator()(const Expr::TSessionId *) const { Res = Interner.GetContextVar(Package, TContextVar::SessionId); }
    virtual void operator()(const Expr::TSequenceOf *that) const { Unary(Package, TUnary::SequenceOf, that); }
    virtual void operator()(const Expr::TSkip *that) const {
      Res = TSkip::New(Package, ReturnType, BuildInline(Package, that->GetLhs(), false), Build(Package, that->GetRhs(), false));
    }
    virtual void operator()(const Expr::TSlice       *that) const {
      //TODO: Slice should maintain mutability of container elements.
      if(that->GetType().Is<Type::TMutable>() && (that->HasColon() || that->GetOptRhs())) {
        //TODO: Embed a PosRange in a NOT_IMPLEMENTED
        NOT_IMPLEMENTED_S("Maintaining mutability through a range slice");
      }
      Res = Interner.GetSlice(Package, ReturnType, Build(Package, that->GetContainer(), true),
          BuildOptInline(Package, that->GetOptLhs()), BuildOptInline(Package, that->GetOptRhs()), that->HasColon());
    }
    virtual void operator()(const Expr::TSort *that) const {
      //NOTE: if the dynamic_cast assertion in as fails, that means that we have sorted something other than a list and
      //      how we find the element type needs to be updated.
      TImplicitFunc::TPtr func = TImplicitFunc::New(Package, TImplicitFunc::TCause::Sort,
          Type::TBool::Get(), {{"lhs", that->GetLhsRhsType()}, {"rhs", that->GetLhsRhsType()}}, that->GetRhs(), false);
      /* Context */ {
        //TODO: The re-discovery by string look up of function arguments is sort of ug.
        TSortCtx ctx(func->GetArg("lhs"), func->GetArg("rhs"));
        func->Build();
      }

      //TODO: Intern
      Res = TSort::New(Package, ReturnType, BuildInline(Package, that->GetLhs(), false), func);

    }
    virtual void operator()(const Expr::TSplit *that) const {
      Res = TSplit::New(Package, Build(Package, that->GetLhs(), false), Build(Package, that->GetRhs(), false)); }
    virtual void operator()(const Expr::TStart *) const { Res = Context::GetStart(); }
    virtual void operator()(const Expr::TSub *that) const { Binary(Package, TBinary::Sub, that); }
    virtual void operator()(const Expr::TSymmetricDiff *that) const { Binary(Package, TBinary::SymmetricDiff, that); }
    virtual void operator()(const Expr::TTake *that) const {
      Res = TTake::New(Package, ReturnType, BuildInline(Package, that->GetLhs(), false), Build(Package, that->GetRhs(), false));
    }
    virtual void operator()(const Expr::TThat *) const { Res = Context::GetThat(); }
    virtual void operator()(const Expr::TTimeObj *that) const {
      auto type = that->GetType();
      const Type::TObj *result = type.TryAs<Type::TObj>();

      // This is overloaded so you have to check structurally which type you have
      if (result) {
        if (result->GetElems() == *Base::Chrono::TimeDiffMap) {
	  Unary(Package, TUnary::TimeDiffObj, that);
	} else if (result->GetElems() == *Base::Chrono::TimePntMap) {
	  Unary(Package, TUnary::TimePntObj, that);
	} else {
	  throw TCompileError(HERE, that->GetPosRange(), "Tried to turn a non-time type into a time object");
	}
      } else {
	throw TCompileError(HERE, that->GetPosRange(), "Cannot convert this into a time object");
      }
    }
    virtual void operator()(const Expr::TToLower *that) const { Unary(Package, TUnary::ToLower, that); }
    virtual void operator()(const Expr::TToUpper *that) const { Unary(Package, TUnary::ToUpper, that); }
    virtual void operator()(const Expr::TUnion *that) const { Binary(Package, TBinary::Union, that); }
    virtual void operator()(const Expr::TUnknown *that) const {
      Res = Interner.GetTypedLeaf(Package, TTypedLeaf::Unknown, that->GetType(), /*ignored */ TAddrDir::Asc);
    }
    virtual void operator()(const Expr::TUserId *) const { Res = Interner.GetContextVar(Package, TContextVar::UserId); }
    virtual void operator()(const Expr::TWhere *that) const { Res = Build(Package, that->GetExpr(), true); }
    virtual void operator()(const Expr::TWhile *that) const {
      auto seq = BuildInline(Package, that->GetLhs(), false);

      //TODO: Filling in the "that" arg then using GetArg to get it back is more than sort of fugly.
      //TODO: Common sub expression elimination for the filter func.
      TImplicitFunc::TPtr while_func = TImplicitFunc::New(Package, TImplicitFunc::TCause::While,
        Type::TBool::Get(), {{"that", Type::UnwrapSequence(that->GetLhs()->GetType())}}, that->GetRhs(), false);
      /* Function Context */ {
        TWhileCtx ctx(while_func->GetArg("that"));
        while_func->Build();
      }

      Res = TWhile::New(Package, ReturnType, seq, while_func);
    }
    virtual void operator()(const Expr::TXor *that) const { Binary(Package, TBinary::Xor, that); }
    private:
    void Binary(const L0::TPackage *package, TBinary::TOp op, const Expr::TBinary *that) const {
      Res = Interner.GetBinary(package, ReturnType, op, Build(Package, that->GetLhs(), false),
          Build(Package, that->GetRhs(), false));
    }
    void Unary(const L0::TPackage *package, TUnary::TOp op, const Expr::TUnary *that) const {
      //In all cases, we don't want the mutable, unless we're doing address of.
      Res = Interner.GetUnary(package, ReturnType, op, Build(Package, that->GetExpr(), op == TUnary::AddressOf));
    }

    TInterner &Interner;
    TInline::TPtr &Res;
    Type::TType ReturnType;
    const L0::TPackage *Package;

  };  // TVisitor

  /* extra */ {
    auto alias = Context::GetOptOverride(expr);

    if(alias) {
      return alias;
    }
  }

  TInline::TPtr res;

  expr->Accept(TVisitor(package, res, keep_sequence ? expr->GetType() : Type::UnwrapSequence(expr->GetType())));
  assert(res);
  if (!keep_mutable && res->GetReturnType().Is<Type::TMutable>()) {
    res = Context::GetInterner().GetUnary(package, Type::UnwrapMutable(res->GetReturnType()), TUnary::UnwrapMutable, res);
  }
  assert(res);
  return res;
}

TInline::TPtr BuildMap(const L0::TPackage *package, const Expr::TExpr::TPtr &expr, bool keep_mutable) {
  assert(&expr);
  assert(expr->GetType().Is<Type::TSeq>());
  assert(!IsCoreSeq(expr));

  /* Sequence building

  We have two types of sequences
  1) Core sequences. These are expressions which give rise to a sequence. Those are built by BuildInline.
  2) Implicit maps. These are expressions that apply non-sequence-friendly ops to sequences. This kind is also created
     By free expresssions (Each free is in effect a sequence.)

  For core sequences we don't have to do anything special. For implicit maps however. when we hit an expression giving
  rise to an implicit map sequence, we have to first hold everything and find its core sequences. All of these core
  sequences __must__ be correllated. Once we have the core sequences, we add to our context maps from their particular
  expressions to the argument in the implicitly created function that performs the mapping.

  We then recuse through the expression tree building inlines and ignoring the fact that non-core sequences return sequences
  (At the same time we eat all the return types to not be sequences). */

  //Gather all the core sequences.
  unordered_set<Expr::TExpr::TPtr> core_seqs;
  unordered_multimap<const Symbol::TDef*, Expr::TExpr::TPtr> core_def_seqs;
  Expr::ForEachExpr(expr, [&core_seqs, &core_def_seqs](const Expr::TExpr::TPtr &expr) -> bool {
    if(IsCoreSeq(expr)) {
      auto ref_expr = expr->TryAs<Expr::TRef>();
      if(ref_expr) {
        core_def_seqs.insert(make_pair(ref_expr->GetDef(), expr));
      } else {
        core_seqs.insert(expr);
        return true;
      }
    }
    return false;
  });

  //If this fails, then it means it looks like synth gave us a map with no sequence that caused it...
  assert(core_seqs.size() + core_def_seqs.size() > 0);

  //Ensure they are all correlated if there is more than one and build up the implicit function's parameter map.
  //Every sequence __must__ be a ref to a result def which arises from the same zero-parameter function OR there must be
  //one and only one core seq.
  if(!((core_seqs.size() == 1) ^ (!core_def_seqs.empty()))) {
    /* TODO: It would be nice to include the individual sequence's pos ranges in the message. */
    cout << "Sequence Locations: " << endl;
    cout << "CORE" << endl;
    for(auto &it: core_seqs) {
      cout << it->GetPosRange() << ", ";
    }
    cout << endl << "CORE DEF";
    for(auto &it: core_def_seqs) {
      cout << it.first->GetPosRange() << ", ";
    }
    /**/
    throw TCompileError(HERE, expr->GetPosRange(), "Uncorrelated sequences in a map containing multiple sequences. "
        "To use multiple sequences in a map, they must all come from the same zero-parameter function.");
  }
  TFunction::TNamedArgs args;
  if (!core_seqs.empty()) {
    args.insert(make_pair("seq", Type::UnwrapSequence((*core_seqs.begin())->GetType())));
  }
  //Check the defs all point to the same function.
  /* extra */ {
    Symbol::TAnyFunction::TPtr func = nullptr;
    for(auto &it: core_def_seqs) {
      class TVisitor : public Symbol::TDef::TVisitor {
        NO_COPY_SEMANTICS(TVisitor);
        public:

        TVisitor(TFunction::TNamedArgs &args, Symbol::TAnyFunction::TPtr &func) : Args(args), Func(func) {}

        virtual void operator()(const Symbol::TGivenParamDef *) const {
          //TODO: Embed a PosRange in a NOT_IMPLEMENTED
          NOT_IMPLEMENTED_S("We don't currently support correlated parameter sequences");
        }
        virtual void operator()(const Symbol::TResultDef *that) const {
          if (Func) {
            if(that->GetFunction() != Func) {
              //TODO: Would be nice to include the overall expr pos_range, as well as the conflicting sequences pos ranges.
              throw TCompileError(HERE, that->GetPosRange(), "Uncorrelated sequence in map containing multiple sequences.");
            }
          } else {
            Func = that->GetFunction();
          }
          //NOTE: We don't assert the insertion passes, as we will call insert on the same name multiple times for
          //correlated sequences.
          Args.insert(make_pair(that->GetName(), Type::UnwrapSequence(that->GetType())));
        }

        private:
        TFunction::TNamedArgs &Args;
        Symbol::TAnyFunction::TPtr &Func;
      };

      it.first->Accept(TVisitor(args, func));
    }
  }

  //Build the mapped function.
  TImplicitFunc::TPtr func = TImplicitFunc::New(package, TImplicitFunc::TCause::Map,
      Type::UnwrapSequence(expr->GetType()), args, expr, keep_mutable);

  /* Map Context */ {
    Context::TAliases overrides;
    //NOTE: We already validated that all the sequences are correlated / point to the same source.
    //TODO: The reacquisition of argument by string is sort of ugly.
    if(!core_seqs.empty()) {
      overrides.insert(make_pair((*core_seqs.begin()), func->GetArg("seq")));
    }
    for(auto &it: core_def_seqs) {
      overrides.insert(make_pair(it.second, func->GetArg(it.first->GetName())));
    }
    TMapCtx ctx(std::move(overrides));
    func->Build();
  }

  //Build the sequence symbols
  unordered_set<TInline::TPtr> seq_inlines;
  if(!core_seqs.empty()) {
    seq_inlines.insert(BuildInline(package, *core_seqs.begin(), true));
  }
  //NOTE: We're relying on the common sub expression elimination of TInlines to condense multiple refs to a single ref inline.
  for(auto &it: core_def_seqs) {
    seq_inlines.insert(BuildInline(package, it.second, true));
  }

  //Return the implicit map.
  //TODO: Intern implicit maps.
  return TMap::New(package, expr->GetType(), seq_inlines, func);
}

/* NOTE: Use this function when it is okay to introduce a sequence from scratch. This means at roots as well as
         builtins such as reduce in the sequence argument. */
TInline::TPtr Stig::CodeGen::BuildInline(const L0::TPackage *package, const Expr::TExpr::TPtr &expr, bool keep_mutable) {
  /* If we're a map, then switch to the map building logic. Otherwise, continue as we were. We only handle maps special
     and not the "core" sequences because we have to collect their source sequences and generate a function which likely
     captures multiple expression tree nodes. */
  if (expr->GetType().Is<Type::TSeq>()) {
    if(IsCoreSeq(expr)) {
      return Build(package, expr, keep_mutable, true);
    } else {
      return BuildMap(package, expr, keep_mutable);
    }
  }

  return Build(package, expr, keep_mutable);

}