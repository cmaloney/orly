/* <stig/expr/test_kit.h>

   TODO

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

#pragma once

#include <string>
#include <vector>

#include <base/chrono.h>
#include <stig/expr.h>
#include <stig/pos_range.h>
#include <stig/rt/opt.h>
#include <stig/symbol/function.h>
#include <stig/symbol/given_param_def.h>
#include <stig/symbol/package.h>
#include <stig/symbol/result_def.h>
#include <stig/type.h>
#include <stig/type/type_czar.h>
#include <stig/var.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Expr;
using namespace Stig::Symbol;

static Type::TTypeCzar type_czar;

/* TType */

/* bool */
static Type::TType bool_type = Type::TBool::Get();
static Type::TType opt_bool_type = Type::TOpt::Get(bool_type);
/* int */
static Type::TType int_type = Type::TInt::Get();
static Type::TType opt_int_type = Type::TOpt::Get(int_type);
/* real */
static Type::TType real_type = Type::TReal::Get();
static Type::TType opt_real_type = Type::TOpt::Get(real_type);
/* str */
static Type::TType str_type = Type::TStr::Get();
static Type::TType opt_str_type = Type::TOpt::Get(str_type);
/* tdf */
static Type::TType tdf_type = Type::TTimeDiff::Get();
static Type::TType opt_tdf_type = Type::TOpt::Get(tdf_type);
/* tpt */
static Type::TType tpt_type = Type::TTimePnt::Get();
static Type::TType opt_tpt_type = Type::TOpt::Get(tpt_type);
/* addr */
static Type::TAddr::TElems addr_vec {{Asc, int_type}, {Desc, int_type}};
static Type::TType addr_type = Type::TAddr::Get(addr_vec);
static Type::TType opt_addr_type = Type::TOpt::Get(addr_type);
/* list */
static Type::TType list_type = Type::TList::Get(int_type);
static Type::TType opt_list_type = Type::TOpt::Get(list_type);
/* dict */
static Type::TType dict_type = Type::TDict::Get(int_type, real_type);
static Type::TType opt_dict_type = Type::TOpt::Get(dict_type);
/* set */
static Type::TType set_type = Type::TSet::Get(int_type);
static Type::TType opt_set_type = Type::TOpt::Get(set_type);
/* obj */
static Type::TType obj_type = Type::TObj::Get(Type::TObj::TElems({{"x", int_type}, {"y", real_type}}));
static Type::TType opt_obj_type = Type::TOpt::Get(obj_type);
/* range */
static Type::TType seq_type = Type::TSeq::Get(int_type);

/* TExpr */

