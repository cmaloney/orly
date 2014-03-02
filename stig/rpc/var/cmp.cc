/* <stig/rpc/var/cmp.cc> 

   Implmements <stig/rpc/var/cmp.h>.

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

#include <stig/rpc/var/cmp.h>

#include <algorithm>

#include <stig/rpc/all_vars.h>
#include <stig/rpc/type/cmp.h>
#include <stig/rpc/var/get_type.h>

using namespace std;
using namespace Stig::Rpc;

template <typename TVal>
static int CmpVals(const TVal &lhs, const TVal &rhs) {
  if (lhs < rhs) {
    return -1;
  }
  if (rhs < lhs) {
    return +1;
  }
  return 0;
}

int Cmp(const TVar &lhs, const TVar &rhs) {
  struct visitor_t final : public TVar::TDoubleVisitor {
    int &Out;
    visitor_t(int &out) : Out(out) {}
    // Addr
    virtual void operator()(const Var::TAddr     *, const Var::TBool     *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TDict     *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TId       *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TInt      *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TList     *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TObj      *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TAddr     *, const Var::TTimePnt  *) const override { Out = -1; }
    // Bool
    virtual void operator()(const Var::TBool     *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TBool     *, const Var::TDict     *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TId       *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TInt      *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TList     *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TObj      *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TBool     *, const Var::TTimePnt  *) const override { Out = -1; }
    // Dict
    virtual void operator()(const Var::TDict     *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TDict     *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TDict     *, const Var::TId       *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TInt      *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TList     *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TObj      *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TDict     *, const Var::TTimePnt  *) const override { Out = -1; }
    // Id
    virtual void operator()(const Var::TId       *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TId       *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TId       *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TId       *, const Var::TInt      *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TList     *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TObj      *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TId       *, const Var::TTimePnt  *) const override { Out = -1; }
    // Int
    virtual void operator()(const Var::TInt      *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TInt      *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TInt      *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TInt      *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TInt      *, const Var::TList     *) const override { Out = -1; }
    virtual void operator()(const Var::TInt      *, const Var::TObj      *) const override { Out = -1; }
    virtual void operator()(const Var::TInt      *, const Var::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Var::TInt      *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TInt      *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TInt      *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TInt      *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TInt      *, const Var::TTimePnt  *) const override { Out = -1; }
    // List
    virtual void operator()(const Var::TList     *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TList     *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TList     *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TList     *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TList     *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TList     *, const Var::TObj      *) const override { Out = -1; }
    virtual void operator()(const Var::TList     *, const Var::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Var::TList     *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TList     *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TList     *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TList     *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TList     *, const Var::TTimePnt  *) const override { Out = -1; }
    // Obj
    virtual void operator()(const Var::TObj      *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TObj      *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TObj      *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TObj      *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TObj      *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TObj      *, const Var::TList     *) const override { Out = +1; }
    virtual void operator()(const Var::TObj      *, const Var::TOpt      *) const override { Out = -1; }
    virtual void operator()(const Var::TObj      *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TObj      *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TObj      *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TObj      *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TObj      *, const Var::TTimePnt  *) const override { Out = -1; }
    // Opt
    virtual void operator()(const Var::TOpt      *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TOpt      *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TOpt      *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TOpt      *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TOpt      *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TOpt      *, const Var::TList     *) const override { Out = +1; }
    virtual void operator()(const Var::TOpt      *, const Var::TObj      *) const override { Out = +1; }
    virtual void operator()(const Var::TOpt      *, const Var::TReal     *) const override { Out = -1; }
    virtual void operator()(const Var::TOpt      *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TOpt      *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TOpt      *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TOpt      *, const Var::TTimePnt  *) const override { Out = -1; }
    // Real
    virtual void operator()(const Var::TReal     *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TList     *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TObj      *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Var::TReal     *, const Var::TSet      *) const override { Out = -1; }
    virtual void operator()(const Var::TReal     *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TReal     *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TReal     *, const Var::TTimePnt  *) const override { Out = -1; }
    // Set
    virtual void operator()(const Var::TSet      *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TList     *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TObj      *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TReal     *) const override { Out = +1; }
    virtual void operator()(const Var::TSet      *, const Var::TStr      *) const override { Out = -1; }
    virtual void operator()(const Var::TSet      *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TSet      *, const Var::TTimePnt  *) const override { Out = -1; }
    // Str
    virtual void operator()(const Var::TStr      *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TList     *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TObj      *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TReal     *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TSet      *) const override { Out = +1; }
    virtual void operator()(const Var::TStr      *, const Var::TTimeDiff *) const override { Out = -1; }
    virtual void operator()(const Var::TStr      *, const Var::TTimePnt  *) const override { Out = -1; }
    // TimeDiff
    virtual void operator()(const Var::TTimeDiff *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TList     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TObj      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TReal     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TSet      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TStr      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimeDiff *, const Var::TTimePnt  *) const override { Out = -1; }
    // TimePnt
    virtual void operator()(const Var::TTimePnt  *, const Var::TAddr     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TBool     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TDict     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TId       *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TInt      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TList     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TObj      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TOpt      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TReal     *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TSet      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TStr      *) const override { Out = +1; }
    virtual void operator()(const Var::TTimePnt  *, const Var::TTimeDiff *) const override { Out = +1; }
    // Comparisons with kind start here.
    virtual void operator()(const Var::TBool *lhs, const Var::TBool *rhs) const override {
      Out = CmpVals(lhs->GetVal(), rhs->GetVal());
    }
    virtual void operator()(const Var::TId *lhs, const Var::TId *rhs) const override {
      Out = CmpVals(lhs->GetVal(), rhs->GetVal());
    }
    virtual void operator()(const Var::TInt *lhs, const Var::TInt *rhs) const override {
      Out = CmpVals(lhs->GetVal(), rhs->GetVal());
    }
    virtual void operator()(const Var::TReal *lhs, const Var::TReal *rhs) const override {
      Out = CmpVals(lhs->GetVal(), rhs->GetVal());
    }
    virtual void operator()(const Var::TStr *lhs, const Var::TStr *rhs) const override {
      Out = lhs->GetVal().compare(rhs->GetVal());
    }
    virtual void operator()(const Var::TTimeDiff *lhs, const Var::TTimeDiff *rhs) const override {
      Out = CmpVals(lhs->GetVal(), rhs->GetVal());
    }
    virtual void operator()(const Var::TTimePnt *lhs, const Var::TTimePnt *rhs) const override {
      Out = CmpVals(lhs->GetVal(), rhs->GetVal());
    }
    virtual void operator()(const Var::TList *lhs, const Var::TList *rhs) const override {
      Out = Cmp(*lhs->GetElemType(), *rhs->GetElemType());
      if (!Out) {
        const auto
            &lhs_elems = lhs->GetElems(),
            &rhs_elems = rhs->GetElems();
        auto
            lhs_iter = lhs_elems.begin(),
            rhs_iter = rhs_elems.begin();
        for (; lhs_iter != lhs_elems.end() && rhs_iter != rhs_elems.end(); ++lhs_iter, ++rhs_iter) {
          Out = Cmp(**lhs_iter, **rhs_iter);
          if (Out) {
            break;
          }
        }
        if (!Out) {
          if (lhs_iter == lhs_elems.end() && rhs_iter != rhs_elems.end()) {
            Out = -1;
          } else if (lhs_iter != lhs_elems.end() && rhs_iter == rhs_elems.end()) {
            Out = +1;
          }
        }
      }
    }
    virtual void operator()(const Var::TOpt *lhs, const Var::TOpt *rhs) const override {
      Out = Cmp(*lhs->GetElemType(), *rhs->GetElemType());
      if (!Out) {
        auto
            *lhs_elem = lhs->TryGetElem(),
            *rhs_elem = rhs->TryGetElem();
        if (lhs_elem && rhs_elem) {
          Out = Cmp(*lhs_elem, *rhs_elem);
        } else if (!lhs_elem && rhs_elem) {
          Out = -1;
        } else if (!rhs_elem && lhs_elem) {
          Out = +1;
        }
      }
    }
    virtual void operator()(const Var::TSet *lhs, const Var::TSet *rhs) const override {
      Out = Cmp(*lhs->GetElemType(), *rhs->GetElemType());
      if (!Out) {
        const auto
            &lhs_elems = lhs->GetElems(),
            &rhs_elems = rhs->GetElems();
        auto
            lhs_iter = lhs_elems.begin(),
            rhs_iter = rhs_elems.begin();
        for (; lhs_iter != lhs_elems.end() && rhs_iter != rhs_elems.end(); ++lhs_iter, ++rhs_iter) {
          Out = Cmp(**lhs_iter, **rhs_iter);
          if (Out) {
            break;
          }
        }
        if (!Out) {
          if (lhs_iter == lhs_elems.end() && rhs_iter != rhs_elems.end()) {
            Out = -1;
          } else if (lhs_iter != lhs_elems.end() && rhs_iter == rhs_elems.end()) {
            Out = +1;
          }
        }
      }
    }
    virtual void operator()(const Var::TDict *lhs, const Var::TDict *rhs) const override {
      Out = Cmp(*lhs->GetKeyType(), *rhs->GetKeyType());
      if (!Out) {
        Out = Cmp(*lhs->GetValType(), *rhs->GetValType());
        if (!Out) {
          const auto
              &lhs_elems = lhs->GetElems(),
              &rhs_elems = rhs->GetElems();
          auto
              lhs_iter = lhs_elems.begin(),
              rhs_iter = rhs_elems.begin();
          for (; lhs_iter != lhs_elems.end() && rhs_iter != rhs_elems.end(); ++lhs_iter, ++rhs_iter) {
            Out = Cmp(*lhs_iter->first, *rhs_iter->first);
            if (Out) {
              break;
            }
            Out = Cmp(*lhs_iter->second, *rhs_iter->second);
            if (Out) {
              break;
            }
          }
          if (!Out) {
            if (lhs_iter == lhs_elems.end() && rhs_iter != rhs_elems.end()) {
              Out = -1;
            } else if (lhs_iter != lhs_elems.end() && rhs_iter == rhs_elems.end()) {
              Out = +1;
            }
          }
        }
      }
    }
    virtual void operator()(const Var::TAddr *lhs, const Var::TAddr *rhs) const override {
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
    virtual void operator()(const Var::TObj *lhs, const Var::TObj *rhs) const override {
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

