/* <stig/code_gen/sort.cc>

   Implements <stig/code_gen/sort.h>

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

#include <stig/code_gen/sort.h>

#include <stig/code_gen/implicit_func.h>

using namespace Stig::CodeGen;

TSort::TPtr TSort::New(const L0::TPackage *package,
                       const Type::TType &ret_type,
                       const TInline::TPtr &container,
                       const TImplicitFuncPtr &func) {
  return TPtr(new TSort(package, ret_type, container, func));
}

void TSort::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << "Sort(" << Container << ", ";
  Func->WriteName(out);
  out << ')';
}

TSort::TSort(const L0::TPackage *package,
             const Type::TType &ret_type,
             const TInline::TPtr &container,
             const TImplicitFuncPtr &func)
    : TInline(package, ret_type), Container(container), Func(func) {}

void TSort::AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const {
  assert(this);
  dependency_set.insert(Container);
  Container->AppendDependsOn(dependency_set);
  dependency_set.insert(Func->GetBody());
  Func->GetBody()->AppendDependsOn(dependency_set);
}