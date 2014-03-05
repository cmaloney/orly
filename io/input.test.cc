/* <io/input.test.cc>

   Unit test for <io/input_producer.h> and <io/input_consumer.h>.

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

#include <io/input_consumer.h>
#include <io/input_producer.h>

#include <cstring>
#include <queue>
#include <vector>

#include <io/chunk_and_pool.h>
#include <test/kit.h>

using namespace std;
using namespace Io;

class TMyProducer
    : public TInputProducer {
  NO_COPY_SEMANTICS(TMyProducer);
  public:

  TMyProducer(const vector<const char *> &msgs) {
    for (auto msg: msgs) {
      Push(msg);
    }
  }

  size_t GetChunkCount() const {
    return Chunks.size();
  }

  void Push(const char *msg) {
    Chunks.push(make_shared<TChunk>(TChunk::Full, msg));
  }

  virtual shared_ptr<const TChunk> TryProduceInput() {
    shared_ptr<const TChunk> chunk;
    if (!Chunks.empty()) {
      chunk = Chunks.front();
      Chunks.pop();
    }
    return chunk;
  }

  private:

  queue<shared_ptr<const TChunk>> Chunks;

};

class TMyConsumer
    : public TInputConsumer {
  NO_COPY_SEMANTICS(TMyConsumer);
  public:

  TMyConsumer(const shared_ptr<TInputProducer> &input_producer)
      : TInputConsumer(input_producer) {}

  using TInputConsumer::GetPeekSize;
  using TInputConsumer::Peek;
  using TInputConsumer::ReadExactly;
  using TInputConsumer::SkipExactly;
  using TInputConsumer::TryPeek;

};

FIXTURE(SimpleRead) {
  static const char *expected = "hello";
  static const size_t size = strlen(expected);
  auto prod = make_shared<TMyProducer>(vector<const char *>({ expected }));
  auto cons = make_shared<TMyConsumer>(prod);
  EXPECT_EQ(prod->GetChunkCount(), 1U);
  EXPECT_FALSE(cons->IsAtEnd());
  EXPECT_EQ(cons->GetPeekSize(), size);
  EXPECT_FALSE(strncmp(cons->Peek(), expected, size));
  char actual[size + 1];
  cons->ReadExactly(actual, size);
  actual[size] = '\0';
  EXPECT_FALSE(strcmp(actual, expected));
  EXPECT_TRUE(cons->IsAtEnd());
  EXPECT_EQ(cons->GetPeekSize(), 0U);
  EXPECT_FALSE(cons->TryPeek());
  EXPECT_EQ(prod->GetChunkCount(), 0U);
}

FIXTURE(SpanningRead) {
  vector<const char *> parts = { "hello", "doctor", "name" };
  vector<size_t> sizes(parts.size());
  size_t total_size = 0;
  for (size_t i = 0; i < parts.size(); ++i) {
    size_t size = strlen(parts[i]);
    sizes[i] = size;
    total_size += size;
  }
  auto prod = make_shared<TMyProducer>(parts);
  auto cons = make_shared<TMyConsumer>(prod);
  EXPECT_EQ(prod->GetChunkCount(), parts.size());
  EXPECT_FALSE(cons->IsAtEnd());
  EXPECT_EQ(cons->GetPeekSize(), sizes[0]);
  EXPECT_FALSE(strncmp(cons->Peek(), parts[0], sizes[0]));
  char actual[total_size + 1];
  cons->ReadExactly(actual, total_size);
  char *csr = actual;
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_FALSE(strncmp(csr, parts[i], sizes[i]));
    csr += sizes[i];
  }
  EXPECT_TRUE(cons->IsAtEnd());
  EXPECT_EQ(cons->GetPeekSize(), 0U);
  EXPECT_FALSE(cons->TryPeek());
  EXPECT_EQ(prod->GetChunkCount(), 0U);
}

FIXTURE(ChunkDeletion) {
  vector<const char *> parts = { "hello", "doctor", "name" };
  vector<size_t> sizes(parts.size());
  size_t total_size = 0;
  for (size_t i = 0; i < parts.size(); ++i) {
    size_t size = strlen(parts[i]);
    sizes[i] = size;
    total_size += size;
  }
  auto prod = make_shared<TMyProducer>(parts);
  auto cons = make_shared<TMyConsumer>(prod);
  EXPECT_EQ(prod->GetChunkCount(), parts.size());
  EXPECT_EQ(cons->GetCachedChunkCount(), 0U);
  for (size_t i = 0; i < parts.size(); ++i) {
    cons->TryPeek();
    EXPECT_EQ(prod->GetChunkCount(), parts.size() - (i + 1));
    EXPECT_EQ(cons->GetCachedChunkCount(), 1U);
    cons->SkipExactly(sizes[i]);
  }
  cons->TryPeek();
  EXPECT_EQ(prod->GetChunkCount(), 0U);
  EXPECT_EQ(cons->GetCachedChunkCount(), 0U);
}

FIXTURE(ChunkRetention) {
  vector<const char *> parts = { "hello", "doctor", "name" };
  vector<size_t> sizes(parts.size());
  size_t total_size = 0;
  for (size_t i = 0; i < parts.size(); ++i) {
    size_t size = strlen(parts[i]);
    sizes[i] = size;
    total_size += size;
  }
  auto prod = make_shared<TMyProducer>(parts);
  auto cons = make_shared<TMyConsumer>(prod);
  /* extra */ {
    TMark mark(cons.get());
    for (size_t i = 0; i < parts.size(); ++i) {
      cons->TryPeek();
      EXPECT_EQ(prod->GetChunkCount(), parts.size() - (i + 1));
      EXPECT_EQ(cons->GetCachedChunkCount(), i + 1);
      cons->SkipExactly(sizes[i]);
    }
    cons->TryPeek();
    EXPECT_EQ(prod->GetChunkCount(), 0U);
    EXPECT_EQ(cons->GetCachedChunkCount(), parts.size());
  }
  cons->TryPeek();
  EXPECT_EQ(prod->GetChunkCount(), 0U);
  EXPECT_EQ(cons->GetCachedChunkCount(), 0U);
}

FIXTURE(Rewind) {
  vector<const char *> parts = { "hello", "doctor", "name" };
  vector<size_t> sizes(parts.size());
  size_t total_size = 0;
  for (size_t i = 0; i < parts.size(); ++i) {
    size_t size = strlen(parts[i]);
    sizes[i] = size;
    total_size += size;
  }
  auto prod = make_shared<TMyProducer>(parts);
  auto cons = make_shared<TMyConsumer>(prod);
  /* extra */ {
    TMark mark(cons.get());
    for (size_t i = 0; i < parts.size(); ++i) {
      if (EXPECT_EQ(cons->GetPeekSize(), sizes[i])) {
        EXPECT_FALSE(strncmp(cons->TryPeek(), parts[i], sizes[i]));
      }
      cons->SkipExactly(sizes[i]);
    }
    EXPECT_EQ(cons->GetPeekSize(), 0U);
    EXPECT_TRUE(cons->IsAtEnd());
    mark.Rewind();
    EXPECT_FALSE(cons->IsAtEnd());
  }
  for (size_t i = 0; i < parts.size(); ++i) {
    if (EXPECT_EQ(cons->GetPeekSize(), sizes[i])) {
      EXPECT_FALSE(strncmp(cons->TryPeek(), parts[i], sizes[i]));
    }
    cons->SkipExactly(sizes[i]);
  }
  EXPECT_TRUE(cons->IsAtEnd());
  EXPECT_EQ(cons->GetPeekSize(), 0U);
}
