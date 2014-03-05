/* <stig/var/new_sabot.cc>

   Implements <stig/var/new_sabot.h>.

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

#include <stig/var/new_sabot.h>

#include <cassert>

#include <stig/var/util.h>

using namespace std;
using namespace Stig;

Var::TStateTranslationError::TStateTranslationError()
    : logic_error("could not translate from stig var to sabot state") {}

Sabot::State::TAny *Var::NewSabot(void *buf, const TVar &var, TAddrDir addr_dir) {
  auto result = TryNewSabot(buf, var, addr_dir);
  if (!result) {
    throw TStateTranslationError();
  }
  return result;
}

Sabot::State::TAny *Var::TryNewSabot(void *buf, const TVar &var, TAddrDir addr_dir) {
  class visitor_t final : public Var::TVar::TVisitor {
    public:
    visitor_t(Sabot::State::TAny *&result, void *buf) : Result(result), Buf(buf) {}
    // no translation
    virtual void operator()(const Var::TErr     */*var*/) const override {}
    virtual void operator()(const Var::TFree    */*var*/) const override {}
    virtual void operator()(const Var::TUnknown */*var*/) const override {}
    // scalars
    virtual void operator()(const Var::TBool     *var) const override { Result = new (Buf) SS::TBool    (var); }
    virtual void operator()(const Var::TId       *var) const override { Result = new (Buf) SS::TId      (var); }
    virtual void operator()(const Var::TInt      *var) const override { Result = new (Buf) SS::TInt     (var); }
    virtual void operator()(const Var::TReal     *var) const override { Result = new (Buf) SS::TReal    (var); }
    virtual void operator()(const Var::TTimeDiff *var) const override { Result = new (Buf) SS::TTimeDiff(var); }
    virtual void operator()(const Var::TTimePnt  *var) const override { Result = new (Buf) SS::TTimePnt (var); }
    // arrays of scalars
    virtual void operator()(const Var::TStr *var) const override { Result = new (Buf) SS::TStr (var); }
    // arrays of single states
    virtual void operator()(const Var::TList *var) const override { Result = new (Buf) SS::TList(var); }
    virtual void operator()(const Var::TSet  *var) const override { Result = new (Buf) SS::TSet (var); }
    virtual void operator()(const Var::TOpt  *var) const override { Result = new (Buf) SS::TOpt (var); }
    // arrays of pairs of states
    virtual void operator()(const Var::TDict *var) const override { Result = new (Buf) SS::TDict(var); }
    // n-ary
    virtual void operator()(const Var::TAddr *var) const override { Result = new (Buf) SS::TAddr(var); }
    virtual void operator()(const Var::TObj  *var) const override { Result = new (Buf) SS::TObj (var); }
    // pass thru
    virtual void operator()(const Var::TMutable  *var) const override { Result = TryNewSabot(Buf, var->GetVal()); }
    private:
    Sabot::State::TAny *&Result;
    void *Buf;
  };
  assert(buf);
  assert(&var);
  Sabot::State::TAny *result = nullptr;
  if (var.Is<TFree>()) {
    result = new (buf) SS::TFree(var.As<TFree>(), addr_dir);
  } else if (addr_dir == TAddrDir::Desc) {
    result = new (buf) SS::TDesc(var);
  } else {
    var.Accept(visitor_t(result, buf));
  }
  return result;
}
