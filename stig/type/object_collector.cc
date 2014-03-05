/* <stig/type/object_collector.cc>

   Implements <stig/type/object_collector.h>

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

#include <stig/type/object_collector.h>

#include <stig/pos_range.h>
#include <stig/type/infix_visitor.h>

using namespace std;
using namespace Stig;
using namespace Stig::Type;

//TODO: Move to new families of visitors.
void Stig::Type::CollectObjects(const TType &type, unordered_set<TType> &object_set) {
  class TObjectCollectorVisitor : public TType::TVisitor {
    NO_COPY_SEMANTICS(TObjectCollectorVisitor);
    public:
    TObjectCollectorVisitor(unordered_set<TType> &object_set) : ObjectSet(object_set) {}
    virtual void operator()(const TAddr *that) const {
      for (auto elem : that->GetElems()) {
        elem.second.Accept(*this);
      }
    }
    virtual void operator()(const TAny *) const {/* DO NOTHING */}
    virtual void operator()(const TBool *) const {/* DO NOTHING */}
    virtual void operator()(const TDict *that) const {
      that->GetKey().Accept(*this);
      that->GetVal().Accept(*this);
    }
    virtual void operator()(const TErr *) const {/* DO NOTHING */}
    virtual void operator()(const TFunc *that) const {
      that->GetReturnType().Accept(*this);
      that->GetParamObject().Accept(*this);
    }
    virtual void operator()(const TId *) const {/* DO NOTHING */}
    virtual void operator()(const TInt *) const {/* DO NOTHING */}
    virtual void operator()(const TList *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TMutable *that) const {
      that->GetVal().Accept(*this);
    }
    virtual void operator()(const TObj *that) const {
      ObjectSet.insert(that->AsType());
      for (auto iter : that->GetElems()) {
        iter.second.Accept(*this);
      }
    }
    virtual void operator()(const TOpt *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TReal *) const {/* DO NOTHING */}
    virtual void operator()(const TSeq *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TSet *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TStr *) const {/* DO NOTHING */}
    virtual void operator()(const TTimeDiff *) const {/* DO NOTHING */}
    virtual void operator()(const TTimePnt *) const {/* DO NOTHING */}
    private:
    unordered_set<TType> &ObjectSet;
  };  // TObjectCollectorVisitor
  type.Accept(TObjectCollectorVisitor(object_set));
}