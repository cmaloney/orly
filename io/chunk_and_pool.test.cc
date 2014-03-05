/* <io/chunk_and_pool.test.cc>

   Unit test for <io/chunk_and_pool.h>.

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

#include <io/chunk_and_pool.h>

#include <cstring>

#include <test/kit.h>

using namespace std;
using namespace Io;

FIXTURE(Growth) {
  auto pool = make_shared<TPool>();
  EXPECT_EQ(pool->GetFreeChunkCount(), 0U);
  auto chunk = pool->AcquireChunk();
  EXPECT_EQ(pool->GetFreeChunkCount(), 0U);
  chunk.reset();
  EXPECT_EQ(pool->GetFreeChunkCount(), 1U);
}

FIXTURE(NoGrowth) {
  auto pool = make_shared<TPool>(TPool::TArgs(TPool::DefaultChunkSize, 1, 0));
  EXPECT_EQ(pool->GetFreeChunkCount(), 1U);
  auto chunk = pool->AcquireChunk();
  EXPECT_EQ(pool->GetFreeChunkCount(), 0U);
  bool caught;
  try {
    auto bad = pool->AcquireChunk();
    caught = false;
  } catch (const TPool::TOutOfChunksError &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
  chunk.reset();
  EXPECT_EQ(pool->GetFreeChunkCount(), 1U);
}

FIXTURE(ExtraGrowth) {
  auto pool = make_shared<TPool>(TPool::TArgs(TPool::DefaultChunkSize, 0, 10));
  EXPECT_EQ(pool->GetFreeChunkCount(), 0U);
  auto chunk = pool->AcquireChunk();
  EXPECT_EQ(pool->GetFreeChunkCount(), 9U);
  chunk.reset();
  EXPECT_EQ(pool->GetFreeChunkCount(), 10U);
}

FIXTURE(NonRecycled) {
  static const char *expected_str = "mofo";
  const size_t expected_size = strlen(expected_str);
  static const size_t buffer_size = 100;
  char buffer[buffer_size];
  auto pool = make_shared<TPool>(TPool::TArgs(TPool::DefaultChunkSize, 0, 0));
  pool->EnqueueChunk(new TChunk(TChunk::Empty, buffer, buffer + buffer_size));
  auto chunk = pool->AcquireChunk();
  const char *csr = expected_str;
  size_t size = expected_size;
  EXPECT_TRUE(chunk->Store(csr, size));
  EXPECT_EQ(csr, expected_str + expected_size);
  EXPECT_FALSE(size);
  buffer[expected_size] = '\0';
  const char *start, *limit;
  chunk->GetData(start, limit);
  EXPECT_LE(start, limit);
  EXPECT_EQ(start, buffer);
  EXPECT_EQ(limit, buffer + expected_size);
  EXPECT_TRUE(strcmp(buffer, expected_str) == 0);
  EXPECT_EQ(pool->GetFreeChunkCount(), 0U);
}
