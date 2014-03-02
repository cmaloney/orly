/* <stig/rpc/type/copy.cc> 

   Implements <stig/rpc/type/copy.h>.

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

#include <stig/rpc/type/copy.h>

#include <cassert>

#include <stig/rpc/all_types.h>

using namespace std;
using namespace Stig::Rpc;

TType::TPtr Copy(const TType &type) {
  struct visitor_t final : public TType::TVisitor {
    TType *&Out;
    visitor_t(TType *&out) : Out(out) {}
    // Mono.
    virtual void operator()(const Type::TBool     *) const override { Out = new Type::TBool    ; }
    virtual void operator()(const Type::TId       *) const override { Out = new Type::TId      ; }
    virtual void operator()(const Type::TInt      *) const override { Out = new Type::TInt     ; }
    virtual void operator()(const Type::TReal     *) const override { Out = new Type::TReal    ; }
    virtual void operator()(const Type::TStr      *) const override { Out = new Type::TStr     ; }
    virtual void operator()(const Type::TTimeDiff *) const override { Out = new Type::TTimeDiff; }
    virtual void operator()(const Type::TTimePnt  *) const override { Out = new Type::TTimePnt ; }
    // Poly1.
    virtual void operator()(const Type::TList *type) const override { Out = new Type::TList(Copy(*type->GetElem())); }
    virtual void operator()(const Type::TOpt  *type) const override { Out = new Type::TOpt (Copy(*type->GetElem())); }
    virtual void operator()(const Type::TSet  *type) const override { Out = new Type::TSet (Copy(*type->GetElem())); }
    // Poly2.
    virtual void operator()(const Type::TDict *type) const override { Out = new Type::TDict(Copy(*type->GetKey()), Copy(*type->GetVal())); }
    // Addr.
    virtual void operator()(const Type::TAddr *type) const override {
      unique_ptr<Type::TAddr> copy(new Type::TAddr());
      for (const auto &field: type->GetFields()) {
        copy->AddField(field.first, Copy(*field.second));
      }
      Out = copy.release();
    }
    // Obj.
    virtual void operator()(const Type::TObj *type) const override {
      unique_ptr<Type::TObj> copy(new Type::TObj());
      for (const auto &field: type->GetFields()) {
        copy->AddField(string(field.first), Copy(*field.second));
      }
      Out = copy.release();
    }
  };
  assert(&type);
  TType *copy;
  type.Accept(visitor_t(copy));
  assert(copy);
  return TType::TPtr(copy);
}

