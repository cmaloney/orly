/* <orly/code_gen/keys.cc>

   Implements <orly/code_gen/keys.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/code_gen/keys.h>

#include <base/split.h>
#include <orly/type/seq.h>
#include <orly/type/unwrap.h>

using namespace Base;
using namespace Orly;
using namespace Orly::CodeGen;

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
    out << Package->GetName() << "::My" << uuid << " ,";
  }
  out
    << "std::tuple<"
    << Join(AddrElems,
            ", ",
            [](TCppPrinter &out, TAddrElems::const_reference it) {
              if (!it.second->IsFree()) {
                switch (it.first) {
                  case Orly::TAddrDir::Asc: {
                    out << it.second->GetReturnType();
                    break;
                  }
                  case Orly::TAddrDir::Desc: {
                    out << "Orly::TDesc<" << it.second->GetReturnType() << ">";
                    break;
                  }
                }
              } else {
                switch (it.first) {
                  case Orly::TAddrDir::Asc: {
                    out << "Native::TFree<" << it.second->GetReturnType() << ">";
                    break;
                  }
                  case Orly::TAddrDir::Desc: {
                    out << "Native::TFree<Orly::TDesc<" << it.second->GetReturnType() << ">>";
                    break;
                  }
                }
              }
            })
    << ">("
    << Join(AddrElems,
            ", ",
            [](TCppPrinter &out, TAddrElems::const_reference it) {
              //NOTE: This sometimes will cause
              if (!it.second->IsFree()) {
                switch (it.first) {
                  case Orly::TAddrDir::Asc: {
                    out << it.second->GetReturnType();
                    break;
                  }
                  case Orly::TAddrDir::Desc: {
                    out << "Orly::TDesc<" << it.second->GetReturnType() << ">";
                    break;
                  }
                }
              } else {
                switch (it.first) {
                  case Orly::TAddrDir::Asc: {
                    out << "Native::TFree<" << it.second->GetReturnType() << ">";
                    break;
                  }
                  case Orly::TAddrDir::Desc: {
                    out << "Native::TFree<Orly::TDesc<" << it.second->GetReturnType() << ">>";
                    break;
                  }
                }
              }
              out << "(" << it.second << ")";
            })
    << "))";
}
