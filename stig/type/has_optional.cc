/* <stig/type/has_optional.cc>

   Implements <stig/type/has_optional.h>.

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

#include <stig/type/has_optional.h>

#include <stig/type.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::Type;

bool Stig::Type::HasOptional(const TType &type) {
  class THasOptionalVisitor
      : public TType::TVisitor {
    NO_COPY_SEMANTICS(THasOptionalVisitor);
    public:
    THasOptionalVisitor(bool &has_optional)
        : HasOptional(has_optional) {}
    virtual void operator()(const TAddr  *that) const {
      for (auto elem : that->GetElems()) {
        elem.second.Accept(*this);
      }
    }
    virtual void operator()(const TAny      *) const { /* DO NOTHING */ }
    virtual void operator()(const TBool     *) const { /* DO NOTHING */ }
    virtual void operator()(const TDict     *that) const {
      that->GetKey().Accept(*this);
      that->GetVal().Accept(*this);
    }
    virtual void operator()(const TErr      *) const { /* DO NOTHING */ }
    virtual void operator()(const TFunc     *that) const {
      // EnsureEmptyObject(that->GetParamObject(), PosRange);
      that->GetReturnType().Accept(*this);
    }
    virtual void operator()(const TId       *) const { /* DO NOTHING */ }
    virtual void operator()(const TInt      *) const { /* DO NOTHING */ }
    virtual void operator()(const TList     *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TMutable  *that) const {
      that->GetAddr().Accept(*this);
      that->GetVal().Accept(*this);
    }
    virtual void operator()(const TObj      *that) const {
      for (auto iter : that->GetElems()) {
        iter.second.Accept(*this);
      }
    }
    virtual void operator()(const TOpt      *) const {
      HasOptional = true;
    }
    virtual void operator()(const TReal     *) const { /* DO NOTHING */ }
    virtual void operator()(const TSeq      *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TSet      *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TStr      *) const { /* DO NOTHING */ }
    virtual void operator()(const TTimeDiff *) const { /* DO NOTHING */ }
    virtual void operator()(const TTimePnt  *) const { /* DO NOTHING */ }
    private:
    bool &HasOptional;
  };  // THasOptionalVisitor
  bool has_optional = false;
  if (type.Is<TOpt>()) {
    UnwrapOptional(type).Accept(THasOptionalVisitor(has_optional));
  } else {
    type.Accept(THasOptionalVisitor(has_optional));
  }
  return has_optional;
}