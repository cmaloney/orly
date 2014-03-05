/* <stig/client/program/translate_expr.test.cc>

   Unit test for <stig/client/program/translate_expr.h>.

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

#include <stig/client/program/translate_expr.h>

#include <functional>
#include <sstream>
#include <string>

#include <stig/client/program/parse_stmt.h>
#include <stig/sabot/state_dumper.h>
#include <stig/sabot/type_dumper.h>
#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Client::Program;

static void ParseExprStr(const char *str, const function<void (const TExpr *)> &cb) {
  ostringstream strm;
  strm << "echo ";
  strm << str;
  strm << ';';
  ParseStmtStr(
      strm.str().c_str(),
      [&cb](const TStmt *stmt) {
        auto echo_stmt = dynamic_cast<const TEchoStmt *>(stmt);
        assert(echo_stmt);
        cb(echo_stmt->GetExpr());
      }
  );
}

static string DumpType(const char *str) {
  ostringstream strm;
  ParseExprStr(
      str,
      [&strm](const TExpr *expr) {
        void *alloc = alloca(Sabot::Type::GetMaxTypeSize());
        Sabot::Type::TAny::TWrapper(NewTypeSabot(expr, alloc))->Accept(Sabot::TTypeDumper(strm));
      }
  );
  return strm.str();
}

static string DumpState(const char *str) {
  ostringstream strm;
  ParseExprStr(
      str,
      [&strm](const TExpr *expr) {
        void *alloc = alloca(Sabot::State::GetMaxStateSize());
        Sabot::State::TAny::TWrapper(NewStateSabot(expr, alloc))->Accept(Sabot::TStateDumper(strm));
      }
  );
  return strm.str();
}

FIXTURE(Bool) {
  EXPECT_EQ(DumpType("true"), "bool");
  EXPECT_EQ(DumpType("false"), "bool");
  EXPECT_EQ(DumpState("true"), "true");
  EXPECT_EQ(DumpState("false"), "false");
}

FIXTURE(Int) {
  EXPECT_EQ(DumpType("0"), "int64");
  EXPECT_EQ(DumpType("101"), "int64");
  EXPECT_EQ(DumpType("-101"), "int64");
  EXPECT_EQ(DumpState("0"), "0");
  EXPECT_EQ(DumpState("101"), "101");
  EXPECT_EQ(DumpState("-101"), "-101");
}

FIXTURE(Real) {
  EXPECT_EQ(DumpType("0.0"), "double");
  EXPECT_EQ(DumpType("98.6"), "double");
  EXPECT_EQ(DumpType("-98.6"), "double");
  EXPECT_EQ(DumpState("0.0"), "0");
  EXPECT_EQ(DumpState("98.6"), "98.6");
  EXPECT_EQ(DumpState("-98.6"), "-98.6");
}

FIXTURE(Id) {
  EXPECT_EQ(DumpType("{AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA}"), "uuid");
  EXPECT_EQ(DumpState("{AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA}"), "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
}

FIXTURE(TimePnt) {
  EXPECT_EQ(DumpType("{1970-01-01T00:00:00Z}"), "time_point");
  EXPECT_EQ(DumpState("{1970-01-01T00:00:00Z}"), "1970:01:01:00:00:00utc");
  EXPECT_EQ(DumpState("{1969-03-02T05:30:25Z}"), "1969:03:02:05:30:25utc");
}

FIXTURE(TimeDiff) {
  EXPECT_EQ(DumpType("{+5T0:0:0}"), "duration");
  EXPECT_EQ(DumpState("{+5T0:0:0}"), "432000000ms");
  EXPECT_EQ(DumpState("{0T0:0:1}"), "1000ms");
  EXPECT_EQ(DumpState("{0T0:0:0}"), "0ms");
}

FIXTURE(Str) {
  EXPECT_EQ(DumpType("'hello'"), "str");
  EXPECT_EQ(DumpType("\"hello\""), "str");
  EXPECT_EQ(DumpType("r'hello'"), "str");
  EXPECT_EQ(DumpType("r\"hello\""), "str");
  EXPECT_EQ(DumpState("'hello'"), "\"hello\"");
  EXPECT_EQ(DumpState("\"hello\""), "\"hello\"");
  EXPECT_EQ(DumpState("r'hello'"), "\"hello\"");
  EXPECT_EQ(DumpState("r\"hello\""), "\"hello\"");
}

FIXTURE(Opt) {
  EXPECT_EQ(DumpType("101?"), "opt(int64)");
  EXPECT_EQ(DumpType("unknown int"), "opt(int64)");
  EXPECT_EQ(DumpState("101?"), "opt(101)");
  EXPECT_EQ(DumpState("unknown int"), "unknown opt(int64)");
}

FIXTURE(Set) {
  EXPECT_EQ(DumpType("{ 101, 102, 103 }"), "set(int64)");
  EXPECT_EQ(DumpType("empty { int }"), "set(int64)");
  EXPECT_EQ(DumpState("{ 102, 103, 101 }"), "set(101, 102, 103)");
  EXPECT_EQ(DumpState("empty { int }"), "empty set(int64)");
}

FIXTURE(List) {
  EXPECT_EQ(DumpType("[ 101, 102, 103 ]"), "vector(int64)");
  EXPECT_EQ(DumpType("empty [ int ]"), "vector(int64)");
  EXPECT_EQ(DumpState("[ 103, 101, 102 ]"), "vector(103, 101, 102)");
  EXPECT_EQ(DumpState("empty [ int ]"), "empty vector(int64)");
}

FIXTURE(Dict) {
  EXPECT_EQ(DumpType("{ 101: 'hello', 102: 'doctor' }"), "map(int64, str)");
  EXPECT_EQ(DumpType("empty { int: str }"), "map(int64, str)");
  EXPECT_EQ(DumpState("{ 102: 'doctor', 101: 'hello' }"), "map(101: \"hello\", 102: \"doctor\")");
  EXPECT_EQ(DumpState("empty { int: str }"), "empty map(int64, str)");
}

FIXTURE(Obj) {
  EXPECT_EQ(DumpType("<{ }>"), "record()");
  EXPECT_EQ(DumpType("<{ .x: 1.5, .y: 2.5, .label: 'north' }>"), "record(label: str, x: double, y: double)");
  EXPECT_EQ(DumpState("<{ }>"), "record()");
  EXPECT_EQ(DumpState("<{ .x: 1.5, .y: 2.5, .label: 'north' }>"), "record(label: \"north\", x: 1.5, y: 2.5)");
}

FIXTURE(Addr) {
  EXPECT_EQ(DumpType("<[ ]>"), "tuple()");
  EXPECT_EQ(DumpType("<[ true, 101, 'hello' ]>"), "tuple(bool, int64, str)");
  EXPECT_EQ(DumpType("<[ desc 101 ]>"), "tuple(desc(int64))");
  EXPECT_EQ(DumpState("<[ ]>"), "tuple()");
  EXPECT_EQ(DumpState("<[ true, 101, 'hello' ]>"), "tuple(true, 101, \"hello\")");
  EXPECT_EQ(DumpState("<[ desc 101 ]>"), "tuple(desc(101))");
}

FIXTURE(ParensType) {
  EXPECT_EQ(DumpType("(true)"), "bool");
  EXPECT_EQ(DumpType("((false))"), "bool");
  EXPECT_EQ(DumpState("(true)"), "true");
  EXPECT_EQ(DumpState("((false))"), "false");
}

#if 0
static string ParseAndDumpExpr(const char *str) {
  ostringstream strm;
  strm << "echo ";
  strm << str;
  strm << ';';
  TSuprena arena;
  void *core_alloc = alloca(sizeof(TCore));
  TCore *core = nullptr;
  ParseStmtStr(
      strm.str().c_str(),
      [&arena, core_alloc, &core](const TStmt *stmt) {
        auto echo_stmt = dynamic_cast<const TEchoStmt *>(stmt);
        assert(echo_stmt);
        core = TranslateExpr(&arena, core_alloc, echo_stmt->GetExpr());
      }
  );
  assert(core);
  strm.str("");
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper(core->NewState(&arena, state_alloc))->Accept(Sabot::TStateDumper(strm));
  return strm.str();
}

FIXTURE(Bool) {
  EXPECT_EQ(ParseAndDumpExpr("true"), "true");
  EXPECT_EQ(ParseAndDumpExpr("false"), "false");
}

FIXTURE(Int) {
  EXPECT_EQ(ParseAndDumpExpr("0"), "0");
  EXPECT_EQ(ParseAndDumpExpr("101"), "101");
  EXPECT_EQ(ParseAndDumpExpr("-101"), "-101");
}

FIXTURE(Real) {
  EXPECT_EQ(ParseAndDumpExpr("0.0"), "0");
  EXPECT_EQ(ParseAndDumpExpr("98.6"), "98.6");
  EXPECT_EQ(ParseAndDumpExpr("-98.6"), "-98.6");
}

FIXTURE(Id) {
  EXPECT_EQ(ParseAndDumpExpr("{AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA}"), "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
}

FIXTURE(TimePnt) {
  EXPECT_EQ(ParseAndDumpExpr("{1970-01-01T00:00:00Z}"), "1970:01:01:00:00:00utc");
}

FIXTURE(TimeDiff) {
  EXPECT_EQ(ParseAndDumpExpr("{+5T0:0:0}"), "432000000ms");
}

FIXTURE(Strs) {
  EXPECT_EQ(ParseAndDumpExpr("'hello'"), "\"hello\"");
  EXPECT_EQ(ParseAndDumpExpr("\"hello\""), "\"hello\"");
  EXPECT_EQ(ParseAndDumpExpr("r'hello'"), "\"hello\"");
  EXPECT_EQ(ParseAndDumpExpr("r\"hello\""), "\"hello\"");
}

FIXTURE(Parens) {
  EXPECT_EQ(ParseAndDumpExpr("(true)"), "true");
  EXPECT_EQ(ParseAndDumpExpr("((false))"), "false");
}
#endif

#if 0
using namespace std;
using namespace Base;
using namespace Stig::Atom;
using namespace Stig::Client::Program;

class TStmtVisitor final : public TStmt::TVisitor {
  public:
  TStmtVisitor(const TExpr *&expr) : Expr(expr) {}
  virtual void operator()(const TExitStmt *) const override {}
  virtual void operator()(const TPrimtStmt *that) const override {
    Expr = that->GetExpr();
  }
  private:
  const TExpr *&Expr;
};

bool Check(const char *str, const TCore &expected, TExtensibleArena *expect_arena) {
  TFastArena arena;
  TProgram *program = TProgram::ParseStr(str);
  const TStmt *stmt = program->GetStmt();
  const TExpr *expr = nullptr;
  stmt->Accept(TStmtVisitor(expr));
  TCore result;
  TranslateExpr(&arena, &result, expr);
  std::cout << "Result = " << TAtom(result, &arena) << std::endl;
  return EXPECT_TRUE(IsEq(result.Compare(&arena, expect_arena, expected, false)));
}

FIXTURE(True) {
  TFastArena arena;
  TCore expected(&arena, true);
  Check("print true;", expected, &arena);
}
FIXTURE(False) {
  TFastArena arena;
  TCore expected(&arena, false);
  Check("print false;", expected, &arena);
}

FIXTURE(Int) {
  TFastArena arena;
  TCore expected(&arena, 12345L);
  Check("print 12345;", expected, &arena);
}

FIXTURE(Real) {
  TFastArena arena;
  TCore expected(&arena, 3.14);
  Check("print 3.14;", expected, &arena);
}

FIXTURE(Id) {
  TFastArena arena;
  TCore expected(&arena, Base::TUuid("AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA"));
  Check("print {AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA};", expected, &arena);
}

FIXTURE(TimePnt) {
  TFastArena arena;
  TCore expected(&arena, chrono::time_point<chrono::system_clock>());
  Check("print {1970-01-01T00:00:00Z};", expected, &arena);
}

FIXTURE(TimeDiff) {
  TFastArena arena;
  TCore expected(&arena, chrono::seconds(432000 /*five days*/));
  Check("print {+5T0:0:0};", expected, &arena);
}

