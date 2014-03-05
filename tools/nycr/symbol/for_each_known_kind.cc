/* <tools/nycr/symbol/for_each_known_kind.cc>

   Implements <tools/nycr/symbol/for_each_known_kind.h>.

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

#include <tools/nycr/symbol/for_each_known_kind.h>

#include <cassert>
#include <queue>
#include <unordered_set>

#include <tools/nycr/symbol/base.h>
#include <tools/nycr/symbol/keyword.h>
#include <tools/nycr/symbol/language.h>
#include <tools/nycr/symbol/operator.h>
#include <tools/nycr/symbol/rule.h>

using namespace std;
using namespace Tools::Nycr::Symbol;

static void Visit(
    const TKind *kind, const function<void (const TKind *)> &cb,
    unordered_set<const TKind *> &done);

void Tools::Nycr::Symbol::ForEachKnownKind(
    const TKind *kind, const function<void (const TKind *)> &cb) {
  assert(kind);
  assert(&cb);
  unordered_set<const TKind *> done;
  Visit(kind, cb, done);
}

static void Visit(
    const TKind *kind, const function<void (const TKind *)> &cb,
    unordered_set<const TKind *> &done) {
  assert(kind);
  assert(&cb);
  assert(&done);
  if (done.insert(kind).second) {
    class TVisitor : public TKind::TVisitor {
      public:
      TVisitor(
          const function<void (const TKind *)> &cb,
          unordered_set<const TKind *> &done)
          : Cb(cb), Done(done) {}
      virtual void operator()(const TBase *that) const {
        OnKind(that);
        const TBase::TSubKinds &sub_kinds = that->GetSubKinds();
        for (auto iter = sub_kinds.begin(); iter != sub_kinds.end(); ++iter) {
          Visit(*iter, Cb, Done);
        }
      }
      virtual void operator()(const TLanguage *that) const {
        OnKind(that);
        OnCompound(that);
      }
      virtual void operator()(const TOperator *that) const {
        OnKind(that);
      }
      virtual void operator()(const TKeyword *that) const {
        OnKind(that);
      }
      virtual void operator()(const TRule *that) const {
        OnKind(that);
        OnCompound(that);
      }
      private:
      void OnCompound(const TCompound *that) const {
        const TCompound::TMembersInOrder &members_in_order = that->GetMembersInOrder();
        for (auto iter = members_in_order.begin();
             iter != members_in_order.end(); ++iter) {
          const TKind *kind = (*iter)->TryGetKind();
          if (kind) {
            Visit(kind, Cb, Done);
          }
        }
        TOperator *oper = that->TryGetOperator();
        if (oper) {
          Visit(oper, Cb, Done);
        }
      }
      void OnKind(const TKind *that) const {
        TBase *base = that->GetBase();
        if (base) {
          Visit(base, Cb, Done);
        }
        Cb(that);
      }
      const function<void (const TKind *)> &Cb;
      unordered_set<const TKind *> &Done;
    };
    kind->Accept(TVisitor(cb, done));
  }
}
