/* <stig/type/unwrap.cc>

   Implements <stig/type/unwrap.h>.

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

#include <stig/type/unwrap.h>

#include <stig/type.h>

using namespace Stig;
using namespace Stig::Type;

class TUnwrapHelperVisitor
    : public TType::TVisitor {
  NO_COPY_SEMANTICS(TUnwrapHelperVisitor);
  protected:

  TUnwrapHelperVisitor(TType &type)
      : Type(type) {}

  virtual void operator()(const TMutable  *that) const = 0;
  virtual void operator()(const TOpt      *that) const = 0;
  virtual void operator()(const TSeq      *that) const = 0;

  TType &Type;

  private:

  virtual void operator()(const TAddr     *that) const { Type = that->AsType(); }
  virtual void operator()(const TAny      *that) const { Type = that->AsType(); }
  virtual void operator()(const TBool     *that) const { Type = that->AsType(); }
  virtual void operator()(const TDict     *that) const { Type = that->AsType(); }
  virtual void operator()(const TErr      *that) const { Type = that->AsType(); }
  virtual void operator()(const TFunc     *that) const {
    // EnsureEmptyObject(that->GetParamObject(), PosRange);
    that->GetReturnType().Accept(*this);
  }
  virtual void operator()(const TId       *that) const { Type = that->AsType(); }
  virtual void operator()(const TInt      *that) const { Type = that->AsType(); }
  virtual void operator()(const TList     *that) const { Type = that->AsType(); }
  virtual void operator()(const TObj      *that) const { Type = that->AsType(); }
  virtual void operator()(const TReal     *that) const { Type = that->AsType(); }
  virtual void operator()(const TSet      *that) const { Type = that->AsType(); }
  virtual void operator()(const TStr      *that) const { Type = that->AsType(); }
  virtual void operator()(const TTimeDiff *that) const { Type = that->AsType(); }
  virtual void operator()(const TTimePnt  *that) const { Type = that->AsType(); }

};  // TUnwrapHelperVisitor

TType Stig::Type::Unwrap(const TType &type) {
  return UnwrapMutable(UnwrapSequence(type));
}

TType Stig::Type::UnwrapMutable(const TType &type) {
  class TUnwrapMutableVisitor
      : public TUnwrapHelperVisitor {
    NO_COPY_SEMANTICS(TUnwrapMutableVisitor);
    public:
    TUnwrapMutableVisitor(TType &type)
        : TUnwrapHelperVisitor(type) {}
    virtual void operator()(const TMutable  *that) const {
      that->GetVal().Accept(*this);
    }
    virtual void operator()(const TOpt      *that) const { Type = that->AsType(); }
    virtual void operator()(const TSeq      *that) const { Type = that->AsType(); }
  };  // TUnwrapMutableVisitor
  TType ret;
  type.Accept(TUnwrapMutableVisitor(ret));
  return ret;
}

TType Stig::Type::UnwrapOptional(const TType &type) {
  class TUnwrapOptionalVisitor
      : public TUnwrapHelperVisitor {
    NO_COPY_SEMANTICS(TUnwrapOptionalVisitor);
    public:
    TUnwrapOptionalVisitor(TType &type)
        : TUnwrapHelperVisitor(type) {}
    virtual void operator()(const TMutable  *that) const { Type = that->AsType(); }
    virtual void operator()(const TOpt      *that) const {
      that->GetElem().Accept(*this);
    }
    virtual void operator()(const TSeq      *that) const { Type = that->AsType(); }
  };  // TUnwrapOptionalVisitor
  TType ret;
  type.Accept(TUnwrapOptionalVisitor(ret));
  return ret;
}

TType Stig::Type::UnwrapSequence(const TType &type) {
  class TUnwrapSequenceVisitor
      : public TUnwrapHelperVisitor {
    NO_COPY_SEMANTICS(TUnwrapSequenceVisitor);
    public:
    TUnwrapSequenceVisitor(TType &type)
      : TUnwrapHelperVisitor(type) {}
    virtual void operator()(const TMutable  *that) const { Type = that->AsType(); }
    virtual void operator()(const TOpt      *that) const { Type = that->AsType(); }
    virtual void operator()(const TSeq      *that) const {
      that->GetElem().Accept(*this);
    }
  };  // TUnwrapSequenceVisitor
  TType ret;
  type.Accept(TUnwrapSequenceVisitor(ret));
  return ret;
}