FIXTURE(DoubleQuotedRawStr) {
  TFastArena arena;
  TCore expected(&arena, "Hello World");
  Check("print r\"Hello World\";", expected, &arena);
}

FIXTURE(SingleQuotedRawStr) {
  TFastArena arena;
  TCore expected(&arena, "Hello World");
  Check("print r'Hello World';", expected, &arena);
}

FIXTURE(DoubleQuotedStr) {
  TFastArena arena;
  TCore expected(&arena, "Hello World");
  Check("print \"Hello World\";", expected, &arena);
}

FIXTURE(SingleQuotedStr) {
  TFastArena arena;
  TCore expected(&arena, "Hello World");
  Check("print 'Hello World';", expected, &arena);
}

FIXTURE(List) {
  TFastArena arena;
  TCore expected_1(&arena, std::vector<int64_t>{1, 2, 3, 4, 5, 6, 9});
  TCore expected_2(&arena, std::vector<std::vector<int64_t>>{std::vector<int64_t>{1, 2, 3}, std::vector<int64_t>{4, 5, 6, 9}});
  Check("print [1, 2, 3, 4, 5, 6, 9];", expected_1, &arena);
  Check("print [[1, 2, 3], [4, 5, 6, 9]];", expected_2, &arena);
}

FIXTURE(Set) {
  TFastArena arena;
  TCore expected_1(&arena, std::set<int64_t>{1, 2, 3, 4, 5, 6, 9});
  TCore expected_2(&arena, std::set<std::set<int64_t>>{std::set<int64_t>{1, 2, 3}, std::set<int64_t>{4, 5, 6, 9}});
  Check("print {1, 2, 3, 4, 5, 6, 9};", expected_1, &arena);
  Check("print {9, 1, 6, 2, 5, 4, 3};", expected_1, &arena);
  Check("print {{1, 2, 3}, {4, 5, 6, 9}};", expected_2, &arena);
}

