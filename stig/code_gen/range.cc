/* <stig/code_gen/range.cc>

   Implements <stig/code_gen/range.h>

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

#include <stig/code_gen/range.h>

#include <stig/type/int.h>
#include <stig/type/seq.h>

using namespace Stig::CodeGen;

TRange::TRange(const L0::TPackage *package,
               const TInline::TPtr &start,
               const TInline::TPtr &opt_limit,
               const TInline::TPtr &opt_stride,
               bool include_end)
  : TInline(package, Type::TSeq::Get(Type::TInt::Get())),
    Start(start),
    OptLimit(opt_limit),
    OptStride(opt_stride),
    IncludeEnd(include_end) {

  assert(start);
}

void TRange::WriteExpr(TCppPrinter &out) const {
  out << "TRangeGenerator::New";
  if (OptStride) {
    out << "WithSecond";
  }
  out << '(' << Start;

  if (OptLimit) {
    out << ", " << OptLimit << ", " << (IncludeEnd ? "true" : "false");
  }

  if (OptStride) {
    out << ", " << OptStride;
  }
  out << ')';
}