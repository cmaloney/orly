/* <orly/csv_to_bin/level1.test.cc>

   Unit test for <orly/csv_to_bin/level1.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except strm compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to strm writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/csv_to_bin/level1.h>

#include <strm/mem/static_in.h>
#include <test/kit.h>

using namespace std;
using namespace Orly::CsvToBin;

using Strm::Mem::TStaticIn;

static const TLevel1::TOptions Simple = { ',', '\'', true, true, '\\', true };

FIXTURE(Empty) {
  TStaticIn mem;
  TLevel1 strm(&mem);
  EXPECT_TRUE(strm->State == TLevel1::EndOfFile);
}

FIXTURE(AlmostEmpty) {
  TStaticIn mem("x");
  TLevel1 strm(&mem);
  EXPECT_TRUE(strm->State == TLevel1::Byte);
  EXPECT_TRUE(strm->Byte == u'x');
  ++strm;
  EXPECT_TRUE(strm->State == TLevel1::EndOfFile);
}

FIXTURE(JustDelim) {
  TStaticIn mem(",");
  TLevel1 strm(&mem);
  EXPECT_TRUE(strm->State == TLevel1::EndOfField);
  ++strm;
  EXPECT_TRUE(strm->State == TLevel1::EndOfFile);
}

FIXTURE(JustEol) {
  TStaticIn mem("\n");
  TLevel1 strm(&mem, Simple);
  EXPECT_TRUE(strm->State == TLevel1::EndOfRecord);
  ++strm;
  EXPECT_TRUE(strm->State == TLevel1::EndOfFile);
}

FIXTURE(QuotedDelim) {
  TStaticIn mem("','");
  TLevel1 strm(&mem, Simple);
  EXPECT_TRUE(strm->State == TLevel1::Byte);
  EXPECT_TRUE(strm->Byte == u',');
  ++strm;
  EXPECT_TRUE(strm->State == TLevel1::EndOfFile);
}

FIXTURE(QuotedEol) {
  TStaticIn mem("'\r\n'");
  TLevel1 strm(&mem, Simple);
  EXPECT_TRUE(strm->State == TLevel1::Byte);
  EXPECT_TRUE(strm->Byte == u'\r');
  ++strm;
  EXPECT_TRUE(strm->Byte == u'\n');
  ++strm;
  EXPECT_TRUE(strm->State == TLevel1::EndOfFile);
}

FIXTURE(QuotedUnixEol) {
  TStaticIn mem("'\n'");
  TLevel1 strm(&mem, Simple);
  EXPECT_TRUE(strm->State == TLevel1::Byte);
  EXPECT_TRUE(strm->Byte == u'\n');
  ++strm;
  EXPECT_TRUE(strm->State == TLevel1::EndOfFile);
}

FIXTURE(Typical) {
  vector<vector<string>> table;
  TStaticIn mem(
      "hello,'doctor,name',\n"
      "continue,'''yesterday''','\\'tomorrow\\''");
  TLevel1 strm(&mem, Simple);
  bool keep_going = true;
  do {
    switch (strm->State) {
      case TLevel1::Byte: {
        if (table.empty()) {
          table.push_back(vector<string>());
        }
        auto &record = table.back();
        if (record.empty()) {
          record.push_back(string());
        }
        auto &field = record.back();
        field += strm->Byte;
        break;
      }
      case TLevel1::EndOfField: {
        if (table.empty()) {
          table.push_back(vector<string>());
        }
        auto &record = table.back();
        record.push_back(string());
        break;
      }
      case TLevel1::EndOfRecord: {
        table.push_back(vector<string>());
        break;
      }
      case TLevel1::EndOfFile: {
        keep_going = false;
        break;
      }
    }
    ++strm;
  } while (keep_going);
  if (EXPECT_EQ(table.size(), 2u)) {
    if (EXPECT_EQ(table[0].size(), 3u)) {
      EXPECT_EQ(table[0][0], "hello");
      EXPECT_EQ(table[0][1], "doctor,name");
      EXPECT_EQ(table[0][2], "");
    }
    if (EXPECT_EQ(table[1].size(), 3u)) {
      EXPECT_EQ(table[1][0], "continue");
      EXPECT_EQ(table[1][1], "'yesterday'");
      EXPECT_EQ(table[1][2], "'tomorrow'");
    }
  }
}
