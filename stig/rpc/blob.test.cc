/* <stig/rpc/blob.test.cc> 

   Unit test for <stig/rpc/blob.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/blob.h>

#include <cstring>
#include <sstream>
#include <string>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Rpc;

static const char
    *Str1 = "hello",
    *Str2 = "doctor";

static const size_t
    Str1Size = strlen(Str1),
    Str2Size = strlen(Str2);

/* Dump a blob into a std string. */
static string ToString(const TBlob &blob) {
  assert(&blob);
  ostringstream strm;
  blob.Dump(strm);
  return strm.str();
}

FIXTURE(Dump) {
  /* Dump an empty blob, get an empty string. */
  TBlob a;
  EXPECT_EQ(ToString(a), "");
  /* Dump a blob with a string in it, get the string back. */
  TBufferPool buffer_pool;
  TBlob::TWriter writer(&buffer_pool);
  writer.Write(Str1, Str1Size);
  a = writer.DraftBlob();
  EXPECT_EQ(ToString(a), Str1);
}

FIXTURE(WriteAndRead) {
  /* Write a blob. */
  TBufferPool buffer_pool;
  TBlob::TWriter writer(&buffer_pool);
  writer.Write(Str1, Str1Size);
  TBlob blob = writer.DraftBlob();
  /* Examine the counts of the blob. */
  size_t buffer_count, byte_count;
  blob.GetCounts(buffer_count, byte_count);
  EXPECT_EQ(buffer_count, 1u);
  EXPECT_EQ(byte_count, Str1Size);
  /* Read the blob. */
  TBlob::TReader reader(&blob);
  EXPECT_TRUE(reader);
  char str[Str1Size + 1];
  reader.Read(str, Str1Size);
  EXPECT_FALSE(reader);
  str[Str1Size] = '\0';
  EXPECT_EQ(strcmp(str, Str1), 0);
}

FIXTURE(Eq) {
  /* Two empty blobs are equal. */
  TBlob a, b;
  EXPECT_TRUE(a == b);
  /* Two blobs containing the same data are equal. */
  TBufferPool buffer_pool;
  TBlob::TWriter writer(&buffer_pool);
  writer.Write(Str1, Str1Size);
  a = writer.DraftBlob();
  writer.Write(Str1, Str1Size);
  b = writer.DraftBlob();
  EXPECT_EQ(a, b);
  /* An empty and a non-empty blob are not equal. */
  a.Reset();
  EXPECT_NE(a, b);
  /* Two non-empty blobs containing different data are not equal. */
  const char *Str2 = "doctor";
  const size_t Str2Size = strlen(Str2);
  writer.Write(Str2, Str2Size);
  a = writer.DraftBlob();
  EXPECT_NE(a, b);
}

FIXTURE(JunkBlobs) {
  static const size_t
      size_count = 6,
      sizes[size_count] = { 0, BlockSize / 2, BlockSize - 1, BlockSize, BlockSize + 1, BlockSize * 2 };
  TBufferPool buffer_pool;
  for (size_t i = 0; i < size_count; ++i) {
    size_t expected_byte_count = sizes[i];
    TBlob blob(&buffer_pool, expected_byte_count);
    size_t buffer_count, actual_byte_count;
    blob.GetCounts(buffer_count, actual_byte_count);
    EXPECT_EQ(buffer_count, (expected_byte_count + BlockSize - 1) / BlockSize);
    EXPECT_EQ(actual_byte_count, expected_byte_count);
  }
}