/* bool */
static auto bool0 = TLiteral::New(Var::TVar(false), TPosRange());
static auto bool1 = TLiteral::New(Var::TVar(true ), TPosRange());
static auto bool2 = TLiteral::New(Var::TVar(false), TPosRange());
static auto opt_bool0 = TLiteral::New(Var::TVar(Rt::TOpt<bool>(false)), TPosRange());
static auto opt_bool1 = TLiteral::New(Var::TVar(Rt::TOpt<bool>(true )), TPosRange());
static auto opt_bool2 = TLiteral::New(Var::TVar(Rt::TOpt<bool>(false)), TPosRange());
/* int */
static auto int0 = TLiteral::New(Var::TVar(0), TPosRange());
static auto int1 = TLiteral::New(Var::TVar(1), TPosRange());
static auto int2 = TLiteral::New(Var::TVar(2), TPosRange());
static auto opt_int0 = TLiteral::New(Var::TVar(Rt::TOpt<int64_t>(0)), TPosRange());
static auto opt_int1 = TLiteral::New(Var::TVar(Rt::TOpt<int64_t>(1)), TPosRange());
static auto opt_int2 = TLiteral::New(Var::TVar(Rt::TOpt<int64_t>(2)), TPosRange());
/* real */
static auto real0 = TLiteral::New(Var::TVar(0.0), TPosRange());
static auto real1 = TLiteral::New(Var::TVar(1.0), TPosRange());
static auto real2 = TLiteral::New(Var::TVar(2.0), TPosRange());
static auto opt_real0 = TLiteral::New(Var::TVar(Rt::TOpt<double>(0.0)), TPosRange());
static auto opt_real1 = TLiteral::New(Var::TVar(Rt::TOpt<double>(1.0)), TPosRange());
static auto opt_real2 = TLiteral::New(Var::TVar(Rt::TOpt<double>(2.0)), TPosRange());
/* str */
static string zero("zero"), one("one"), two("two");
static auto str0 = TLiteral::New(Var::TVar(zero), TPosRange());
static auto str1 = TLiteral::New(Var::TVar(one ), TPosRange());
static auto str2 = TLiteral::New(Var::TVar(two ), TPosRange());
static auto opt_str0 = TLiteral::New(Var::TVar(Rt::TOpt<string>(zero)), TPosRange());
static auto opt_str1 = TLiteral::New(Var::TVar(Rt::TOpt<string>(one )), TPosRange());
static auto opt_str2 = TLiteral::New(Var::TVar(Rt::TOpt<string>(two )), TPosRange());
/* tdf */
static Chrono::TTimeDiff lt0(-2), eq0(0), gt0(2);
static auto tdf0 = TLiteral::New(Var::TVar(lt0), TPosRange());
static auto tdf1 = TLiteral::New(Var::TVar(eq0), TPosRange());
static auto tdf2 = TLiteral::New(Var::TVar(gt0), TPosRange());
static auto opt_tdf0 = TLiteral::New(Var::TVar(Rt::TOpt<Chrono::TTimeDiff>(lt0)), TPosRange());
static auto opt_tdf1 = TLiteral::New(Var::TVar(Rt::TOpt<Chrono::TTimeDiff>(eq0)), TPosRange());
static auto opt_tdf2 = TLiteral::New(Var::TVar(Rt::TOpt<Chrono::TTimeDiff>(gt0)), TPosRange());
/* tpt */
static Chrono::TTimePnt now(Chrono::Now());
static auto tpt0 = TLiteral::New(Var::TVar(now), TPosRange());
static auto tpt1 = TLiteral::New(Var::TVar(now), TPosRange());
static auto tpt2 = TLiteral::New(Var::TVar(now), TPosRange());
static auto opt_tpt0 = TLiteral::New(Var::TVar(Rt::TOpt<Chrono::TTimePnt>(now)), TPosRange());
static auto opt_tpt1 = TLiteral::New(Var::TVar(Rt::TOpt<Chrono::TTimePnt>(now)), TPosRange());
static auto opt_tpt2 = TLiteral::New(Var::TVar(Rt::TOpt<Chrono::TTimePnt>(now)), TPosRange());
/* addr: <[asc 0, desc 1]> */
static auto addr_int0 = TLiteral::New(Var::TVar(0), TPosRange());
static auto addr_int1 = TLiteral::New(Var::TVar(1), TPosRange());
static auto addr0 = TAddr::New(TAddr::TMemberVec {{Asc, addr_int0}, {Desc, addr_int1}}, TPosRange());
/* list: [0, 1, 2] */
static auto list_int0 = TLiteral::New(Var::TVar(0), TPosRange());
static auto list_int1 = TLiteral::New(Var::TVar(1), TPosRange());
static auto list_int2 = TLiteral::New(Var::TVar(2), TPosRange());
static TList::TExprVec list_expr_vec {list_int0, list_int1, list_int2};
static auto empty_list = TList::New(Type::TList::Get(int_type), TPosRange());
static auto list0 = TList::New(list_expr_vec, TPosRange());
/* dict: {0: 0.0, 1: 1.1} */
static auto dict_int0 = TLiteral::New(Var::TVar(0), TPosRange());
static auto dict_int1 = TLiteral::New(Var::TVar(1), TPosRange());
static auto dict_real0 = TLiteral::New(Var::TVar(0.0), TPosRange());
static auto dict_real1 = TLiteral::New(Var::TVar(1.0), TPosRange());
static TDict::TMemberMap dict_member_map {{dict_int0, dict_real0}, {dict_int1, dict_real1}};
static auto empty_dict = TDict::New(Type::TDict::Get(int_type, real_type), TPosRange());
static auto dict0 = TDict::New(dict_member_map, TPosRange());
/* set: {0, 1, 2} */
static auto set_int0 = TLiteral::New(Var::TVar(0), TPosRange());
static auto set_int1 = TLiteral::New(Var::TVar(1), TPosRange());
static auto set_int2 = TLiteral::New(Var::TVar(2), TPosRange());
static TSet::TExprSet set_expr_set {set_int0, set_int1, set_int2};
static auto empty_set = TSet::New(Type::TSet::Get(int_type), TPosRange());
static auto set0 = TSet::New(set_expr_set, TPosRange());
/* obj: <{.x: 0, .y: 0.0}> */
static auto obj_int0 = TLiteral::New(Var::TVar(0), TPosRange());
static auto obj_real0 = TLiteral::New(Var::TVar(0.0), TPosRange());
static TObj::TMemberMap obj_member_map({{"x", obj_int0}, {"y", obj_real0}});
static auto obj0 = TObj::New(obj_member_map, TPosRange());
/* range: [0..1] */
static auto range_int0 = TLiteral::New(Var::TVar(0), TPosRange());
static auto range_int1 = TLiteral::New(Var::TVar(1), TPosRange());
static auto range0 = TRange::New(range_int0, nullptr, range_int1, true, TPosRange());
