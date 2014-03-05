/* <stig/rt/operator.test.cc>

   Unit test for <stig/rt/operator.h>

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

#include <stig/rt/operator.h>

#include <stig/rt/tuple.h>
#include <stig/rt/opt.h>
#include <stig/var/impl.h>

#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Rt;

typedef vector<int64_t> Tili;
typedef vector<TOpt<int64_t>> Toili;
typedef std::tuple<int64_t, bool> TIntBoolTuple;
typedef TMutable<TIntBoolTuple, int64_t> Tmibi;
typedef TMutable<TIntBoolTuple, TOpt<int64_t>> Tmiboi;
typedef TOpt<int64_t> Toi;

/* List of known values */
Tili li1{{1}};
Tili li12{1, 2};
Tili li123{1, 2, 3};
Tili li3{{3}};
Tili li32{3, 2};
Tili li321{3, 2, 1};

/* List of optionals, there is an implicit cast to TOpt here */
Toili opt_li1{{1}};
Toili opt_li12{1, 2};
Toili opt_li123{1, 2, 3};
Toili opt_li3{{3}};
Toili opt_li32{3, 2};
Toili opt_li321{3, 2, 1};

/* Mutables, both with the implicit cast to TOpt and without */
Tmibi mutable_1(TIntBoolTuple(1, true), 1);
Tmibi mutable_2(TIntBoolTuple(2, true), 2);
Tmiboi mutable_1o(TIntBoolTuple(1, true), 1);
Tmiboi mutable_2o(TIntBoolTuple(2, true), 2);

FIXTURE(ListCompare) {
  /* On knowns */
  EXPECT_TRUE(EqEq(li123, li123));
  EXPECT_TRUE(Lt  (li123, li3));
  EXPECT_TRUE(LtEq(li123, li32));
  EXPECT_TRUE(Gt  (li321, li12));
  EXPECT_TRUE(GtEq(li321, li321));

  /* On optionals */
  EXPECT_TRUE(IsKnownTrue(EqEq(opt_li123, opt_li123)));
  EXPECT_TRUE(IsKnownTrue(Lt  (opt_li123, opt_li3)));
  EXPECT_TRUE(IsKnownTrue(LtEq(opt_li123, opt_li32)));
  EXPECT_TRUE(IsKnownTrue(Gt  (opt_li321, opt_li12)));
  EXPECT_TRUE(IsKnownTrue(GtEq(opt_li321, opt_li321)));
  /* We know the second element is different */
  EXPECT_TRUE(IsKnownFalse(EqEq(opt_li123, vector<Toi>({*Toi::Unknown, Toi(1), Toi(3)}))));
}

FIXTURE(Mutable) {
  EXPECT_TRUE(EqEq(mutable_1, mutable_1));
  EXPECT_TRUE(Lt  (mutable_1, mutable_2));
  EXPECT_TRUE(LtEq(mutable_1, mutable_2));
  EXPECT_TRUE(Gt  (mutable_2, mutable_1));
  EXPECT_TRUE(GtEq(mutable_2, mutable_1));

  /* mutable vs. non-mutable */
  EXPECT_TRUE(EqEq(mutable_1, 1));
  EXPECT_TRUE(Lt  (mutable_1, 2));
  EXPECT_TRUE(LtEq(mutable_1, 2));
  EXPECT_TRUE(Gt  (mutable_2, 1));
  EXPECT_TRUE(GtEq(mutable_2, 1));

  EXPECT_TRUE(EqEq(1, mutable_1));
  EXPECT_TRUE(Lt  (1, mutable_2));
  EXPECT_TRUE(LtEq(1, mutable_2));
  EXPECT_TRUE(Gt  (2, mutable_1));
  EXPECT_TRUE(GtEq(2, mutable_1));

  //TODO: With optionals!
  EXPECT_TRUE(IsKnownTrue(EqEq(mutable_1o, mutable_1)));
  EXPECT_TRUE(IsKnownTrue(Lt  (mutable_1o, mutable_2)));
  EXPECT_TRUE(IsKnownTrue(LtEq(mutable_1o, mutable_2)));
  EXPECT_TRUE(IsKnownTrue(Gt  (mutable_2, mutable_1o)));
  EXPECT_TRUE(IsKnownTrue(GtEq(mutable_2, mutable_1o)));

  /* mutable vs. non-mutable */
  EXPECT_TRUE(IsKnownTrue(EqEq(mutable_1o, 1)));
  EXPECT_TRUE(IsKnownTrue(Lt  (mutable_1o, 2)));
  EXPECT_TRUE(IsKnownTrue(LtEq(mutable_1o, 2)));
  EXPECT_TRUE(IsKnownTrue(Gt  (mutable_2o, 1)));
  EXPECT_TRUE(IsKnownTrue(GtEq(mutable_2o, 1)));

  EXPECT_TRUE(IsKnownTrue(EqEq(Toi(1), mutable_1)));
  EXPECT_TRUE(IsKnownTrue(Lt  (Toi(1), mutable_2)));
  EXPECT_TRUE(IsKnownTrue(LtEq(Toi(1), mutable_2)));
  EXPECT_TRUE(IsKnownTrue(Gt  (Toi(2), mutable_1)));
  EXPECT_TRUE(IsKnownTrue(GtEq(Toi(2), mutable_1)));
}

