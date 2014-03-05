/* <stig/code_gen/collated_by.cc>

   Implements <stig/code_gen/collated_by.h>

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

#include <stig/code_gen/collated_by.h>

#include <base/split.h>
#include <stig/code_gen/implicit_func.h>
#include <stig/type/unwrap.h>

using namespace Stig::CodeGen;

TCollatedBy::TPtr TCollatedBy::New(
      const L0::TPackage *package,
      const Type::TType &ret_type,
      const TInline::TPtr &seq,
      const TInline::TPtr &start,
      const TImplicitFunc::TPtr &reduce_func,
      const TImplicitFunc::TPtr &having_func) {
  return TPtr(new TCollatedBy(package, ret_type, seq, start, reduce_func, having_func));
}

void TCollatedBy::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  out << "TCollatedByGenerator<"
      << Type::UnwrapSequence(GetReturnType())
      << ", " << Type::UnwrapSequence(Seq->GetReturnType())
      << ", " << Type::UnwrapSequence(ReduceFunc->GetReturnType())
      << ", " << Type::UnwrapSequence(HavingFunc->GetReturnType())
      << ">::New(";
  ReduceFunc->WriteName(out);
  out << ", ";
  HavingFunc->WriteName(out);
  out << ", " << Seq << ", " << Start << ')';
}

TCollatedBy::TCollatedBy(
      const L0::TPackage *package,
      const Type::TType &ret_type,
      const TInline::TPtr &seq,
      const TInline::TPtr &start,
      const TImplicitFunc::TPtr &reduce_func,
      const TImplicitFunc::TPtr &having_func)
  : TInline(package, ret_type),
    HavingFunc(having_func),
    ReduceFunc(reduce_func),
    Seq(seq),
    Start(start) {}

void TCollatedBy::AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const {
  assert(this);
  dependency_set.insert(Seq);
  Seq->AppendDependsOn(dependency_set);
  dependency_set.insert(Start);
  Start->AppendDependsOn(dependency_set);
  dependency_set.insert(ReduceFunc->GetBody());
  ReduceFunc->GetBody()->AppendDependsOn(dependency_set);
  dependency_set.insert(HavingFunc->GetBody());
  HavingFunc->GetBody()->AppendDependsOn(dependency_set);
}
