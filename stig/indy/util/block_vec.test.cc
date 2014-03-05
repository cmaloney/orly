/* <stig/indy/util/block_vec.test.cc>

   Unit test for <stig/indy/util/block_vec.h>.

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

#include <stig/indy/util/block_vec.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Indy::Util;

void ValidateVector(const std::vector<size_t> &expected_block_vec) {
  const size_t num_elems = expected_block_vec.size();
  TBlockVec block_vec;
  EXPECT_EQ(block_vec.Size(), 0UL);
  for (size_t i = 0; i < num_elems; ++i) {
    block_vec.PushBack(expected_block_vec[i]);
    EXPECT_EQ(block_vec.Size(), i + 1UL);
    EXPECT_EQ(block_vec[i], expected_block_vec[i]);
  }
}

void CheckVec(const TBlockVec &block_vec, const std::vector<size_t> &expected_block_vec) {
  const size_t num_elems = expected_block_vec.size();
  EXPECT_EQ(block_vec.Size(), num_elems);
  for (size_t i = 0; i < num_elems; ++i) {
    EXPECT_EQ(block_vec[i], expected_block_vec[i]);
  }
}

void CheckCursor(const TBlockVec &block_vec, const std::vector<size_t> &expected_block_vec) {
  auto expected_iter = expected_block_vec.begin();
  for (size_t block_id : block_vec) {
    EXPECT_TRUE(expected_iter != expected_block_vec.end());
    EXPECT_EQ(block_id, *expected_iter);
    ++expected_iter;
  }
}

void ValidateCursor(const std::vector<size_t> &expected_block_vec) {
  const size_t num_elems = expected_block_vec.size();
  TBlockVec block_vec;
  for (size_t i = 0; i < num_elems; ++i) {
    block_vec.PushBack(expected_block_vec[i]);
  }
  CheckCursor(block_vec, expected_block_vec);
}

void Push(size_t block_id, TBlockVec &block_vec, std::vector<size_t> &expected_vec) {
  block_vec.PushBack(block_id);
  expected_vec.push_back(block_id);
}

void Push(std::pair<size_t, size_t> block_range, TBlockVec &block_vec, std::vector<size_t> &expected_vec) {
  block_vec.PushBack(block_range);
  for (size_t i = 0; i < block_range.second; ++i) {
    expected_vec.push_back(block_range.first + i);
  }
}

void Trim(size_t n, TBlockVec &block_vec, std::vector<size_t> &expected_vec) {
  block_vec.Trim(n);
  expected_vec.erase(expected_vec.begin() + expected_vec.size() - n, expected_vec.end());
}

void Check(const TBlockVec &block_vec, const std::vector<size_t> &expected_block_vec) {
  CheckVec(block_vec, expected_block_vec);
  CheckCursor(block_vec, expected_block_vec);
}

FIXTURE(Typical) {
  // all sequential
  ValidateVector(std::vector<size_t>{7UL, 8UL, 9UL});
  // multi-sequential
  ValidateVector(std::vector<size_t>{7UL, 8UL, 11UL, 12UL, 13UL, 15UL});
  // reverse-sequential
  ValidateVector(std::vector<size_t>{10UL, 9UL, 8UL, 7UL});
}

FIXTURE(Iterate) {
  // all sequential
  ValidateCursor(std::vector<size_t>{7UL, 8UL, 9UL});
  // multi-sequential
  ValidateCursor(std::vector<size_t>{7UL, 8UL, 11UL, 12UL, 13UL, 15UL});
  // reverse-sequential
  ValidateCursor(std::vector<size_t>{10UL, 9UL, 8UL, 7UL});
}

FIXTURE(Trim) {
  std::vector<size_t> expected_vec;
  TBlockVec block_vec;
  Check(block_vec, expected_vec);
  Push(make_pair(7UL, 3UL), block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Trim(1, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Trim(1, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Push(8UL, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Push(9UL, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Trim(1, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Push(9UL, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Push(10UL, block_vec, expected_vec);
  Push(make_pair(13UL, 2UL), block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Trim(1, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Push(14UL, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Trim(2, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Push(make_pair(13UL, 2UL), block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Trim(3, block_vec, expected_vec);
  Check(block_vec, expected_vec);
  Trim(2, block_vec, expected_vec);
  Check(block_vec, expected_vec);
}

void ValidateSubRanges(const std::vector<size_t> &expected_block_vec) {
  const size_t num_elems = expected_block_vec.size();
  TBlockVec block_vec;
  EXPECT_EQ(block_vec.Size(), 0UL);
  for (size_t i = 0; i < num_elems; ++i) {
    block_vec.PushBack(expected_block_vec[i]);
    EXPECT_EQ(block_vec[i], expected_block_vec[i]);
  }
  for (size_t from = 0; from < block_vec.Size(); ++from) {
    for (size_t to = from; to < block_vec.Size(); ++to) {
      auto iter = expected_block_vec.begin() + from;
      const auto end = expected_block_vec.begin() + to;
      block_vec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
        for (size_t i = 0; i < num_blocks; ++i) {
          assert(iter != end);
          EXPECT_EQ(block_id + i, *iter);
          ++iter;
        }
        return true;
      }, from, to);
      EXPECT_TRUE(iter == end);
    }
  }
}

FIXTURE(ForEachRangeInRange) {
  ValidateSubRanges(std::vector<size_t>{});
  ValidateSubRanges(std::vector<size_t>{7UL});
  ValidateSubRanges(std::vector<size_t>{7UL, 8UL});
  ValidateSubRanges(std::vector<size_t>{7UL, 8UL, 9UL});
  ValidateSubRanges(std::vector<size_t>{7UL, 8UL, 9UL, 11UL, 12UL});
  ValidateSubRanges(std::vector<size_t>{7UL, 8UL, 9UL, 11UL, 12UL, 15UL});
}
