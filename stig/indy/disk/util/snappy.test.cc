/* <stig/indy/disk/util/snappy.test.cc>

   Unit test for <stig/indy/disk/util/snappy.h>.

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

#include <stig/indy/disk/util/snappy.h>

#include <stig/indy/disk/sim/mem_engine.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;

static const size_t BlockSize = 4096UL * 16;

TBufBlock::TPool TBufBlock::Pool(BlockSize, 256);

FIXTURE(Typical) {
  unique_ptr<TBufBlock> in_block(new TBufBlock());
  unique_ptr<TBufBlock> compressed_block(new TBufBlock());
  unique_ptr<TBufBlock> out_block(new TBufBlock());

  size_t *ptr = reinterpret_cast<size_t *>(in_block->GetData());
  for (size_t j = 0; j < PhysicalBlockSize / sizeof(size_t); ++j) {
    ptr[j] = j;
  }
  size_t compressed_length = 0UL;
  snappy::RawCompress(in_block->GetData(), BlockSize, compressed_block->GetData(), &compressed_length);
  cout << "compressed_length = " << compressed_length << endl;
  bool ret = snappy::RawUncompress(compressed_block->GetData(), compressed_length, out_block->GetData());
  assert(ret);
  EXPECT_TRUE(memcmp(in_block->GetData(), out_block->GetData(), BlockSize) == 0);
}