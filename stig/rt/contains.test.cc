/* <stig/rt/contains.test.cc>

   Unit test for <stig/rt/containers.h>

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

#include <stig/rt/opt.h>

#include <stig/rt/containers.h>
#include <stig/rt/operator.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rt;

typedef vector<int64_t> TListInt;
typedef TSet<bool> TSetBool;
typedef TDict<int64_t, string> TDictIntStr;

/* Empties */
TListInt empty_li;
TSetBool empty_se;
TDictIntStr empty_di;

/* Non-empties */
TListInt li({1, 2, 3});
TSetBool se({true, false});
TDictIntStr di({{1, "1"}, {2, "2"}});

/* Unknowns */
TOpt<TListInt>    unknown_li;
TOpt<TSetBool>    unknown_se;
TOpt<TDictIntStr> unknown_di;

/* Opt empties */
TOpt<TListInt>    opt_empty_li(empty_li);
TOpt<TSetBool>    opt_empty_se(empty_se);
TOpt<TDictIntStr> opt_empty_di(empty_di);

/* Opt Non-empties */
TOpt<TListInt>    opt_li(li);
TOpt<TSetBool>    opt_se(se);
TOpt<TDictIntStr> opt_di(di);

FIXTURE(TInEmptyContainerOfT) {
  /* T in empty [T] */
  EXPECT_FALSE(Contains(empty_li, 1L));
  /* T in empty {T} */
  EXPECT_FALSE(Contains(empty_se, true));
  /* Tk? in empty {Tk:Tv} */
  EXPECT_FALSE(Contains(empty_di, 1L));
}

FIXTURE(TOptInEmptyContainerOfT) {
  /* T? in empty [T] */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_li, TOpt<int64_t>(1L))));
  /* T? in empty {T} */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_se, TOpt<bool>(true))));
  /* Tk? in empty {Tk:Tv} */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_di, TOpt<int64_t>(1L))));
}

FIXTURE(TInNonEmptyContainerOfT) {
  /* T in [T] */
  EXPECT_TRUE(Contains(li, 1L));
  /* T in {T} */
  EXPECT_TRUE(Contains(se, true));
  /* Tk in {Tk:Tv} */
  EXPECT_TRUE(Contains(di, 1L));
}

FIXTURE(TOptInNonEmptyContainerOfT) {
  /* T? in [T] */
  EXPECT_TRUE(IsKnownTrue(Contains(li, TOpt<int64_t>(1L))));
  /* T? in {T} */
  EXPECT_TRUE(IsKnownTrue(Contains(se, TOpt<bool>(true))));
  /* Tk? in {Tk:Tv} */
  EXPECT_TRUE(IsKnownTrue(Contains(di, TOpt<int64_t>(1L))));
}

FIXTURE(TInUnknownContainerOfT) {
  /* T in unknown [T] */
  EXPECT_TRUE(IsUnknown(Contains(unknown_li, 1L)));
  /* T in unknown {T} */
  EXPECT_TRUE(IsUnknown(Contains(unknown_se, true)));
  /* Tk in unknown {Tk:Tv} */
  EXPECT_TRUE(IsUnknown(Contains(unknown_di, 1L)));
}

FIXTURE(TOptInUnknownContainerOfT) {
  /* T? in unknown [T] */
  EXPECT_TRUE(IsUnknown(Contains(unknown_li, TOpt<int64_t>(1L))));
  /* T? in unknown {T} */
  //EXPECT_TRUE(Contains(unknown_se, TOpt<bool>(true)).IsUnknown());
  /* Tk? in unknown {Tk:Tv} */
  //EXPECT_TRUE(Contains(unknown_di, TOpt<int64_t>(1L)).IsUnknown());
}

FIXTURE(TInOptEmptyContainerOfT) {
  /* T in (empty [T])? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_li, 1L)));
  /* T in (empty {T})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_se, true)));
  /* Tk in (empty {Tk:Tv})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_di, 1L)));
}

FIXTURE(TOptInOptEmptyContainerOfT) {
  /* T? in (empty [T])? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_li, TOpt<int64_t>(1L))));
  /* T? in (empty {T})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_se, TOpt<bool>(true))));
  /* Tk? in (empty {Tk:Tv})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_di, TOpt<int64_t>(1L))));
}

FIXTURE(TInOptNonEmptyContainerOfT) {
  /* T in [T]? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_li, 1L)));
  /* T in {T}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_se, true)));
  /* Tk in {Tk:Tv}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_di, 1L)));
}

FIXTURE(TOptInOptNonEmptyContainerOfT) {
  /* T? in [T]? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_li, TOpt<int64_t>(1L))));
  /* T? in {T}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_se, TOpt<bool>(true))));
  /* Tk? in {Tk:Tv}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_di, TOpt<int64_t>(1L))));
}

typedef vector<TOpt<int64_t>> TListOptInt;
typedef TSet<TOpt<bool>> TSetOptBool;
typedef TDict<TOpt<int64_t>, string> TDictOptIntStr;

/* Empties containers with optionals */
TListOptInt    empty_li_opt;
TSetOptBool    empty_se_opt;
TDictOptIntStr empty_di_opt;

/* Non-empties */
TListOptInt    li_opt({TOpt<int64_t>(1), TOpt<int64_t>(2), TOpt<int64_t>(3)});
TSetOptBool    se_opt({TOpt<bool>(true), TOpt<bool>(false)});
TDictOptIntStr di_opt({{TOpt<int64_t>(1), "1"}, {TOpt<int64_t>(2), "2"}});

/* Unknowns */
TOpt<TListOptInt>    unknown_li_opt;
TOpt<TSetOptBool>    unknown_se_opt;
TOpt<TDictOptIntStr> unknown_di_opt;

/* Opt empties */
TOpt<TListOptInt>    opt_empty_li_opt(empty_li_opt);
TOpt<TSetOptBool>    opt_empty_se_opt(empty_se_opt);
TOpt<TDictOptIntStr> opt_empty_di_opt(empty_di_opt);

/* Opt Non-empties */
TOpt<TListOptInt> opt_li_opt(li_opt);
TOpt<TSetOptBool> opt_se_opt(se_opt);
TOpt<TDictOptIntStr> opt_di_opt(di_opt);

FIXTURE(TInEmptyContainerOfTOpt) {
  /* T in empty [T?] */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_li_opt, 1L)));
  /* T in empty {T?} */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_se_opt, true)));
  /* Tk in empty {Tk?:Tv} */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_di_opt, 1L)));
}

