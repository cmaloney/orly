/* <stig/code_gen/slice.cc>

   Implements <stig/code_gen/slice.h>

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

#include <stig/code_gen/slice.h>

using namespace Stig::CodeGen;

TSlice::TSlice(const L0::TPackage *package,
               const Type::TType &ret_type,
               const TInline::TPtr &container,
               const TInline::TPtr &opt_lhs,
               const TInline::TPtr &opt_rhs,
               bool colon)
  : TInline(package, ret_type),
    Colon(colon),
    Container(container),
    OptLhs(opt_lhs),
    OptRhs(opt_rhs) {}


void TSlice::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << "Slice" << (Colon ? "Range" : "Single");
  if(OptLhs && OptRhs) {
    out << "Both";
  }
  out << '(' << Container;
  if(Colon && bool(OptLhs) ^ bool(OptRhs)) {
    out << ", " << (OptLhs ? "false" : "true");
  }
  if(OptLhs) {
    out << ", " << OptLhs;
  }
  if(OptRhs) {
    out << ", " << OptRhs;
  }
  out << ')';

}