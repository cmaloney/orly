/* <stig/rt/postfix_cast.test.cc>

   Unit test for <stig/rt/postfix_cast.h>

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

#include <stig/rt/postfix_cast.h>

#include <vector>

#include <stig/rt/tuple.h>
#include <stig/rt/containers.h>
#include <stig/rt/operator.h>
#include <stig/rt/opt.h>
#include <stig/uuid.h>

#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Rt;

/* typedefs for containers */
typedef vector<int64_t> TIntList;
typedef vector<double> TRealList;
typedef vector<string> TStrList;
typedef TDict<int64_t, double> TIntRealDict;
typedef TDict<double, int64_t> TRealIntDict;
typedef TDict<int64_t, string> TIntStrDict;
typedef TSet<int64_t> TIntSet;
typedef TSet<double> TRealSet;
typedef TSet<string> TStrSet;
typedef Rt::TGenerator<long int>::TPtr TIntGen;
typedef Rt::TGenerator<string>::TPtr TStrGen;

/* */
TIntList il({1, 2, 3});
TRealList dl({1.0, 2.0, 3.0});
TStrList sl({"1", "2", "3"});
TIntRealDict idd({{1, 1.0}});
TRealIntDict did({{1.0, 1}});
TIntStrDict isd({{1, "1"}});
TIntSet is({1, 2});
TRealSet rs({1.0, 2.0});
TStrSet ss({"1", "2"});
TUUID uuid;
const TIntGen ig = MakeGenerator(il);
const TStrGen sg = MakeGenerator(sl);

FIXTURE(ValToVal) {
  /* TBool to TBool */
  EXPECT_EQ((CastAs<bool, bool>::Do(true)), true);
  /* TDict<TKey, TVal> to TDict<TKey, TVal> */
  EXPECT_TRUE(EqEq(CastAs<TIntRealDict, TIntRealDict>::Do(idd), idd));
  /* TId to TId */
  EXPECT_EQ((CastAs<TUUID, TUUID>::Do(uuid)), uuid);
  /* TInt to TInt */
  EXPECT_EQ((CastAs<int64_t, int64_t>::Do(1)), 1);
  /* TList<TVal> to TList<TVal> */
  EXPECT_TRUE(EqEq(CastAs<TIntList, TIntList>::Do(il), il));
  /* TObj to TObj */
  // TODO: Add a test case for object
  /* TOpt to TOpt */
  EXPECT_TRUE(IsKnownTrue(EqEq(CastAs<TOpt<int64_t>, TOpt<int64_t>>::Do(TOpt<int64_t>(1)), TOpt<int64_t>(1))));
  /* TReal to TReal */
  EXPECT_EQ((CastAs<double, double>::Do(1.0)), 1.0);
  /* TSeq to TSeq */
  // TODO: Add a test case for sequence
  /* TSet to TSet */
  EXPECT_TRUE(EqEq(CastAs<TIntSet, TIntSet>::Do(is), is));
  /* TStr to TStr */
  EXPECT_EQ((CastAs<string, string>::Do("hello")), "hello");
  /* TTimeDiff to TTimeDiff */
  // TODO: Add a test case for time diff
  /* TTimePnt to TTimePnt */
  // TODO: Add a test case for time pnt
}

FIXTURE(IntToReal) {
  EXPECT_EQ((CastAs<double, int64_t>::Do(0)), 0.0);
  EXPECT_EQ((CastAs<double, int64_t>::Do(1)), 1.0);
  EXPECT_EQ((CastAs<double, int64_t>::Do(-1)), -1.0);
}

FIXTURE(RealToInt) {
  EXPECT_EQ((CastAs<int64_t, double>::Do(0.0)), 0);
  EXPECT_EQ((CastAs<int64_t, double>::Do(1.0)), 1);
  EXPECT_EQ((CastAs<int64_t, double>::Do(-1.0)), -1);
}

