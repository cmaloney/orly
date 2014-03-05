/* <stig/code_gen/reduce.cc>

   Implements <stig/code_gen/reduce.h>

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

#include <stig/code_gen/reduce.h>

#include <base/not_implemented_error.h>
#include <stig/code_gen/implicit_func.h>

using namespace Stig::CodeGen;


TReduce::TPtr TReduce::New(const L0::TPackage *package,
                           const Type::TType &ret_type,
                           const TInline::TPtr &seq,
                           const TInline::TPtr &start,
                           const TImplicitFunc::TPtr &reduce_func) {
  return TPtr(new TReduce(package, ret_type, seq, start, reduce_func));
}

void TReduce::WriteExpr(TCppPrinter &out) const {
  assert(&out);

  out << "Reduce" << '(' << Seq << ", ";
  Func->WriteName(out);
  out << ", " << Start << ')';
}

TReduce::TReduce(const L0::TPackage *package,
                 const Type::TType &ret_type,
                 const TInline::TPtr &seq,
                 const TInline::TPtr &start,
                 const TImplicitFunc::TPtr &reduce_func)
  : TInline(package, ret_type),
    Func(reduce_func),
    Seq(seq),
    Start(start) {}

void TReduce::AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const {
  assert(this);
  dependency_set.insert(Seq);
  Seq->AppendDependsOn(dependency_set);
  dependency_set.insert(Start);
  Start->AppendDependsOn(dependency_set);
  dependency_set.insert(Func->GetBody());
  Func->GetBody()->AppendDependsOn(dependency_set);
}
