/* <stig/type/new_sabot.cc>

   Implements <stig/type/new_sabot.h>.

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

#include <stig/type/new_sabot.h>

#include <cassert>

using namespace std;
using namespace Stig;

Stig::Type::TTypeTranslationError::TTypeTranslationError()
    : logic_error("could not translate from stig type to sabot type") {}

Sabot::Type::TAny *Stig::Type::NewSabot(void *buf, const TType &type) {
  auto result = TryNewSabot(buf, type);
  if (!result) {
    throw TTypeTranslationError();
  }
  return result;
}

Sabot::Type::TAny *Stig::Type::TryNewSabot(void *buf, const Type::TType &type) {
  class visitor_t final : public Type::TType::TVisitor {
    public:
    visitor_t(Sabot::Type::TAny *&result, void *buf) : Result(result), Buf(buf) {}
    // no translation
    virtual void operator()(const TAny      */*type*/) const override {}
    virtual void operator()(const TErr      */*type*/) const override {}
    virtual void operator()(const TFunc     */*type*/) const override {}
    virtual void operator()(const TSeq      */*type*/) const override {}
    // nullary
    virtual void operator()(const TBool     */*type*/) const override { Result = new (Buf) Sabot::Type::TBool(); }
    virtual void operator()(const TId       */*type*/) const override { Result = new (Buf) Sabot::Type::TUuid(); }
    virtual void operator()(const TInt      */*type*/) const override { Result = new (Buf) Sabot::Type::TInt64(); }
    virtual void operator()(const TReal     */*type*/) const override { Result = new (Buf) Sabot::Type::TDouble(); }
    virtual void operator()(const TStr      */*type*/) const override { Result = new (Buf) Sabot::Type::TStr(); }
    virtual void operator()(const TTimeDiff */*type*/) const override { Result = new (Buf) Sabot::Type::TDuration(); }
    virtual void operator()(const TTimePnt  */*type*/) const override { Result = new (Buf) Sabot::Type::TTimePoint(); }
    // unary and binary
    virtual void operator()(const TMutable *type) const override {
      Result = TryNewSabot(Buf, type->GetVal());
    }
    virtual void operator()(const TList *type) const override {
      Result = new (Buf) ST::TVector(type->GetElem());
    }
    virtual void operator()(const TOpt *type) const override {
      Result = new (Buf) ST::TOpt(type->GetElem());
    }
    virtual void operator()(const TSet *type) const override {
      Result = new (Buf) ST::TSet(type->GetElem());
    }
    virtual void operator()(const TDict *type) const override {
      Result = new (Buf) ST::TMap(type->GetKey(), type->GetVal());
    }
    // n-ary
    virtual void operator()(const TAddr *type) const override {
      Result = new (Buf) ST::TTuple(type);
    }
    virtual void operator()(const TObj *type) const override {
      Result = new (Buf) ST::TRecord(type);
    }
    private:
    Sabot::Type::TAny *&Result;
    void *Buf;
  };
  assert(buf);
  assert(&type);
  Sabot::Type::TAny *result = nullptr;
  type.Accept(visitor_t(result, buf));
  return result;
}
