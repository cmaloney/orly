/* <stig/indy/disk/block_hit_counter.test.cc>

   Unit test for <stig/indy/disk/block_hit_counter.h>.

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

#include <stig/indy/disk/block_hit_counter.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Indy::Disk;

const size_t NumBlocks = 167772160UL; /* 10 TB */
const size_t BlockSize = 4096 * 16; /* 64K */

FIXTURE(Typical) {
  TBlockHitCounter counter(BlockSize, NumBlocks);
  counter.AddHits(0, 4);
  EXPECT_EQ(counter.GetNumHits(0), floor(log(4)));
}

FIXTURE(ManySmall) {
  TBlockHitCounter counter(BlockSize, NumBlocks);
  for (size_t i = 0; i < 100000; ++i) {
    counter.AddHits(0, 1);
  }
  EXPECT_EQ(counter.GetNumHits(0), floor(log(1)));
}

FIXTURE(OneLarge) {
  size_t hits = 1000;
  TBlockHitCounter counter(BlockSize, NumBlocks);
  counter.AddHits(0, hits);
  EXPECT_EQ(counter.GetNumHits(0), floor(log(hits)));
}

FIXTURE(ManyLarge) {
  size_t hits = 10000;
  TBlockHitCounter counter(BlockSize, NumBlocks);
  uint8_t expected = 0;
  for (size_t i = 0; i < 10000; ++i) {
    counter.AddHits(0, hits);
    expected = floor(log(exp(expected) + hits));
  }
  EXPECT_EQ(counter.GetNumHits(0), expected);
}

FIXTURE(Reset) {
  size_t hits = 1000;
  TBlockHitCounter counter(BlockSize, NumBlocks);
  counter.AddHits(0, hits);
  EXPECT_EQ(counter.GetNumHits(0), floor(log(hits)));
  counter.Reset(0);
  EXPECT_EQ(counter.GetNumHits(0), 0);
}