FIXTURE(IntToString) {
  EXPECT_EQ((CastAs<string, int64_t>::Do(0)), "0");
  EXPECT_EQ((CastAs<string, int64_t>::Do(1)), "1");
  EXPECT_EQ((CastAs<string, int64_t>::Do(-1)), "-1");
}

FIXTURE(StringToInt) {
  EXPECT_EQ((CastAs<int64_t, string>::Do("0")), 0);
  EXPECT_EQ((CastAs<int64_t, string>::Do("1")), 1);
  EXPECT_EQ((CastAs<int64_t, string>::Do("-1")), -1);
}

FIXTURE(RealToString) {
  EXPECT_EQ((CastAs<string, double>::Do(0.0)), "0.00000");
  EXPECT_EQ((CastAs<string, double>::Do(1.0)), "1.00000");
  EXPECT_EQ((CastAs<string, double>::Do(-1.0)), "-1.00000");
}

FIXTURE(StringToReal) {
  EXPECT_EQ((CastAs<double, string>::Do("0.0")), 0.0);
  EXPECT_EQ((CastAs<double, string>::Do("1.0")), 1.0);
  EXPECT_EQ((CastAs<double, string>::Do("-1.0")), -1.0);
}

FIXTURE(ValToOptVal) {
  EXPECT_TRUE(IsKnownTrue((CastAs<TOpt<bool>, bool>::Do(true))));
  EXPECT_EQ((CastAs<TOpt<int64_t>, int64_t>::Do(1)).GetVal(), 1);
}

#if 0
FIXTURE(AddressToAddress) {
  /* TAddr<TInt, TStr> to TAddr<TStr, TInt> */
  EXPECT_EQ(EqEq(CastAs<TStrIntAddr, TIntStrAddr>::Do(TIntStrAddr(1, "1"))), TStrIntAddr("1", 1));
  /* TAddr<TInt, TStr> to TAddr<TStr, TReal> */
  EXPECT_EQ(EqEq(CastAs<TStrRealAddr, TIntStrAddr>::Do(TIntStrAddr(1, "1"))), TStrRealAddr("1", 1.0));
}
#endif

FIXTURE(ListToList) {
  /* TList<TInt> to TList<TReal> */
  EXPECT_TRUE(EqEq(CastAs<TRealList, TIntList>::Do(il), dl));
  /* TList<TStr> to TList<TInt> */
  EXPECT_TRUE(EqEq(CastAs<TIntList, TStrList>::Do(sl), il));
}

FIXTURE(SetToSet) {
  /* TSet<TInt> to TSet<TReal> */
  EXPECT_TRUE(EqEq(CastAs<TRealSet, TIntSet>::Do(is), rs));
  /* TSet<TStr> to TSet<TInt> */
  EXPECT_TRUE(EqEq(CastAs<TIntSet, TStrSet>::Do(ss), is));
}

FIXTURE(DictToDict) {
  /* TDict<TInt, TReal> to TDict<TReal, TInt> */
  EXPECT_TRUE(EqEq(CastAs<TRealIntDict, TIntRealDict>::Do(idd), did));
  /* TDict<TInt, TStr> to TDict<TReal, TInt> */
  EXPECT_TRUE(EqEq(CastAs<TRealIntDict, TIntStrDict>::Do(isd), did));
}

FIXTURE(GeneratorToList) {
  EXPECT_TRUE(EqEq(CastAs<TIntList, TIntGen>::Do(ig), il));
  EXPECT_TRUE(EqEq(CastAs<TStrList, TStrGen>::Do(sg), sl));
  EXPECT_TRUE(EqEq(CastAs<TIntList, TStrGen>::Do(sg), il));
}

FIXTURE(GeneratorToSet) {
  EXPECT_TRUE(EqEq(CastAs<TIntSet, TIntGen>::Do(ig), TIntSet{1, 2, 3}));
  EXPECT_TRUE(EqEq(CastAs<TStrSet, TStrGen>::Do(sg), TStrSet{"1", "2", "3"}));
  EXPECT_TRUE(EqEq(CastAs<TIntSet, TStrGen>::Do(sg), TIntSet{1, 2, 3}));
}