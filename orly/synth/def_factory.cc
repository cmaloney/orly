/* <orly/synth/def_factory.cc>

   Implements <orly/synth/def_factory.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/synth/def_factory.h>

#include <iomanip>

#include <base/as_str.h>
#include <base/assert_true.h>
#include <orly/error.h>
#include <orly/synth/context.h>
#include <orly/synth/cst_utils.h>
#include <orly/synth/given_collector.h>
#include <orly/synth/new_expr.h>
#include <orly/synth/test_def.h>
#include <orly/synth/type_def.h>

using namespace Orly;
using namespace Orly::Synth;

TDefFactory::TDefFactory(const TExprFactory *expr_factory)
    : ExprFactory(Base::AssertTrue(expr_factory)) {}

TDefFactory::~TDefFactory() {}

void TDefFactory::NewDefs(const Package::Syntax::TOptDefSeq *opt_def_seq) const {
  assert(this);
  assert(opt_def_seq);
  ForEach<Package::Syntax::TDef>(opt_def_seq,
      [this](const Package::Syntax::TDef *def) -> bool {
        def->Accept(*this);
        return true;
      });
}

void TDefFactory::operator()(const Package::Syntax::TFuncDef *that) const {
  assert(this);
  auto name = TName(that->GetName());
  TGivenCollector::TGivenSet givens;
  TGivenCollector::CollectGivens(that->GetExpr(), givens);
  switch (givens.size()) {
    case 0: {
      new TPureFuncDef(ExprFactory, that);
      break;
    }
    case 1: {
      if (ExprFactory->EnclosingFunc) {
        new TParamFuncDef(ExprFactory, that, *(givens.begin()));
      } else {
        GetContext().AddError(
            name.GetPosRange(),
            Base::AsStr(std::quoted(name.GetText()), " contains a given but is not inside a pure function"));
      }
      break;
    }
    default: {
      GetContext().AddError(name.GetPosRange(),
                            Base::AsStr(std::quoted(name.GetText()), " contains more than one given"));
    }
  }
}

void TDefFactory::operator()(const Package::Syntax::TBadDef *) const { /* DO NOTHING */ }

void TDefFactory::operator()(const Package::Syntax::TTypeDef *that) const {
  assert(this);
  new TTypeDef(ExprFactory->OuterScope, that);
}

void TDefFactory::operator()(const Package::Syntax::TImportDef *that) const {
  throw TNotImplementedError(HERE, TPosRange(
      that->GetName()->GetLexeme().GetPosRange(),
      that->GetSemi()->GetLexeme().GetPosRange()));
}

void TDefFactory::operator()(const Package::Syntax::TGeneratorDef *that) const {
  throw TNotImplementedError(HERE, TPosRange(
      that->GetName()->GetLexeme().GetPosRange(),
      that->GetSemi()->GetLexeme().GetPosRange()));
}

void TDefFactory::operator()(const Package::Syntax::TPackageDef *that) const {
  throw TNotImplementedError(HERE, TPosRange(
      that->GetName()->GetLexeme().GetPosRange(),
      that->GetSemi()->GetLexeme().GetPosRange()));
}

void TDefFactory::operator()(const Package::Syntax::TTestDef *that) const {
  assert(this);
  new TTestDef(ExprFactory->OuterScope, that);
}