FIXTURE(Dict) {
  TFastArena arena;
  TCore expected_1(&arena, std::map<int64_t, double>{{1, 1.1}, {2, 2.2}, {3, 3.3}, {4, 4.4}, {5, 5.5}, {6, 6.6}, {9, 9.9}});
  TCore expected_2(&arena, std::map<int64_t, std::map<int64_t, double>>{{ 1, std::map<int64_t, double>{{1, 1.1}, {2, 2.2}, {3, 3.3}}}, {2, std::map<int64_t, double>{{4, 4.4}, {5, 5.5}, {6, 6.6}, {9, 9.9}}}});
  Check("print {1:1.1, 2:2.2, 3:3.3, 4:4.4, 5:5.5, 6:6.6, 9:9.9};", expected_1, &arena);
  Check("print {9:9.9, 1:1.1, 6:6.6, 2:2.2, 5:5.5, 4:4.4, 3:3.3};", expected_1, &arena);
  Check("print {1:{1:1.1, 2:2.2, 3:3.3}, 2:{4:4.4, 5:5.5, 6:6.6, 9:9.9}};", expected_2, &arena);
}

FIXTURE(Addr) {
  TFastArena arena;
  TCore expected_1(&arena, std::tuple<int64_t, bool, std::set<int64_t>>(5,
                                                                        true,
                                                                        std::set<int64_t>{1, 2, 3, 4}));
  TCore expected_2(&arena, std::tuple<int64_t, bool, std::set<int64_t>, std::tuple<int64_t, bool, std::set<int64_t>>>(5,
                                                                                                                      true,
                                                                                                                      std::set<int64_t>{1, 2, 3, 4}, std::tuple<int64_t, bool, std::set<int64_t>>(1, false, std::set<int64_t>{9, 10})));
  Check("print <[5, true, {1, 2, 3, 4}]>;", expected_1, &arena);
  Check("print <[5, true, {1, 2, 3, 4}, <[1, false, {9, 10}]>]>;", expected_2, &arena);
}

