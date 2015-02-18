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

#include <sstream>
#include <string>
#include <tuple>

#include <base/as_str.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wexit-time-destructors"


/* Parse a text. */
template <typename TArg>
static TJson Parse(TArg &&arg) {
  std::istringstream strm(std::forward<TArg>(arg));
  TJson result;
  result.Read(strm);
  return std::move(result);
}

template <typename TTpl, size_t... Idx>
vector<TJson::TKind> GetKindsImpl(const index_sequence<Idx...> &) {
  return vector<TJson::TKind>{TJson(typename tuple_element<Idx, TTpl>::type()).GetKind()...};
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
  for(size_t i = 0; success && i < expected_size; ++i) {
    success = (kinds[i] == expected_kind);
  }
  return success;
}

FIXTURE(ConstructedKinds) {
  EXPECT_TRUE(TJson().GetKind() == TJson::Null);
  EXPECT_TRUE(
      (AllKindsEq<tuple<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>>(
          TJson::Number)));
  EXPECT_TRUE((AllKindsEq<tuple<char, const char *, string>>(TJson::String)));
  EXPECT_TRUE((AllKindsEq<tuple<TJson::TObject>>(TJson::Object)));
  EXPECT_TRUE((AllKindsEq<tuple<TJson::TArray>>(TJson::Array)));
}

static const TJson::TArray Array = {1, 2, 3};
static const TJson::TObject Object = {{"a", 1}, {"b", 2}, {"c", 3}};

FIXTURE(Format) {
  EXPECT_EQ(AsStr(TJson()), "null");
  EXPECT_EQ(AsStr(TJson(true)), "true");
  EXPECT_EQ(AsStr(TJson(false)), "false");
  EXPECT_EQ(AsStr(TJson("")), R"("")");
  EXPECT_EQ(AsStr(TJson('x')), R"("x")");
  EXPECT_EQ(AsStr(TJson("foo")), R"("foo")");
  EXPECT_EQ(AsStr(TJson(string("bar"))), R"("bar")");
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
  EXPECT_EQ(Parse("null"), TJson());
  EXPECT_EQ(Parse("true"), true);
  EXPECT_EQ(Parse("false"), false);
  EXPECT_EQ(Parse("0"), 0);
  EXPECT_EQ(Parse("+101"), 101);
  EXPECT_EQ(Parse("-101"), -101);
  EXPECT_EQ(Parse("[]"), TJson::TArray());
  EXPECT_EQ(Parse("[ 1, 2, 3 ]"), Array);
  EXPECT_EQ(Parse("{}"), TJson::TObject());
  EXPECT_EQ(Parse(R"({ "a": 1, "b": 2, "c": 3} )"), Object);
  EXPECT_EQ(Parse(R"("")"), "");
  EXPECT_EQ(Parse(R"("hello")"), "hello");
  EXPECT_EQ(Parse(R"(hello)"), "hello");
  EXPECT_EQ(Parse(R"([hello,world,null,no,truest,falser,nullary])"),
            TJson::TArray({"hello", "world", TJson(), "no", "truest", "falser", "nullary"}));

  EXPECT_EQ(Parse(R"(["/usr/include/stdc-predef.h","/usr/include/c++/4.9.0/ostream"])"),
            TJson::TArray({"/usr/include/stdc-predef.h", "/usr/include/c++/4.9.0/ostream"}));
  EXPECT_EQ(Parse("[1,]"), TJson::TArray{1});
  EXPECT_EQ(Parse("{\"a\": 1,}"), TJson::TObject({{"a", 1}}));
}

FIXTURE(EscapeSequences) {
  EXPECT_EQ(Parse(R"foo("\"\\\/\b\f\n\r\t")foo").GetString(), "\"\\/\b\f\n\r\t");
  EXPECT_EQ(AsStr(TJson("\"\\/\b\f\n\r\t7")), R"foo("\"\\/\b\f\n\r\t7")foo");
}

FIXTURE(KindToStr) {
  EXPECT_EQ(AsStr(TJson::Bool), "bool");
  EXPECT_EQ(AsStr(TJson::Null), "null");
  EXPECT_EQ(AsStr(TJson::Number), "number");
  EXPECT_EQ(AsStr(TJson::Array), "array");
  EXPECT_EQ(AsStr(TJson::Object), "object");
  EXPECT_EQ(AsStr(TJson::String), "string");
}

FIXTURE(RoundTrip) {
  EXPECT_EQ(Parse(AsStr(TJson(12345678))), 12345678);
  EXPECT_EQ(Parse(AsStr(TJson(0))), 0);
  EXPECT_EQ(Parse(AsStr(TJson(101))), 101);
  EXPECT_EQ(Parse(AsStr(TJson(-101))), -101);
  EXPECT_EQ(Parse(AsStr(TJson(Array))), Array);
  EXPECT_EQ(Parse(AsStr(TJson(Object))), Object);
}
