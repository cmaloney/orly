/* <stig/synth/def_factory.cc>

   Implements <stig/synth/def_factory.h>.

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

#include <stig/synth/def_factory.h>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/given_collector.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/test_def.h>
#include <stig/synth/type_def.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

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
        Tools::Nycr::TError::TBuilder(name.GetPosRange())
            << '"' << name.GetText() << "\" contains a given but is not inside a pure function";
      }
      break;
    }
    default: {
      Tools::Nycr::TError::TBuilder(name.GetPosRange())
          << '"' << name.GetText() << "\" contains more than one given";
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
