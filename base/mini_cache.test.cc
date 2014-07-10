/* <base/mini_cache.test.cc>

   Unit test for <base/mini_cache.h>.

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

#include <base/mini_cache.h>

#include <test/kit.h>

using namespace Base;

FIXTURE(Typical) {
  const int64_t num_iter = 1000;
  using TKey = int64_t;
  using TVal = int64_t;
  TMiniCache<64, TKey, TVal> cache;
  for (int64_t i = 0; i < num_iter; ++i) {
    cache.Emplace(std::forward_as_tuple(i), std::forward_as_tuple(i * 10));
    const TVal *res = cache.TryGet(i);
    EXPECT_TRUE(res);
    EXPECT_EQ(*res, i * 10);
  }
}

