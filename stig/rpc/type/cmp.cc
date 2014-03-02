/* <stig/rpc/type/cmp.cc> 

   Implmements <stig/rpc/type/cmp.h>.

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

#include <stig/rpc/type/cmp.h>

#include <algorithm>

#include <stig/rpc/all_types.h>

using namespace std;
using namespace Stig::Rpc;

int Cmp(const TType &lhs, const TType &rhs) {
  struct visitor_t final : public TType::TDoubleVisitor {
    int &Out;
    visitor_t(int &out) : Out(out) {}
    // Addr
    virtual void operator()(const Type::TAddr     *, const Type::TBool     *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TDict     *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TId       *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TInt      *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TList     *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TObj      *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TAddr     *, const Type::TTimePnt  *) const override { Out = -1; }
    // Bool
    virtual void operator()(const Type::TBool     *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TBool     *, const Type::TDict     *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TId       *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TInt      *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TList     *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TObj      *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TBool     *, const Type::TTimePnt  *) const override { Out = -1; }
    // Dict
    virtual void operator()(const Type::TDict     *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TDict     *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TDict     *, const Type::TId       *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TInt      *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TList     *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TObj      *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TDict     *, const Type::TTimePnt  *) const override { Out = -1; }
    // Id
    virtual void operator()(const Type::TId       *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TId       *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TId       *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TId       *, const Type::TInt      *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TList     *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TObj      *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TId       *, const Type::TTimePnt  *) const override { Out = -1; }
    // Int
    virtual void operator()(const Type::TInt      *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TInt      *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TInt      *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TInt      *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TInt      *, const Type::TList     *) const override { Out = -1; }
    virtual void operator()(const Type::TInt      *, const Type::TObj      *) const override { Out = -1; }
    virtual void operator()(const Type::TInt      *, const Type::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Type::TInt      *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TInt      *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TInt      *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TInt      *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TInt      *, const Type::TTimePnt  *) const override { Out = -1; }
    // List
    virtual void operator()(const Type::TList     *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TList     *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TList     *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TList     *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TList     *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TList     *, const Type::TObj      *) const override { Out = -1; }
    virtual void operator()(const Type::TList     *, const Type::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Type::TList     *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TList     *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TList     *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TList     *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TList     *, const Type::TTimePnt  *) const override { Out = -1; }
    // Obj
    virtual void operator()(const Type::TObj      *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TObj      *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TObj      *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TObj      *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TObj      *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TObj      *, const Type::TList     *) const override { Out = +1; }
    virtual void operator()(const Type::TObj      *, const Type::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Type::TObj      *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TObj      *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TObj      *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TObj      *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TObj      *, const Type::TTimePnt  *) const override { Out = -1; }
    // Opt
    virtual void operator()(const Type::TOpt      *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TOpt      *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TOpt      *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TOpt      *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TOpt      *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TOpt      *, const Type::TList     *) const override { Out = +1; }
    virtual void operator()(const Type::TOpt      *, const Type::TObj      *) const override { Out = +1; }
    virtual void operator()(const Type::TOpt      *, const Type::TReal     *) const override { Out = -1; }
    virtual void operator()(const Type::TOpt      *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TOpt      *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TOpt      *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TOpt      *, const Type::TTimePnt  *) const override { Out = -1; }
    // Real
    virtual void operator()(const Type::TReal     *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TList     *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TObj      *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Type::TReal     *, const Type::TSet      *) const override { Out = -1; }
    virtual void operator()(const Type::TReal     *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TReal     *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TReal     *, const Type::TTimePnt  *) const override { Out = -1; }
    // Set
    virtual void operator()(const Type::TSet      *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TList     *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TObj      *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TReal     *) const override { Out = +1; }
    virtual void operator()(const Type::TSet      *, const Type::TStr      *) const override { Out = -1; }
    virtual void operator()(const Type::TSet      *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TSet      *, const Type::TTimePnt  *) const override { Out = -1; }
    // Str
    virtual void operator()(const Type::TStr      *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TList     *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TObj      *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TReal     *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TSet      *) const override { Out = +1; }
    virtual void operator()(const Type::TStr      *, const Type::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Type::TStr      *, const Type::TTimePnt  *) const override { Out = -1; }
    // TimeDiff
    virtual void operator()(const Type::TTimeDiff *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TList     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TObj      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TReal     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TSet      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TStr      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TTimePnt  *) const override { Out = -1; }
    // TimePnt
    virtual void operator()(const Type::TTimePnt  *, const Type::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TBool     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TDict     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TId       *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TInt      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TList     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TObj      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TReal     *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TSet      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TStr      *) const override { Out = +1; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TTimeDiff *) const override { Out = +1; }
    // Monomorphs of the same kind are always equal.
    virtual void operator()(const Type::TBool     *, const Type::TBool     *) const override { Out = 0; }
    virtual void operator()(const Type::TId       *, const Type::TId       *) const override { Out = 0; }
    virtual void operator()(const Type::TInt      *, const Type::TInt      *) const override { Out = 0; }
    virtual void operator()(const Type::TReal     *, const Type::TReal     *) const override { Out = 0; }
    virtual void operator()(const Type::TStr      *, const Type::TStr      *) const override { Out = 0; }
    virtual void operator()(const Type::TTimeDiff *, const Type::TTimeDiff *) const override { Out = 0; }
    virtual void operator()(const Type::TTimePnt  *, const Type::TTimePnt  *) const override { Out = 0; }
    // Poly1 types of the same kind compare based on their element type.
    virtual void operator()(const Type::TList *lhs, const Type::TList *rhs) const override { Out = Cmp(*lhs->GetElem(), *rhs->GetElem()); }
    virtual void operator()(const Type::TOpt  *lhs, const Type::TOpt  *rhs) const override { Out = Cmp(*lhs->GetElem(), *rhs->GetElem()); }
    virtual void operator()(const Type::TSet  *lhs, const Type::TSet  *rhs) const override { Out = Cmp(*lhs->GetElem(), *rhs->GetElem()); }
    // Poly2 types of the same kind compare based on their key type, then their val type.
    virtual void operator()(const Type::TDict *lhs, const Type::TDict *rhs) const override {
      Out = Cmp(*lhs->GetKey(), *rhs->GetKey());
      if (!Out) {
        Out = Cmp(*lhs->GetVal(), *rhs->GetVal());
      }
    }
    // Compare addrs field by field.  Fields compare first by type, then by sorting direction.
    virtual void operator()(const Type::TAddr *lhs, const Type::TAddr *rhs) const override {
      const auto
          &lhs_fields = lhs->GetFields(),
          &rhs_fields = rhs->GetFields();
      auto
          lhs_iter = lhs_fields.begin(),
          rhs_iter = rhs_fields.begin();
      Out = 0;
      for (; lhs_iter != lhs_fields.end() && rhs_iter != rhs_fields.end(); ++lhs_iter, ++rhs_iter) {
        Out = Cmp(*lhs_iter->second, *rhs_iter->second);
        if (Out) {
          break;
        }
        Out = CmpDirs(lhs_iter->first, rhs_iter->first);
        if (Out) {
          break;
        }
      }
      if (!Out) {
        if (lhs_iter == lhs_fields.end() && rhs_iter != rhs_fields.end()) {
          Out = -1;
        } else if (lhs_iter != lhs_fields.end() && rhs_iter == rhs_fields.end()) {
          Out = +1;
        }
      }
    }
    // Compare objs field by field.  Fields compare first by name, then by type.
    virtual void operator()(const Type::TObj *lhs, const Type::TObj *rhs) const override {
      const auto
          &lhs_fields = lhs->GetFields(),
          &rhs_fields = rhs->GetFields();
      auto
          lhs_iter = lhs_fields.begin(),
          rhs_iter = rhs_fields.begin();
      Out = 0;
      for (; lhs_iter != lhs_fields.end() && rhs_iter != rhs_fields.end(); ++lhs_iter, ++rhs_iter) {
        Out = lhs_iter->first.compare(rhs_iter->first);
        if (Out) {
          break;
        }
        Out = Cmp(*lhs_iter->second, *rhs_iter->second);
        if (Out) {
          break;
        }
      }
      if (!Out) {
        if (lhs_iter == lhs_fields.end() && rhs_iter != rhs_fields.end()) {
          Out = -1;
        } else if (lhs_iter != lhs_fields.end() && rhs_iter == rhs_fields.end()) {
          Out = +1;
        }
      }
    }
  };
  assert(&lhs);
  assert(&rhs);
  int out;
  visitor_t(out).Visit(&lhs, &rhs);
  return out;
}

