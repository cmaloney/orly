/* <stig/code_gen/typed_leaf.cc>

   Implements <stig/code_gen/typed_leaf.h>

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

#include <stig/code_gen/typed_leaf.h>

#include <stig/type/gen_code.h>
#include <stig/type/unwrap.h>

using namespace Stig::CodeGen;

TTypedLeaf::TTypedLeaf(const L0::TPackage *package,
                       TKind kind,
                       const Type::TType &type,
                       TAddrDir addr_dir)
    : TInline(package, type),
      Kind(kind),
      AddrDir(addr_dir) {}

void TTypedLeaf::WriteExpr(TCppPrinter &out) const {
  switch(Kind) {
    case Free:
      #if 0
      out << "Var::TVar::Free(";
      Type::GenCode(out.GetOstream(), Type::UnwrapSequence(GetReturnType()));
      out << ')';
      break;
      #endif
      out << "Native::TFree<";
      switch (AddrDir) {
        case TAddrDir::Asc: {
          out << Type::UnwrapSequence(GetReturnType());
          break;
        }
        case TAddrDir::Desc: {
          out << "TDesc<" << Type::UnwrapSequence(GetReturnType()) << ">";
          break;
        }
      }
      out << ">()";
      break;
    case Unknown: out << GetReturnType() << "()";
      break;
  }
}