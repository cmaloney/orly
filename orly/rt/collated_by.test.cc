/* <orly/rt/collated_by.test.cc>

   Unit test for <orly/rt/collated_by.h>

   Collated by generator.

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

#include <orly/rt/collated_by.h>

#include <memory>
#include <unordered_set>
#include <vector>

#include <orly/rt/generator.h>
#include <orly/rt/opt.h>
#include <orly/rt/runtime_error.h>

#include <test/kit.h>

using namespace Stig::Rt;

FIXTURE(CollatedBy) {
  auto x = TCollatedByGenerator<std::tuple<int64_t, int64_t>, int64_t, int64_t, int64_t>::New(
          [](const int64_t & carry, const int64_t & item) -> int64_t { return carry + item; },
          [](const int64_t & item) -> int64_t { return item; },
          TRangeGenerator::New(0, 3, true),
          0);
  int i = 0;
  for (auto cursor = MakeCursor(x); cursor; ++cursor) {
    auto result = *cursor;
    EXPECT_EQ(std::get<0>(result), i);
    EXPECT_EQ(std::get<1>(result), i);
    ++i;
  }  // for
}