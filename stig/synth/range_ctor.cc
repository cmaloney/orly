/* <stig/synth/range_ctor.cc>

   Implements <stig/synth/range_ctor.h>.

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

#include <stig/synth/range_ctor.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/pos_range.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/type/impl.h>

using namespace Stig;
using namespace Stig::Synth;

TRangeCtor::TRangeCtor(const TExprFactory *expr_factory, const Package::Syntax::TRangeCtor *range_ctor)
    : RangeCtor(Base::AssertTrue(range_ctor)),
      Start(nullptr),
      OptStride(nullptr),
      OptEnd(nullptr) {
  assert(expr_factory);
  class TRangeEndVisitor
      : public Package::Syntax::TRangeEnd::TVisitor {
    NO_COPY_SEMANTICS(TRangeEndVisitor);
    public:
    TRangeEndVisitor(const Package::Syntax::TExpr *&end, bool &end_included)
        : End(end), EndIncluded(end_included) {}
    virtual void operator()(const Package::Syntax::TClosedRangeEnd *that) const {
      End = that->GetExpr();
      EndIncluded = true;
    }
    virtual void operator()(const Package::Syntax::TOpenRangeEnd *that) const {
      End = that->GetExpr();
      EndIncluded = false;
    }
    virtual void operator()(const Package::Syntax::TUndefinedRangeEnd *) const {
      End = nullptr;
      EndIncluded = false;
    }
    private:
    const Package::Syntax::TExpr *&End;
    bool &EndIncluded;
  };  // TRangeEndVisitor
  try {
    Start = expr_factory->NewExpr(RangeCtor->GetExpr());
    auto stride = TryGetNode<Package::Syntax::TRangeStride,
                             Package::Syntax::TNoRangeStride>(RangeCtor->GetOptRangeStride());
    OptStride = stride ? expr_factory->NewExpr(stride->GetExpr()) : nullptr;
    const Package::Syntax::TExpr *end = nullptr;
    RangeCtor->GetRangeEnd()->Accept(TRangeEndVisitor(end, EndIncluded));
    OptEnd = end ? expr_factory->NewExpr(end) : nullptr;
    if (!OptEnd) { // TODO: Remove this throw when infinite series is supported
      throw TNotImplementedError(HERE, GetPosRange(RangeCtor),
            "We currently we not support Infinite range constructors");
    }
  } catch(...) {
    Cleanup();
    throw;
  }
}

TRangeCtor::~TRangeCtor() {
  Cleanup();
}

Expr::TExpr::TPtr TRangeCtor::Build() const {
  assert(this);
  return Expr::TRange::New(
            Start->Build(),
            OptStride ? OptStride->Build() : nullptr,
            OptEnd ? OptEnd->Build() : nullptr,
            EndIncluded,
            GetPosRange(RangeCtor));
}

void TRangeCtor::Cleanup() {
  assert(this);
  delete Start;
  delete OptStride;
  delete OptEnd;
}

void TRangeCtor::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Start->ForEachInnerScope(cb);
  if (OptStride) {
    OptStride->ForEachInnerScope(cb);
  }
  if (OptEnd) {
    OptEnd->ForEachInnerScope(cb);
  }
}

void TRangeCtor::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Start->ForEachRef(cb);
  if (OptStride) {
    OptStride->ForEachRef(cb);
  }
  if (OptEnd) {
    OptEnd->ForEachRef(cb);
  }
}
