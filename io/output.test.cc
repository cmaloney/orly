/* <io/output.test.cc>

   Unit test for <io/output_consumer.h> and <io/output_producer.h>.

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

#include <io/output_consumer.h>
#include <io/output_producer.h>

#include <cstring>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Io;

class TMyConsumer
    : public TOutputConsumer {
  NO_COPY_SEMANTICS(TMyConsumer);
  public:

  typedef vector<shared_ptr<const TChunk>> TChunks;

  TMyConsumer() {}

  virtual void ConsumeOutput(const shared_ptr<const TChunk> &chunk) {
    Chunks.push_back(chunk);
  }

  const TChunks &GetChunks() const {
    return Chunks;
  }

  private:

  TChunks Chunks;

};

class TMyProducer
    : public TOutputProducer {
  NO_COPY_SEMANTICS(TMyProducer);
  public:

  TMyProducer(const shared_ptr<TOutputConsumer> &consumer)
      : TOutputProducer(consumer, make_shared<TPool>()) {}

  using TOutputProducer::Flush;
  using TOutputProducer::WriteExactly;

};

FIXTURE(Typical) {
  auto cons = make_shared<TMyConsumer>();
  auto prod = make_shared<TMyProducer>(cons);
  static const char *expected = "hello";
  static const size_t expected_size = strlen(expected);
  prod->WriteExactly(expected, expected_size);
  prod->Flush();
  const TMyConsumer::TChunks &chunks = cons->GetChunks();
  EXPECT_EQ(chunks.size(), 1U);
  const char *start, *limit;
  chunks[0]->GetData(start, limit);
  size_t actual_size = limit - start;
  EXPECT_EQ(actual_size, expected_size);
  EXPECT_FALSE(strncmp(start, expected, expected_size));
}
