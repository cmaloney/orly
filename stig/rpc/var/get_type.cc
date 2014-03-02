/* <stig/rpc/var/get_type.cc> 

   Implements <stig/rpc/var/get_type.h>.

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

#include <stig/rpc/var/get_type.h>

#include <cassert>

#include <stig/rpc/all_types.h>
#include <stig/rpc/all_vars.h>
#include <stig/rpc/type/copy.h>

using namespace std;
using namespace Stig::Rpc;

TType::TPtr GetType(const TVar &var) {
  struct visitor_t final : public TVar::TVisitor {
    TType *&Out;
    visitor_t(TType *&out) : Out(out) {}
    // Mono.
    virtual void operator()(const Var::TBool     *) const override { Out = new Type::TBool    ; }
    virtual void operator()(const Var::TId       *) const override { Out = new Type::TId      ; }
    virtual void operator()(const Var::TInt      *) const override { Out = new Type::TInt     ; }
    virtual void operator()(const Var::TReal     *) const override { Out = new Type::TReal    ; }
    virtual void operator()(const Var::TStr      *) const override { Out = new Type::TStr     ; }
    virtual void operator()(const Var::TTimeDiff *) const override { Out = new Type::TTimeDiff; }
    virtual void operator()(const Var::TTimePnt  *) const override { Out = new Type::TTimePnt ; }
    // Poly1
    virtual void operator()(const Var::TList *var) const override { Out = new Type::TList(Copy(*var->GetElemType())); }
    virtual void operator()(const Var::TOpt  *var) const override { Out = new Type::TOpt (Copy(*var->GetElemType())); }
    virtual void operator()(const Var::TSet  *var) const override { Out = new Type::TSet (Copy(*var->GetElemType())); }
    // Poly2
    virtual void operator()(const Var::TDict *var) const override { Out = new Type::TDict(Copy(*var->GetKeyType()), Copy(*var->GetValType())); }
    // Addr
    virtual void operator()(const Var::TAddr *var) const override {
      unique_ptr<Type::TAddr> type(new Type::TAddr());
      for (const auto &field: var->GetFields()) {
        type->AddField(field.first, GetType(*field.second));
      }
      Out = type.release();
    }
    // Obj
    virtual void operator()(const Var::TObj *var) const override {
      unique_ptr<Type::TObj> type(new Type::TObj());
      for (const auto &field: var->GetFields()) {
        type->AddField(string(field.first), GetType(*field.second));
      }
      Out = type.release();
    }
  };
  assert(&var);
  TType *type = nullptr;
  var.Accept(visitor_t(type));
  assert(type);
  return TType::TPtr(type);
}

