/* <stig/code_gen/context.cc>

   Implements <stig/code_gen/context.h>

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

#include <stig/code_gen/context.h>

#include <base/assert_true.h>
#include <stig/code_gen/function.h>
#include <stig/code_gen/scope.h>

//TODO: Dependency inject scope and function?

using namespace Stig::CodeGen;

std::stack<TInline::TPtr> Context::Lhs, Context::Rhs, Context::Start, Context::That;
std::stack<TFunction*> Context::NearestFunc;
Context::TAliases Context::Overrides;
std::stack<TCodeScope*> Context::Scope;
std::stack<TStmtBlock*> Context::StmtBlock;

TInline::TPtr Context::GetLhs() {
  assert(!Lhs.empty());

  return Lhs.top();
}

TInline::TPtr Context::GetOptOverride(const TExprPtr &expr) {
  auto res = Overrides.find(expr);
  if(res == Overrides.end()) {
    return 0;
  } else {
    return res->second;
  }
}

TInterner &Context::GetInterner() {
  return *Base::AssertTrue(GetScope()->GetInterner());
}


TFunction *Context::GetNearestFunc() {
  assert(!NearestFunc.empty());
  return NearestFunc.top();
}

TInline::TPtr Context::GetRhs() {
  assert(!Rhs.empty());

  return Rhs.top();
}

TCodeScope* Context::GetScope() {
  assert(!Scope.empty());

  return Scope.top();
}

TInline::TPtr Context::GetStart() {
  assert(!Start.empty());

  return Start.top();
}

TStmtBlock *Context::GetStmtBlock() {
  assert(!StmtBlock.empty());

  return StmtBlock.top();
}

TInline::TPtr Context::GetThat() {
  assert(!That.empty());

  //TODO: Increment the reference count on the Inline ptr so that it common subexpression eliminates if used more than once.
  return That.top();
}


TThatableCtx::TThatableCtx(const TInline::TPtr &that) {
  Context::That.push(that);
}

TThatableCtx::~TThatableCtx() {
  Context::That.pop();
}

TFilterCtx::TFilterCtx(const TInline::TPtr &that) : Thatable(that) {}

TFilterCtx::~TFilterCtx() {}

TFunctionCtx::TFunctionCtx(TFunction* func) : Scope(func->GetCodeScope()) {
  Context::NearestFunc.push(func);
}
TFunctionCtx::~TFunctionCtx() {
  Context::NearestFunc.pop();
}

TMapCtx::TMapCtx(Context::TAliases &&aliases) : Aliases(std::move(aliases)) {
  for(auto &it: Aliases) {
    /* NOTE: If this assertion fails, then we have overriden the same expression twice, which is illegal. */
    auto res = Context::Overrides.insert(it).second;
    assert(res);
  }
}

TMapCtx::~TMapCtx() {
  for(auto &it: Aliases) {
    auto num_erased = Context::Overrides.erase(it.first);

    /* The item should have been in the map once, and only once. */
    assert(num_erased == 1);
  }
}


TReduceCtx::TReduceCtx(const TInline::TPtr &carry, const TInline::TPtr &elem) {
  Context::Start.push(carry);
  Context::That.push(elem);
}

TReduceCtx::~TReduceCtx() {
  Context::That.pop();
  Context::Start.pop();
}

TScopeCtx::TScopeCtx(TCodeScope *scope) {
  Context::Scope.push(scope);
  Context::StmtBlock.push(&scope->GetStmts());
}
TScopeCtx::~TScopeCtx() {
  Context::StmtBlock.pop();
  Context::Scope.pop();
}

TSortCtx::TSortCtx(const TInline::TPtr &lhs, const TInline::TPtr &rhs) {
  Context::Lhs.push(lhs);
  Context::Rhs.push(rhs);
}

TSortCtx::~TSortCtx() {
  Context::Rhs.pop();
  Context::Lhs.pop();
}

TStmtCtx::TStmtCtx(TStmtBlock *stmt_block) {
  Context::StmtBlock.push(stmt_block);
}

TStmtCtx::~TStmtCtx() {
  Context::StmtBlock.pop();
}

TWhileCtx::TWhileCtx(const TInline::TPtr &that) : Thatable(that) {}

TWhileCtx::~TWhileCtx() {}