FIXTURE(AddrInList) {
  EXPECT_TRUE(EqEq(vector<TIntBoolTuple>(), vector<TIntBoolTuple>()));
  EXPECT_TRUE(EqEq(vector<TIntBoolTuple>({TIntBoolTuple(1, true)}),
                   vector<TIntBoolTuple>({TIntBoolTuple(1, true)})));
  EXPECT_TRUE(EqEq(vector<TIntBoolTuple>({TIntBoolTuple(1, true), TIntBoolTuple(2, false)}),
                   vector<TIntBoolTuple>({TIntBoolTuple(1, true), TIntBoolTuple(2, false)})));
  EXPECT_FALSE(EqEq(vector<TIntBoolTuple>({TIntBoolTuple(1, true)}),
                    vector<TIntBoolTuple>({TIntBoolTuple(1, false)})));
  EXPECT_FALSE(EqEq(vector<TIntBoolTuple>({TIntBoolTuple(1, true), TIntBoolTuple(2, true)}),
                    vector<TIntBoolTuple>({TIntBoolTuple(1, true), TIntBoolTuple(1, false)})));
}

FIXTURE(Match) {
  /* Should Match */
  EXPECT_TRUE(Match(1, 1));
  EXPECT_TRUE(Match(TOpt<int64_t>(1), TOpt<int64_t>(1)));
  EXPECT_TRUE(Match(*TOpt<int64_t>::Unknown, *TOpt<int64_t>::Unknown));
  EXPECT_TRUE(Match(Tili({1, 2, 3}), Tili({1, 2, 3})));
  EXPECT_TRUE(Match(Toili({TOpt<int64_t>(), TOpt<int64_t>()}),
                    Toili({TOpt<int64_t>(), TOpt<int64_t>()})));
  EXPECT_TRUE(Match(Toili({TOpt<int64_t>(1), TOpt<int64_t>(2)}),
                    Toili({TOpt<int64_t>(1), TOpt<int64_t>(2)})));

  /* Should not match */
  EXPECT_FALSE(Match(0, 1));
  EXPECT_FALSE(Match(TOpt<int64_t>(1), TOpt<int64_t>(2)));
  EXPECT_FALSE(Match(TOpt<int64_t>(1), *TOpt<int64_t>::Unknown));
  EXPECT_FALSE(Match(Toili({TOpt<int64_t>(2), TOpt<int64_t>()}),
                     Toili({TOpt<int64_t>(1), TOpt<int64_t>(2)})));
}

/* Logical operators. */

/* And */

FIXTURE(BoolAndBool) {
  EXPECT_FALSE(And(false, false));
  EXPECT_FALSE(And(false, true ));
  EXPECT_FALSE(And(true , false));
  EXPECT_TRUE (And(true , true ));
}

FIXTURE(OptBoolAndBool) {
  EXPECT_TRUE(IsUnknown(And(*TOpt<bool>::Unknown, true)));
  EXPECT_FALSE(And(TOpt<bool>(false), false).GetVal());
  EXPECT_FALSE(And(TOpt<bool>(false), true).GetVal());
  EXPECT_FALSE(And(TOpt<bool>(true), false).GetVal());
  EXPECT_TRUE(And(TOpt<bool>(true), true).GetVal());
}

