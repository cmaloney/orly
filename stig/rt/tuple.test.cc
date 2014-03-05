/* <stig/rt/tuple.test.cc>

   Unit test for <stig/rt/tuple.h>.

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

#include <stig/rt/tuple.h>

#include <stig/rt/operator.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rt;

/* Addr */
//TAddr<> empty_addr;
std::tuple<> empty_addr;
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>> addr_1(5, 2, 1);
std::tuple<int64_t, int64_t, int64_t> addr_1(5, 2, 1);
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Desc, int64_t>> addr_1d(6, 2, 1);
std::tuple<int64_t, int64_t, Stig::TDesc<int64_t>> addr_1d(6, 2, 1);
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>> addr_2(6, 2, 1);
std::tuple<int64_t, int64_t, int64_t> addr_2(6, 2, 1);
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Desc, int64_t>, TAddrElem<TDir::Asc, int64_t>> addr_2d(6, 2, 1);
std::tuple<int64_t, Stig::TDesc<int64_t>, int64_t> addr_2d(6, 2, 1);
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, double>,  TAddrElem<TDir::Asc, int64_t>> addr_3(5, 2.0, 1);
std::tuple<int64_t, double,  int64_t> addr_3(5, 2.0, 1);
//TAddr<TAddrElem<TDir::Desc, int64_t>, TAddrElem<TDir::Asc, double>,  TAddrElem<TDir::Asc, int64_t>> addr_3d(5, 2.0, 1);
std::tuple<Stig::TDesc<int64_t>, double,  int64_t> addr_3d(5, 2.0, 1);
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, double>,  TAddrElem<TDir::Asc, int64_t>> addr_4(5, 2.1, 1);
std::tuple<int64_t, double,  int64_t> addr_4(5, 2.1, 1);
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>> addr_5(5, 2);
std::tuple<int64_t, int64_t> addr_5(5, 2);
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, int64_t>> addr_6(5, 2, 1, 0);
std::tuple<int64_t, int64_t, int64_t, int64_t> addr_6(5, 2, 1, 0);

/* Nested */
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, TAddr<>>> addr_8(1, TAddr<>());
std::tuple<int64_t, std::tuple<>> addr_8(1, std::tuple<>());
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, string>>>> addr_9(1, TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, string>>(1, "hello"));
std::tuple<int64_t, std::tuple<int64_t, string>> addr_9(1, std::tuple<int64_t, string>(1, "hello"));
//TAddr<TAddrElem<TDir::Asc, int64_t>, TAddrElem<TDir::Asc, TAddr<TAddrElem<TDir::Asc, int64_t>>>> addr_10(1, TAddr<TAddrElem<TDir::Asc, int64_t>>(2));
std::tuple<int64_t, std::tuple<int64_t>> addr_10(1, std::tuple<int64_t>(2));

FIXTURE(Access) {
  EXPECT_EQ(std::get<0>(addr_1), 5);
  EXPECT_EQ(std::get<1>(addr_1), 2);
  EXPECT_EQ(std::get<2>(addr_1), 1);
}

FIXTURE(EmptyToEmptyComp) {
  EXPECT_TRUE (EqEq(empty_addr, empty_addr));
  EXPECT_FALSE(Neq (empty_addr, empty_addr));
  EXPECT_FALSE(Lt  (empty_addr, empty_addr));
  EXPECT_TRUE (LtEq(empty_addr, empty_addr));
  EXPECT_FALSE(Gt  (empty_addr, empty_addr));
  EXPECT_TRUE (GtEq(empty_addr, empty_addr));
}

FIXTURE(SomeToSomeComp) {
  EXPECT_TRUE(EqEq(addr_1, addr_1));
  EXPECT_TRUE(Neq (addr_1, addr_2));
  EXPECT_TRUE(Lt  (addr_1, addr_2));
  EXPECT_TRUE(Gt  (addr_2, addr_1));
  EXPECT_TRUE(Lt  (addr_3, addr_4));
  EXPECT_TRUE(Gt  (addr_4, addr_3));

}


FIXTURE(Size) {
  EXPECT_EQ(Stig::Rt::GetTupleSize(empty_addr), 0U);
  EXPECT_EQ(Stig::Rt::GetTupleSize(addr_1), 3U);
}


FIXTURE(Hash) {
  typedef std::tuple<int64_t, bool> TAddrLocal;
  TSet<TAddrLocal> addr_set({
      TAddrLocal(12, true),
      TAddrLocal(13, false)
      });
}

struct TPnt {
  public:
  TPnt(int x, int y) : X(x), Y(y) {}
  bool operator==(const TPnt &that) const {
    return X == that.X && Y == that.Y;
  }
  private:
  int X, Y;
};

FIXTURE(Obj) {
  std::tuple<TPnt> a(TPnt(1, 2)), b(TPnt(1, 2)), c(TPnt(3, 4));
  EXPECT_TRUE(a == a);
  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a != c);
  EXPECT_FALSE(a != a);
  EXPECT_FALSE(a != b);
  EXPECT_FALSE(a == c);
  std::tuple<TPnt> x(a), y(b), z(c);
  EXPECT_TRUE(x == x);
  EXPECT_TRUE(x == y);
  EXPECT_TRUE(x != z);
  EXPECT_FALSE(x != x);
  EXPECT_FALSE(x != y);
  EXPECT_FALSE(x == z);
}