FIXTURE(TOptInEmptyContainerOfTOpt) {
  /* T? in empty [T?] */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_li_opt, TOpt<int64_t>(1L))));
  /* T? in empty {T?} */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_se_opt, TOpt<bool>(true))));
  /* Tk? in empty {Tk?:Tv} */
  EXPECT_TRUE(IsKnownFalse(Contains(empty_di_opt, TOpt<int64_t>(1L))));
}

FIXTURE(TInNonEmptyContainerOfTOpt) {
  /* T in [T?] */
  EXPECT_TRUE(IsKnownTrue(Contains(li_opt, 1L)));
  /* T in {T?} */
  EXPECT_TRUE(IsKnownTrue(Contains(se_opt, true)));
  /* Tk in {Tk?:Tv} */
  EXPECT_TRUE(IsKnownTrue(Contains(di_opt, 1L)));
}

FIXTURE(TOptInNonEmptyContainerOfTOpt) {
  /* T? in [T?] */
  EXPECT_TRUE(IsKnownTrue(Contains(li_opt, TOpt<int64_t>(1L))));
  /* T? in {T?} */
  EXPECT_TRUE(IsKnownTrue(Contains(se_opt, TOpt<bool>(true))));
  /* Tk? in {Tk?:Tv} */
  EXPECT_TRUE(IsKnownTrue(Contains(di_opt, TOpt<int64_t>(1L))));
}

FIXTURE(TInUnknownContainerOfTOpt) {
  /* T in unknown [T] */
  EXPECT_TRUE(IsUnknown(Contains(unknown_li_opt, 1L)));
  /* T in unknown {T} */
  EXPECT_TRUE(IsUnknown(Contains(unknown_se_opt, true)));
  /* Tk in unknown {Tk:Tv} */
  EXPECT_TRUE(IsUnknown(Contains(unknown_di_opt, 1L)));
}

FIXTURE(TOptInUnknownContainerOfTOpt) {
  /* T? in unknown [T] */
  EXPECT_TRUE(IsUnknown(Contains(unknown_li_opt, TOpt<int64_t>(1L))));
  /* T? in unknown {T} */
  EXPECT_TRUE(IsUnknown(Contains(unknown_se_opt, TOpt<bool>(true))));
  /* Tk? in unknown {Tk:Tv} */
  EXPECT_TRUE(IsUnknown(Contains(unknown_di_opt, TOpt<int64_t>(1L))));
}

FIXTURE(TInOptEmptyContainerOfTOpt) {
  /* T in (empty [T?])? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_li_opt, 1L)));
  /* T in (empty {T?})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_se_opt, true)));
  /* Tk in (empty {Tk?:Tv})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_di_opt, 1L)));
}

FIXTURE(TOptInOptEmptyContainerOfTOpt) {
  /* T? in (empty [T?])? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_li_opt, TOpt<int64_t>(1L))));
  /* T? in (empty {T?})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_se_opt, TOpt<bool>(true))));
  /* Tk? in (empty {Tk?:Tv})? */
  EXPECT_TRUE(IsKnownFalse(Contains(opt_empty_di_opt, TOpt<int64_t>(1L))));
}

FIXTURE(TInOptNonEmptyContainerOfTOpt) {
  /* T in [T?]? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_li_opt, 1L)));
  /* T in {T?}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_se_opt, true)));
  /* Tk in {Tk?:Tv}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_di_opt, 1L)));
}

FIXTURE(TOptInOptNonEmptyContainerOfTOpt) {
  /* T? in [T?]? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_li_opt, TOpt<int64_t>(1L))));
  /* T? in {T?}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_se_opt, TOpt<bool>(true))));
  /* Tk? in {Tk?:Tv}? */
  EXPECT_TRUE(IsKnownTrue(Contains(opt_di_opt, TOpt<int64_t>(1L))));
}