FIXTURE(BoolAndOptBool) {
  EXPECT_TRUE(IsKnownFalse(And(false, *TOpt<bool>::Unknown)));
  EXPECT_FALSE(And(false, TOpt<bool>(false)).GetVal());
  EXPECT_FALSE(And(false, TOpt<bool>(true )).GetVal());
  EXPECT_FALSE(And(true , TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (And(true , TOpt<bool>(true )).GetVal());
}

FIXTURE(OptBoolAndOptBool) {
  EXPECT_TRUE (IsUnknown(And(*TOpt<bool>::Unknown, *TOpt<bool>::Unknown)));
  EXPECT_FALSE(And(TOpt<bool>(false), TOpt<bool>(false)).GetVal());
  EXPECT_FALSE(And(TOpt<bool>(false), TOpt<bool>(true )).GetVal());
  EXPECT_FALSE(And(TOpt<bool>(true ), TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (And(TOpt<bool>(true ), TOpt<bool>(true )).GetVal());
}

/* Not */

FIXTURE(NotBool) {
  EXPECT_TRUE(Not(false));
  EXPECT_FALSE(Not(true));
}

FIXTURE(NotOptBool) {
  EXPECT_TRUE(IsUnknown(Not(*TOpt<bool>::Unknown)));
  EXPECT_TRUE(IsKnownTrue(Not(TOpt<bool>(false))));
  EXPECT_TRUE(IsKnownFalse(Not(TOpt<bool>(true))));
}

/* Or */

FIXTURE(BoolOrBool) {
  EXPECT_FALSE(Or(false, false));
  EXPECT_TRUE (Or(false, true ));
  EXPECT_TRUE (Or(true , false));
  EXPECT_TRUE (Or(true , true ));
}

FIXTURE(OptBoolOrBool) {
  EXPECT_TRUE(IsKnownTrue(Or(*TOpt<bool>::Unknown, true)));
  EXPECT_FALSE(Or(TOpt<bool>(false), false).GetVal());
  EXPECT_TRUE (Or(TOpt<bool>(false), true ).GetVal());
  EXPECT_TRUE (Or(TOpt<bool>(true ), false).GetVal());
  EXPECT_TRUE (Or(TOpt<bool>(true ), true ).GetVal());
}

FIXTURE(BoolOrOptBool) {
  EXPECT_TRUE(IsUnknown(Or(false, *TOpt<bool>::Unknown)));
  EXPECT_FALSE(Or(false, TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (Or(false, TOpt<bool>(true )).GetVal());
  EXPECT_TRUE (Or(true , TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (Or(true , TOpt<bool>(true )).GetVal());
}

FIXTURE(OptBoolOrOptBool) {
  EXPECT_TRUE(IsUnknown(Or(*TOpt<bool>::Unknown, *TOpt<bool>::Unknown)));
  EXPECT_FALSE(Or(TOpt<bool>(false), TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (Or(TOpt<bool>(false), TOpt<bool>(true )).GetVal());
  EXPECT_TRUE (Or(TOpt<bool>(true ), TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (Or(TOpt<bool>(true ), TOpt<bool>(true )).GetVal());
}

/* Xor */

FIXTURE(BoolXorBool) {
  EXPECT_FALSE(Xor(false, false));
  EXPECT_TRUE (Xor(false, true ));
  EXPECT_TRUE (Xor(true , false));
  EXPECT_FALSE(Xor(true , true ));
}

FIXTURE(OptBoolXorBool) {
  EXPECT_TRUE(IsUnknown(Xor(*TOpt<bool>::Unknown, true)));
  EXPECT_FALSE(Xor(TOpt<bool>(false), false).GetVal());
  EXPECT_TRUE (Xor(TOpt<bool>(false), true ).GetVal());
  EXPECT_TRUE (Xor(TOpt<bool>(true ), false).GetVal());
  EXPECT_FALSE(Xor(TOpt<bool>(true ), true ).GetVal());
}

FIXTURE(BoolXorOptBool) {
  EXPECT_TRUE(IsUnknown(Xor(false, *TOpt<bool>::Unknown)));
  EXPECT_FALSE(Xor(false, TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (Xor(false, TOpt<bool>(true )).GetVal());
  EXPECT_TRUE (Xor(true , TOpt<bool>(false)).GetVal());
  EXPECT_FALSE(Xor(true , TOpt<bool>(true )).GetVal());
}

FIXTURE(OptBoolXorOptBool) {
  EXPECT_TRUE(IsUnknown(Xor(*TOpt<bool>::Unknown, *TOpt<bool>::Unknown)));
  EXPECT_FALSE(Xor(TOpt<bool>(false), TOpt<bool>(false)).GetVal());
  EXPECT_TRUE (Xor(TOpt<bool>(false), TOpt<bool>(true )).GetVal());
  EXPECT_TRUE (Xor(TOpt<bool>(true ), TOpt<bool>(false)).GetVal());
  EXPECT_FALSE(Xor(TOpt<bool>(true ), TOpt<bool>(true )).GetVal());
}