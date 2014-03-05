/* <stig/code_gen/implicit_func.cc>

   Implements <stig/code_gen/implicit_func.h>

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

#include <stig/code_gen/implicit_func.h>

#include <base/impossible_error.h>
#include <stig/code_gen/context.h>
#include <stig/type/func.h>
#include <stig/type/obj.h>
#include <stig/type/util.h>

using namespace Stig;
using namespace Stig::CodeGen;

TImplicitFunc::TPtr TImplicitFunc::New(const L0::TPackage *package, TCause cause, const Type::TType &ret_type, const TNamedArgs &args,
      const Expr::TExpr::TPtr &expr, bool keep_mutable) {

  TPtr res(new TImplicitFunc(package, Context::GetNearestFunc()->GetCodeScope()->GetIdScope(), cause, ret_type, args, expr,
      keep_mutable));
  Context::GetNearestFunc()->AddChild(res);
  return res;
}

std::string TImplicitFunc::GetName() const {
  assert(this);

  switch(Cause) {
    case TCause::Collect:
      return "collect";
    case TCause::Filter:
      return "filter";
    case TCause::Map:
      return "implicit map";
    case TCause::Reduce:
      return "reduce";
    case TCause::Sort:
      return "sort";
    case TCause::While:
      return "while";
  };

  //NOTE: This should never be reached as we enumerate every case in the switch.
  assert(false);
  throw Base::TImpossibleError(HERE);
}

Type::TType TImplicitFunc::GetReturnType() const {
  assert(this);

  /* TODO: A little ug */
  return Type.As<Type::TFunc>()->GetReturnType();
}

Type::TType TImplicitFunc::GetType() const {
assert(this);

  return Type;
}

TImplicitFunc::TImplicitFunc(const L0::TPackage *package, const TIdScope::TPtr &id_scope, TCause cause, const Type::TType &ret_type,
      const TNamedArgs &args, const Expr::TExpr::TPtr &expr, bool keep_mutable)
    : TFunction(package, id_scope), TInlineFunc(package, id_scope), Cause(cause),
      Type(Type::TFunc::Get(Type::TObj::Get(args), ret_type))  {

  PostCtor(args, expr, keep_mutable, true);
}