class TObj {
  public:

  TObj(int a, bool b, double c)
      : A(a), B(b), C(c) {}

  bool operator==(const TObj &that) const {
    return A == that.A && B == that.B && B == that.C;
  }

  int A;
  bool B;
  double C;

  private:

  STIG__DECL_RECORD(TObj, STIG__BASES(), int, bool, double);

};

STIG__IMPL_RECORD(
    TObj,
    STIG__FIELD(TObj, A),
    STIG__FIELD(TObj, B),
    STIG__FIELD(TObj, C)
);

FIXTURE(Obj) {
  TFastArena arena;
  string expected_str = "[3](A: 5, B: true, C: 10.1)";
  TProgram *program = TProgram::ParseStr("print <{.A:5, .C:10.1, .B:true}>;");
  const TStmt *stmt = program->GetStmt();
  const TExpr *expr = nullptr;
  stmt->Accept(TStmtVisitor(expr));
  TCore result;
  TranslateExpr(&arena, &result, expr);
  ostringstream strm;
  result.Dump(&arena, strm);
  EXPECT_EQ(strm.str(), expected_str);

}

FIXTURE(Opt) {
  TFastArena arena;
  TCore expected_1(&arena, Base::TOpt<int64_t>(45));
  TCore expected_2(&arena, Base::TOpt<std::set<int64_t>>(std::set<int64_t>{1, 2, 3}));
  TCore expected_3(&arena, Base::TOpt<Base::TOpt<int64_t>>(Base::TOpt<int64_t>(45)));
  Check("print 45?;", expected_1, &arena);
  Check("print {1, 2, 3}?;", expected_2, &arena);
  Check("print 45??;", expected_3, &arena);
}

FIXTURE(Unknown) {
  TFastArena arena;
  TCore expected_1(&arena, Base::TOpt<int64_t>());
  TCore expected_2(&arena, Base::TOpt<std::set<int64_t>>());
  Check("print unknown int;", expected_1, &arena);
  Check("print unknown {int};", expected_2, &arena);
}

FIXTURE(Paren) {
  TFastArena arena;
  TCore expected_1(&arena, 12345L);
  TCore expected_2(&arena, std::vector<int64_t>{1, 2, 3, 4, 5});
  Check("print (12345);", expected_1, &arena);
  Check("print ((12345));", expected_1, &arena);
  Check("print ([(1), 2, (3), 4, 5]);", expected_2, &arena);
}

FIXTURE(Empty) {
  TFastArena arena;
  TCore expected_1(&arena, std::vector<int64_t>{});
  TCore expected_2(&arena, std::set<int64_t>{});
  TCore expected_3(&arena, std::map<int64_t, double>{});
  Check("print empty [int];", expected_1, &arena);
  Check("print empty {int};", expected_2, &arena);
  Check("print empty {int : real };", expected_3, &arena);
}
#endif
