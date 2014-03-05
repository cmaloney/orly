/* <stig/code_gen/collected_by.cc>

   Implements <stig/code_gen/collected_by.h>

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

#include <stig/code_gen/collected_by.h>

#include <stig/code_gen/implicit_func.h>

#include <stig/type/seq.h>

using namespace Stig::CodeGen;

TCollectedBy::TPtr TCollectedBy::New(
      const L0::TPackage *package,
      const Type::TType &ret_type,
      const TInline::TPtr &seq,
      const TImplicitFuncPtr &collect_func) {
  return TPtr(new TCollectedBy(package, ret_type, seq, collect_func));
}

void TCollectedBy::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  auto type = Seq->GetReturnType();
  auto seq = type.As<Type::TSeq>();
  auto elem = seq->GetElem();
  auto addr = elem.As<Type::TAddr>();
  auto elems = addr->GetElems();
  auto return_type = GetReturnType();
  auto dict = return_type.As<Type::TDict>();
  out << "CollectedBy<" << elems[0].second << ", " << elems[1].second << ", " << dict->GetVal() << ">(" << Seq << ", ";
  CollectFunc->WriteName(out);
  out << ')';
}

TCollectedBy::TCollectedBy(
    const L0::TPackage *package,
    const Type::TType &ret_type,
    const TInline::TPtr &seq,
    const TImplicitFuncPtr &collect_func)
      : TInline(package, ret_type), Seq(seq), CollectFunc(collect_func) {}

void TCollectedBy::AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const {
  assert(this);
  dependency_set.insert(Seq);
  Seq->AppendDependsOn(dependency_set);
  dependency_set.insert(CollectFunc->GetBody());
  CollectFunc->GetBody()->AppendDependsOn(dependency_set);
}