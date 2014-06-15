/* <base/json.test.cc>

   Unit test for <base/json.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/json.h>

#include <tuple>

#include <base/tuple_utils.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

template <typename TTpl, size_t... Idx>
vector<TJson::TKind> GetKindsImpl(const index_sequence<Idx...> &) {
  return vector<TJson::TKind> { TJson(typename tuple_element<Idx, TTpl>::type()).GetKind()... };
}

template <typename TTpl>
vector<TJson::TKind> GetKinds() {
  return GetKindsImpl<TTpl>(make_index_sequence<tuple_size<TTpl>::value>());
}

template <typename TTpl>
bool AllKindsEq(TJson::TKind expected_kind) {
  static const size_t expected_size = tuple_size<TTpl>::value;
  auto kinds = GetKinds<TTpl>();
  bool success = (kinds.size(), expected_size);
  for (size_t i = 0; success && i < expected_size; ++i) {
    success = (kinds[i] == expected_kind);
  }
  return success;
}

FIXTURE(ConstructedKinds) {
  EXPECT_TRUE(TJson().GetKind() == TJson::Null);
  EXPECT_TRUE((AllKindsEq<tuple<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double>>(TJson::Number)));
  EXPECT_TRUE((AllKindsEq<tuple<char, const char *, string>>(TJson::String)));
  EXPECT_TRUE((AllKindsEq<tuple<TJson::TObject>>(TJson::Object)));
  EXPECT_TRUE((AllKindsEq<tuple<TJson::TArray>>(TJson::Array)));
}

static const TJson::TArray  Array  = { 1, 2, 3 };
static const TJson::TObject Object = { { "a", 1 }, { "b", 2 }, { "c", 3 } };

FIXTURE(Format) {
  EXPECT_EQ(TJson().Format(), "null");
  EXPECT_EQ(TJson(true).Format(), "true");
  EXPECT_EQ(TJson(false).Format(), "false");
  EXPECT_EQ(TJson(0).Format(), "0");
  EXPECT_EQ(TJson(101).Format(), "101");
  EXPECT_EQ(TJson(-101).Format(), "-101");
  EXPECT_EQ(TJson(98.6).Format(), "98.6");
  EXPECT_EQ(TJson(Array).Format(), "[1,2,3]");
  EXPECT_EQ(TJson(Object).Format(), R"({"a":1,"b":2,"c":3})");
  EXPECT_EQ(TJson("").Format(), R"("")");
  EXPECT_EQ(TJson('x').Format(), R"("x")");
  EXPECT_EQ(TJson("foo").Format(), R"("foo")");
  EXPECT_EQ(TJson(string("bar")).Format(), R"("bar")");
}

FIXTURE(Defaults) {
  EXPECT_EQ(TJson(TJson::Null), TJson());
  EXPECT_EQ(TJson(TJson::Bool), TJson(false));
  EXPECT_EQ(TJson(TJson::Number), TJson(0));
  EXPECT_EQ(TJson(TJson::Array), TJson(TJson::TArray()));
  EXPECT_EQ(TJson(TJson::Object), TJson(TJson::TObject()));
  EXPECT_EQ(TJson(TJson::String), TJson(""));
}

template <typename TArg>
void TestCopyAndMove(TArg &&arg, TJson::TKind kind) {
  const TJson expected = forward<TArg>(arg);
  EXPECT_EQ(expected.GetKind(), kind);
  TJson a = expected;
  EXPECT_EQ(a, expected);
  TJson b = move(a);
  EXPECT_EQ(b, expected);
  EXPECT_TRUE(a.GetKind() == TJson::Null);
  a = expected;
  EXPECT_EQ(a, expected);
  b.Reset();
  EXPECT_TRUE(b.GetKind() == TJson::Null);
  b = move(a);
  EXPECT_EQ(b, expected);
  EXPECT_TRUE(a.GetKind() == TJson::Null);
}

FIXTURE(CopyAndMove) {
  TestCopyAndMove(TJson(), TJson::Null);
  TestCopyAndMove(true, TJson::Bool);
  TestCopyAndMove(101, TJson::Number);
  TestCopyAndMove(Array, TJson::Array);
  TestCopyAndMove(Object, TJson::Object);
  TestCopyAndMove("hello", TJson::String);
}

FIXTURE(ArraySubscripting) {
  TJson j(3, TJson::String);
  EXPECT_EQ(j.GetSize(), 3U);
  EXPECT_EQ(j[0], "");
  EXPECT_EQ(j[1], "");
  EXPECT_EQ(j[2], "");
  j[0] = "hello";
  j[1] = "doctor";
  j[2] = "name";
  EXPECT_EQ(j[0], "hello");
  EXPECT_EQ(j[1], "doctor");
  EXPECT_EQ(j[2], "name");
}

FIXTURE(ObjectSubscripting) {
  TJson j = TJson::Object;
  EXPECT_EQ(j.GetSize(), 0U);
  j["a"] = 1;
  EXPECT_EQ(j.GetSize(), 1U);
  j["b"] = 2;
  EXPECT_EQ(j.GetSize(), 2U);
  j["c"] = 3;
  EXPECT_EQ(j.GetSize(), 3U);
  EXPECT_EQ(j, Object);
  const TJson k = j;
  EXPECT_EQ(k["a"], 1);
  EXPECT_EQ(k["b"], 2);
  EXPECT_EQ(k["c"], 3);
}

FIXTURE(Parse) {
  EXPECT_EQ(TJson::Parse("null"), TJson());
  EXPECT_EQ(TJson::Parse("true"), true);
  EXPECT_EQ(TJson::Parse("false"), false);
  EXPECT_EQ(TJson::Parse("0"), 0);
  EXPECT_EQ(TJson::Parse("+101"), 101);
  EXPECT_EQ(TJson::Parse("-101"), -101);
  EXPECT_EQ(TJson::Parse("98.6"), 98.6);
  EXPECT_EQ(TJson::Parse("1e10"), 1e10);
  EXPECT_EQ(TJson::Parse("[]"), TJson::TArray());
  EXPECT_EQ(TJson::Parse("[ 1, 2, 3 ]"), Array);
  EXPECT_EQ(TJson::Parse("{}"), TJson::TObject());
  EXPECT_EQ(TJson::Parse(R"({ "a": 1, "b": 2, "c": 3} )"), Object);
  EXPECT_EQ(TJson::Parse(R"("")"), "");
  EXPECT_EQ(TJson::Parse(R"("hello")"), "hello");

  EXPECT_EQ(TJson::Parse(R"(["/usr/include/stdc-predef.h","/usr/include/c++/4.9.0/ostream"])"),
            TJson::TArray({"/usr/include/stdc-predef.h", "/usr/include/c++/4.9.0/ostream"}));
}

FIXTURE(EscapeSequences) {
  //TODO: Properly unescape things json.org says we should.
  //EXPECT_EQ(TJson::Parse(R"foo("\"\\\/\b\f\n\r\t\u0037")foo").GetString(), "\\/\b\f\n\r\t7");
  //NOTE: For now we're not formatting / escaping unicode sequences.
  EXPECT_EQ(TJson("\"\\/\b\f\n\r\t7").Format(), R"foo("\"\\\/\b\f\n\r\t7")foo");
}

std::string AsStr(Base::TJson::TKind kind) {
  std::ostringstream oss;
  oss << kind;
  return oss.str();
}

FIXTURE(KindToStr) {
  EXPECT_EQ(AsStr(TJson::Bool), "bool");
  EXPECT_EQ(AsStr(TJson::Null), "null");
  EXPECT_EQ(AsStr(TJson::Number), "number");
  EXPECT_EQ(AsStr(TJson::Array), "array");
  EXPECT_EQ(AsStr(TJson::Object), "object");
  EXPECT_EQ(AsStr(TJson::String), "string");
}