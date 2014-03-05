/* <stig/code_gen/map.cc>

   Implements <stig/code_gen/map.h>

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

#include <stig/code_gen/map.h>

#include <base/not_implemented_error.h>
#include <stig/code_gen/implicit_func.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::CodeGen;

TMap::TPtr TMap::New(const L0::TPackage *package,
                     const Type::TType &ret,
                     const TSeqs &seqs,
                     const TImplicitFunc::TPtr &func) {
  return TPtr(new TMap(package, ret, seqs, func));
}

TMap::TMap(const L0::TPackage *package,
           const Type::TType &ret,
           const TSeqs &seqs,
           const TImplicitFunc::TPtr &func)
    : TInline(package, ret),
      Func(func),
      Seqs(seqs) {}

void TMap::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  if(Seqs.size() != 1) {
    //NOTE: For proper code printing we must take all the correllated sequences as a sequence of objects.
    throw Base::TNotImplementedError(HERE, "Maps containing more than one sequence do not yet have proper code printing");
  }
  out << "TMapGenerator<" << Type::UnwrapSequence(GetReturnType()) << ", "
      << Type::UnwrapSequence((*Seqs.begin())->GetReturnType()) << ">::New(";
  Func->WriteName(out);
  out  << ", " << *Seqs.begin() << ')';
}

void TMap::AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const {
  assert(this);
  for (const auto &iter : Seqs) {
    dependency_set.insert(iter);
    iter->AppendDependsOn(dependency_set);
  }
  dependency_set.insert(Func->GetBody());
  Func->GetBody()->AppendDependsOn(dependency_set);
}