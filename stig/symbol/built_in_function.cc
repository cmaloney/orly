/* <stig/symbol/built_in_function.cc>

   Implements <stig/symbol/built_in_function.h>

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

#include <stig/symbol/built_in_function.h>

#include <base/assert_true.h>
#include <stig/symbol/result_def.h>
#include <stig/symbol/scope.h>
#include <stig/type/bool.h>
#include <stig/type/int.h>
#include <stig/type/time_diff.h>
#include <stig/type/time_pnt.h>

using namespace Stig;
using namespace Stig::Symbol;

// NOTE: For built in functions, we shouldn't have a pos range for result defs.
//       For now there is an invalid value to satisfy the current restrictions,

const Base::TSafeGlobal<TBuiltInFunction::TPtr> TBuiltInFunction::TimeDiff(
  []() -> TBuiltInFunction::TPtr * {
    auto time_diff = TBuiltInFunction::New(
        "::Base::Chrono::CreateTimeDiff",
        {"is_forward", "day", "hour", "minute", "second", "nanosecond"},
        {{"is_forward"     , Type::TBool::Get()},
         {"day"            , Type::TInt::Get()},
         {"hour"           , Type::TInt::Get()},
         {"minute"         , Type::TInt::Get()},
         {"second"         , Type::TInt::Get()},
         {"nanosecond"    , Type::TInt::Get()}},
        Type::TTimeDiff::Get());
    TResultDef::New(time_diff, "time_diff", TPosRange()); // TODO: Get rid of pos range
    return new TBuiltInFunction::TPtr(time_diff);
  });

const Base::TSafeGlobal<TBuiltInFunction::TPtr> TBuiltInFunction::TimePnt(
  []() -> TBuiltInFunction::TPtr * {
    auto time_pnt = TBuiltInFunction::New(
        "::Base::Chrono::CreateTimePnt",
        {"year", "month", "day", "hour", "minute", "second", "nanosecond", "utc_offset"},
        {{"year"      , Type::TInt::Get()},
         {"month"     , Type::TInt::Get()},
         {"day"       , Type::TInt::Get()},
         {"hour"      , Type::TInt::Get()},
         {"minute"    , Type::TInt::Get()},
         {"second"    , Type::TInt::Get()},
         {"nanosecond"    , Type::TInt::Get()},
         {"utc_offset", Type::TInt::Get()}},
        Type::TTimePnt::Get());
    TResultDef::New(time_pnt, "time_pnt", TPosRange()); // TODO: Get rid of pos range
    return new TBuiltInFunction::TPtr(time_pnt);
  });

const Base::TSafeGlobal<TBuiltInFunction::TPtr> TBuiltInFunction::RandomInt(
  []() -> TBuiltInFunction::TPtr * {
    auto random_int = TBuiltInFunction::New(
      "ctx.GetRandomInt",
      {"gen_id", "min", "max", "idx"},
      {{"gen_id", Type::TInt::Get()},
       {"min", Type::TInt::Get()},
       {"max", Type::TInt::Get()},
       {"idx", Type::TInt::Get()}},
       Type::TInt::Get());
    TResultDef::New(random_int, "random_int", TPosRange()); // TODO: Get rid of pos range
    return new TBuiltInFunction::TPtr(random_int);
  });

const Base::TSafeGlobal<TBuiltInFunction::TPtr> TBuiltInFunction::Replace(
  []() -> TBuiltInFunction::TPtr * {
    auto str_replace = TBuiltInFunction::New(
	"Rt::TStrReplace::Replace",
        {"oldstr", "regex", "newstr"},
        {{"oldstr"    , Type::TStr::Get()},
         {"regex"     , Type::TStr::Get()},
	 {"newstr"    , Type::TStr::Get()}},
        Type::TStr::Get());
    TResultDef::New(str_replace, "str_replace", TPosRange()); // TODO: Get rid of pos range
    return new TBuiltInFunction::TPtr(str_replace);
  });

TBuiltInFunction::TPtr TBuiltInFunction::New(
    const std::string &name,
    const TOrderedParams &ordered_params,
    const Type::TObj::TElems &params,
    const Type::TType &return_type) {
  return TBuiltInFunction::TPtr(new TBuiltInFunction(name, ordered_params, params, return_type));
}

TBuiltInFunction::TBuiltInFunction(
    const std::string &name,
    const TOrderedParams &ordered_params,
    const Type::TObj::TElems &params,
    const Type::TType &return_type)
      : TAnyFunction(name),
        OrderedParams(ordered_params),
        Params(params),
        ReturnType(return_type) {}

void TBuiltInFunction::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

const TBuiltInFunction::TOrderedParams &TBuiltInFunction::GetOrderedParams() const {
  assert(this);
  return OrderedParams;
}

Type::TObj::TElems TBuiltInFunction::GetParams() const {
  assert(this);
  return Params;
}

std::shared_ptr<const TBuiltInFunction> TBuiltInFunction::GetPtr() const {
  assert(this);
  return shared_from_this();
}

Type::TType TBuiltInFunction::GetReturnType() const {
  assert(this);
  return ReturnType;
}
