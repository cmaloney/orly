/* <stig/indy/util/merge_sorter.test.cc>

   Unit test for <stig/indy/util/merge_sorter.h>.

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

#include <stig/indy/util/merge_sorter.h>

#include <stig/atom/suprena.h>
#include <stig/indy/key.h>
#include <stig/sabot/type_dumper.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Util;

FIXTURE(Typical) {
  /*
  Cur Prefix [tuple(132192934)] is not less than Stored Prefix [tuple(14933990)]
  [0] Pushing Current [tuple(6224472, desc(2013:03:04:19:49:29utc))]
  [2] Pushing Current [tuple(2478538, desc(2013:03:04:19:49:36utc))]
  [tuple(2478538, desc(2013:03:04:19:49:36utc))]
  [tuple(2478538)]
  [2] Pushing Cur [tuple(132192934, desc(2013:03:04:19:49:36utc))]
  [tuple(6224472, desc(2013:03:04:19:49:29utc))]
  [tuple(6224472)]
  [0] Pushing Cur [tuple(14933990, desc(2013:03:04:19:49:10utc))]
  [tuple(132192934, desc(2013:03:04:19:49:36utc))]
  [tuple(132192934)]
  [tuple(14933990, desc(2013:03:04:19:49:10utc))]
  [tuple(14933990)]
  */
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  TKeyCopySorter<size_t> sorter;
  TSuprena suprena;
  const Sabot::TStdTimePoint time_point_1(Sabot::TStdDuration(1));
  const Sabot::TStdTimePoint time_point_2(Sabot::TStdDuration(2));
  const Sabot::TStdTimePoint time_point_3(Sabot::TStdDuration(3));
  const Sabot::TStdTimePoint time_point_4(Sabot::TStdDuration(4));
  TKey key_1(make_tuple(6224472L, Stig::TDesc<Sabot::TStdTimePoint>(time_point_1)), &suprena, state_alloc);
  TKey key_2(make_tuple(2478538L, Stig::TDesc<Sabot::TStdTimePoint>(time_point_2)), &suprena, state_alloc);
  TKey key_3(make_tuple(132192934L, Stig::TDesc<Sabot::TStdTimePoint>(time_point_3)), &suprena, state_alloc);
  TKey key_4(make_tuple(14933990L, Stig::TDesc<Sabot::TStdTimePoint>(time_point_4)), &suprena, state_alloc);
  TKeyCopySorter<size_t>::TMergeElement *key_sorter_alloc = reinterpret_cast<TKeyCopySorter<size_t>::TMergeElement *>(malloc(sizeof(TKeyCopySorter<size_t>::TMergeElement) * 2));
  new (key_sorter_alloc + 0) TKeyCopySorter<size_t>::TMergeElement(&sorter, key_1, 0UL, 0UL);
  new (key_sorter_alloc + 1) TKeyCopySorter<size_t>::TMergeElement(&sorter, key_2, 1UL, 1UL);
  EXPECT_LT(key_2, key_1);
  EXPECT_FALSE(Atom::IsGt(key_2.Compare(key_1)));
  EXPECT_LT(key_1, key_4);
  EXPECT_FALSE(Atom::IsGt(key_1.Compare(key_4)));
  EXPECT_LT(key_4, key_3);
  EXPECT_FALSE(Atom::IsGt(key_4.Compare(key_3)));
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  std::cout << "types [";
  key_2.GetCore().GetType(key_2.GetArena(), type_alloc)->Accept(Sabot::TTypeDumper(std::cout));
  std::cout << "] > [";
  key_1.GetCore().GetType(key_1.GetArena(), type_alloc)->Accept(Sabot::TTypeDumper(std::cout));
  std::cout<< "]" << std::endl;
  if (EXPECT_FALSE(sorter.IsEmpty())) {
    TSequenceNumber pop_num;
    TKey &key = sorter.Peek();
    EXPECT_EQ(key, key_2);
    size_t pos = sorter.Pop(pop_num);
    EXPECT_EQ(pos, 1UL);
    EXPECT_EQ(pop_num, 1UL);
    new (key_sorter_alloc + 1) TKeyCopySorter<size_t>::TMergeElement(&sorter, key_3, 2UL, 1UL);
  }
  if (EXPECT_FALSE(sorter.IsEmpty())) {
    TSequenceNumber pop_num;
    TKey &key = sorter.Peek();
    EXPECT_EQ(key, key_1);
    size_t pos = sorter.Pop(pop_num);
    EXPECT_EQ(pos, 0UL);
    EXPECT_EQ(pop_num, 0UL);
    new (key_sorter_alloc + 0) TKeyCopySorter<size_t>::TMergeElement(&sorter, key_4, 3UL, 0UL);
  }
  if (EXPECT_FALSE(sorter.IsEmpty())) {
    TSequenceNumber pop_num;
    TKey &key = sorter.Peek();
    EXPECT_EQ(key, key_4);
    size_t pos = sorter.Pop(pop_num);
    EXPECT_EQ(pos, 0UL);
    EXPECT_EQ(pop_num, 3UL);
  }
  if (EXPECT_FALSE(sorter.IsEmpty())) {
    TSequenceNumber pop_num;
    TKey &key = sorter.Peek();
    EXPECT_EQ(key, key_3);
    size_t pos = sorter.Pop(pop_num);
    EXPECT_EQ(pos, 1UL);
    EXPECT_EQ(pop_num, 2UL);
  }
  EXPECT_TRUE(sorter.IsEmpty());
}