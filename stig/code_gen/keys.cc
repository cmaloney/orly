/* <stig/code_gen/keys.cc>

   Implements <stig/code_gen/keys.h>

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

#include <stig/code_gen/keys.h>

#include <base/split.h>
#include <stig/type/seq.h>
#include <stig/type/unwrap.h>

using namespace Base;
using namespace Stig;
using namespace Stig::CodeGen;

TKeys::TKeys(const L0::TPackage *package,
             const Type::TType &ret_type,
             const Type::TType &val_type,
             TAddrElems &&addr_elems)
    : TInline(package, ret_type),
      AddrElems(addr_elems),
      ValType(val_type) {}

void TKeys::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  #if 0
  out << "ctx.New<" << Type::UnwrapSequence(GetReturnType()) << ">(ctx.GetFlux(), Var::TVar::Addr({";
  #if 0
  out << "Spa::FluxCapacitor::TKeyGenerator<" << Type::UnwrapSequence(GetReturnType()) << ">::New(ctx.GetFlux(), Var::TVar::Addr({";
  #endif
  Join(", ", AddrElems, [](TAddrElems::const_reference it, TCppPrinter &out) {
    //NOTE: This sometimes will cause
    out << '{';
    WriteCppType(out.GetOstream(), it.first);
    out << ", Var::TVar(" << it.second << ")}";
  }, out);
  out << "}))";
  #endif
  out << "ctx.New<" << Type::UnwrapSequence(GetReturnType()) << ">(ctx.GetFlux(), ";
  /* this is where we put the index id */ {
    Type::TType addr_type = Type::UnwrapSequence(GetReturnType());
    const Base::TUuid &index_id = Package->GetIndexIdFor(addr_type, ValType);
    char uuid[37];
    index_id.FormatUnderscore(uuid);
    out << TStigNamespace(Package->GetNamespace()) << "::My" << uuid << " ,";
  }
  out << "std::tuple<";
  Join(", ", AddrElems, [](TAddrElems::const_reference it, TCppPrinter &out) {

    if (!it.second->IsFree()) {
      switch (it.first) {
        case Stig::TAddrDir::Asc: {
          out << it.second->GetReturnType();
          break;
        }
        case Stig::TAddrDir::Desc: {
          out << "Stig::TDesc<" << it.second->GetReturnType() << ">";
          break;
        }
      }
    } else {
      switch (it.first) {
        case Stig::TAddrDir::Asc: {
          out << "Native::TFree<" << it.second->GetReturnType() << ">";
          break;
        }
        case Stig::TAddrDir::Desc: {
          out << "Native::TFree<Stig::TDesc<" << it.second->GetReturnType() << ">>";
          break;
        }
      }
    }
  }, out);
  out << ">";
  out << "(";
  Join(", ", AddrElems, [](TAddrElems::const_reference it, TCppPrinter &out) {
    //NOTE: This sometimes will cause

    if (!it.second->IsFree()) {
      switch (it.first) {
        case Stig::TAddrDir::Asc: {
          out << it.second->GetReturnType();
          break;
        }
        case Stig::TAddrDir::Desc: {
          out << "Stig::TDesc<" << it.second->GetReturnType() << ">";
          break;
        }
      }
    } else {
      switch (it.first) {
        case Stig::TAddrDir::Asc: {
          out << "Native::TFree<" << it.second->GetReturnType() << ">";
          break;
        }
        case Stig::TAddrDir::Desc: {
          out << "Native::TFree<Stig::TDesc<" << it.second->GetReturnType() << ">>";
          break;
        }
      }
    }
    out << "(" << it.second << ")";
  }, out);
  out << "))";
}