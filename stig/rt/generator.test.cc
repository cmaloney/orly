/* <stig/rt/generator.test.cc>

   Unit test for <stig/rt/
   Generator base classes.

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

#include <stig/rt/generator.h>

#include <memory>
#include <unordered_set>
#include <vector>

#include <stig/rt/opt.h>
#include <stig/rt/runtime_error.h>

#include <test/kit.h>

using namespace Stig::Rt;

FIXTURE(Range) {
  int expected = 14;
  for(auto it = MakeCursor(TRangeGenerator::NewWithSecond(14, 42, true, 15)); it; ++it) {
    EXPECT_EQ(expected++, *it);
  }
  EXPECT_EQ(expected, 43);

  EXPECT_THROW(TSystemError, []() {
    TRangeGenerator::NewWithSecond(1, 2, false, 0);
  });

  TRangeGenerator::NewWithSecond(2, 1, true, -1);

  expected = 14;
  for(auto it = MakeCursor(TRangeGenerator::New(14, 0, false)); it; ++it) {
    EXPECT_EQ(expected--, *it);
  }
  EXPECT_EQ(expected, 0);
}

FIXTURE(Stl) {
  std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  int index = 0;
  for(auto it = MakeCursor(TStlGenerator<std::vector<int>>::New(vec)); it; ++it, ++index) {
    EXPECT_EQ(vec[index], *it);
  }
  std::unordered_set<int> Set = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  size_t count = 0;
  for(auto it = MakeCursor(TStlGenerator<std::unordered_set<int>>::New(Set)); it; ++it, ++count) {}
  EXPECT_EQ(count, Set.size());
}

FIXTURE(VectorOfBool) {
  const std::vector<bool> vec = { true, false, false, true, false, false, true, false };
  size_t index = 0;
  for(auto it = MakeCursor(TStlGenerator<std::vector<bool>>::New(vec)); it; ++it, ++index) {
    EXPECT_EQ(*it, (index % 3) == 0);
  }
  EXPECT_EQ(index, vec.size());
}

FIXTURE(Filter) {
  std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  bool keep = true;
  std::function<bool (const int &)> filter_func = [&keep] (const int &){
    return keep;
  };
  auto vec_gen = TStlGenerator<std::vector<int>>::New(vec);
  unsigned int index = 0;
  for(auto it = MakeCursor(TFilterGenerator<int>::New(filter_func, vec_gen)); it; ++it, ++index) {
    EXPECT_EQ(vec[index], *it);
  }
  EXPECT_EQ(index, vec.size());

  //Make sure the filter/stl generator is reusable, and that the lambda is working properly.
  index = 0u;
  for(auto it = MakeCursor(TFilterGenerator<int>::New(filter_func, vec_gen)); it; ++it, ++index, keep = !keep) {
    EXPECT_EQ(vec[index], *it);
  }
  //We should only have gotten 2 out.
  EXPECT_EQ(index, 2u);

}

FIXTURE(Map) {
  std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::function<bool (const int &)> map_func = [] (const int &v) { return v % 2 == 0; };

  auto vec_gen = TStlGenerator<std::vector<int>>::New(vec);
  bool expected = false;
  for(auto it = TMapGenerator<bool, int>::New(map_func, vec_gen)->NewCursor(); it; ++it, expected = !expected) {
    EXPECT_EQ(expected, *it);
  }

}

FIXTURE(Take) {
  std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto vec_gen = TStlGenerator<std::vector<int>>::New(vec);
  int64_t take_count = 3;
  auto iter = TTakeGenerator<int>::New(take_count, vec_gen)->NewCursor();
  for (int i = 0; iter; ++i, ++iter) {
    EXPECT_TRUE(i < take_count);
    EXPECT_EQ(*iter, vec[i]);
  }
}

FIXTURE(While) {
  // **[1, 2, 3, 4, 5, 6, 7, 8, 9] while true
  std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto vec_gen = TStlGenerator<std::vector<int>>::New(vec);
  std::function<bool (const int &)> while_func = [](const int &) -> bool { return true; };
  auto iter = TWhileGenerator<int>::New(while_func, vec_gen)->NewCursor();
  for (int i = 0; iter; ++i, ++iter) {
    EXPECT_EQ(*iter, vec[i]);
  }
  // **[1, 2, 3, 4, 5] while that < 3
  std::vector<int> vec1 {1, 2, 3, 4, 5};
  auto vec_gen1 = TStlGenerator<std::vector<int>>::New(vec1);
  std::function<bool (const int &)> while_func1 = [](const int &n) -> bool { return n < 3; };
  auto iter1 = TWhileGenerator<int>::New(while_func1, vec_gen1)->NewCursor();
  for (int i = 0; iter1; ++i, ++iter1) {
    EXPECT_LT(*iter1, 3);
    EXPECT_TRUE(while_func1(*iter1));
    EXPECT_EQ(*iter1, vec1[i]);
  }
}

FIXTURE(GeneratorOfGenerator) { //generators of generators
  typedef TStlGenerator<std::vector<int>> TGen1;
  std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  TGen1::TPtr vec_gen = TGen1::New(vec);
  std::vector<TGen1::TPtr> vec_vec_gen {vec_gen, vec_gen, vec_gen};
  auto vec_gen_gen = TStlGenerator<std::vector<TGen1::TPtr>>::New(vec_vec_gen);
  int outer_count = 0, inner_count = 0;
  for (auto it1 = MakeCursor(vec_gen_gen); it1; ++it1) {
    ++outer_count;
    for (auto it2 = MakeCursor(*it1); it2; ++it2) {
      ++inner_count;
    }
  }
  EXPECT_EQ(outer_count, 3);
  EXPECT_EQ(inner_count, 27);
  //TODO: Generator of generator (aka seq of seq)
}

FIXTURE(GenericGenerators) {
  TGenerator<int64_t>::TPtr foo = TRangeGenerator::NewWithSecond(1, 2, true, 2);
  uint32_t count = 0;
  for(auto it = foo->NewCursor(); it; ++it) {
    ++count;
  }
  EXPECT_EQ(count, 2u);

  TGenerator<int64_t>::TPtr bar = TFilterGenerator<int64_t>::New([](const int64_t &) { return true; }, foo);
  count = 0;
  for(auto it2 = bar->NewCursor(); it2; ++it2) {
    ++count;
  }
  EXPECT_EQ(count, 2u);
}
