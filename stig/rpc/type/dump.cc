/* <stig/rpc/type/dump.cc> 

   Implements <stig/rpc/type/dump.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/type/dump.h>

#include <cassert>
#include <sstream>

#include <stig/rpc/all_types.h>

using namespace std;
using namespace Stig::Rpc;

ostream &Stig::Rpc::operator<<(ostream &strm, const TType &type) {
  struct visitor_t final : public TType::TVisitor {
    ostream &Strm;
    visitor_t(ostream &strm) : Strm(strm) {}
    // Mono.
    virtual void operator()(const Type::TBool     *) const override { OnMono("bool"     ); }
    virtual void operator()(const Type::TId       *) const override { OnMono("id"       ); }
    virtual void operator()(const Type::TInt      *) const override { OnMono("int"      ); }
    virtual void operator()(const Type::TReal     *) const override { OnMono("real"     ); }
    virtual void operator()(const Type::TStr      *) const override { OnMono("str"      ); }
    virtual void operator()(const Type::TTimeDiff *) const override { OnMono("time_diff"); }
    virtual void operator()(const Type::TTimePnt  *) const override { OnMono("time_pnt" ); }
    // Poly1.
    virtual void operator()(const Type::TList *type) const override { OnPoly1("list", type->GetElem()); }
    virtual void operator()(const Type::TOpt  *type) const override { OnPoly1("opt",  type->GetElem()); }
    virtual void operator()(const Type::TSet  *type) const override { OnPoly1("set",  type->GetElem()); }
    // Poly2.
    virtual void operator()(const Type::TDict *type) const override { OnPoly2("dict", type->GetKey(), type->GetVal()); }
    // Addr.
    virtual void operator()(const Type::TAddr *type) const override {
      Strm << "addr(";
      bool sep = false;
      for (const auto &field: type->GetFields()) {
        if (sep) {
          Strm << ", ";
        } else {
          sep = true;
        }
        Strm << GetDirName(field.first) << ' ';
        field.second->Accept(*this);
      }
      Strm << ')';
    }
    // Obj.
    virtual void operator()(const Type::TObj *type) const override {
      Strm << "obj(";
      bool sep = false;
      for (const auto &field: type->GetFields()) {
        if (sep) {
          Strm << ", ";
        } else {
          sep = true;
        }
        Strm << field.first << ": ";
        field.second->Accept(*this);
      }
      Strm << ')';
    }
    void OnMono(const char *name) const {
      Strm << name;
    }
    void OnPoly1(const char *name, const TType *elem) const {
      Strm << name << '(';
      elem->Accept(*this);
      Strm << ')';
    }
    void OnPoly2(const char *name, const TType *key, const TType *val) const {
      Strm << name << '(';
      key->Accept(*this);
      Strm << ", ";
      val->Accept(*this);
      Strm << ')';
    }
  };
  assert(&strm);
  assert(&type);
  type.Accept(visitor_t(strm));
  return strm;
}

string ToString(const TType &type) {
  ostringstream strm;
  strm << type;
  return strm.str();
}

