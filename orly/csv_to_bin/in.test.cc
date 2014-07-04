/* <orly/csv_to_bin/in.test.cc>

   Unit test for <orly/csv_to_bin/in.h>.

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

#include <orly/csv_to_bin/in.h>

#include <strm/mem/static_in.h>
#include <test/kit.h>

using namespace std;
using namespace Orly::CsvToBin;

using Strm::Mem::TStaticIn;

static const TIn::TOptions Simple = { ',', '\'', true };

FIXTURE(Empty) {
  TStaticIn mem;
  TIn in(&mem);
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(AlmostEmpty) {
  TStaticIn mem("x");
  TIn in(&mem);
  EXPECT_TRUE(in->Kind == TIn::Byte);
  EXPECT_TRUE(in->Byte == u'x');
  ++in;
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(JustDelim) {
  TStaticIn mem(",");
  TIn in(&mem);
  EXPECT_TRUE(in->Kind == TIn::EndOfField);
  ++in;
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(JustEol) {
  TStaticIn mem("\r\n");
  TIn in(&mem);
  EXPECT_TRUE(in->Kind == TIn::EndOfRecord);
  ++in;
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(JustUnixEol) {
  TStaticIn mem("\n");
  TIn in(&mem, Simple);
  EXPECT_TRUE(in->Kind == TIn::EndOfRecord);
  ++in;
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(QuotedDelim) {
  TStaticIn mem("','");
  TIn in(&mem, Simple);
  EXPECT_TRUE(in->Kind == TIn::Byte);
  EXPECT_TRUE(in->Byte == u',');
  ++in;
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(QuotedEol) {
  TStaticIn mem("'\r\n'");
  TIn in(&mem, Simple);
  EXPECT_TRUE(in->Kind == TIn::Byte);
  EXPECT_TRUE(in->Byte == u'\r');
  ++in;
  EXPECT_TRUE(in->Byte == u'\n');
  ++in;
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(QuotedUnixEol) {
  TStaticIn mem("'\n'");
  TIn in(&mem, Simple);
  EXPECT_TRUE(in->Kind == TIn::Byte);
  EXPECT_TRUE(in->Byte == u'\n');
  ++in;
  EXPECT_TRUE(in->Kind == TIn::EndOfFile);
}

FIXTURE(Typical) {
  vector<vector<string>> table;
  TStaticIn mem("hello,'doctor,name',\ncontinue,'''yesterday''',tomorrow");
  TIn in(&mem, Simple);
  bool keep_going = true;
  do {
    switch (in->Kind) {
      case TIn::Byte: {
        if (table.empty()) {
          table.push_back(vector<string>());
        }
        auto &record = table.back();
        if (record.empty()) {
          record.push_back(string());
        }
        auto &field = record.back();
        field += in->Byte;
        break;
      }
      case TIn::EndOfField: {
        if (table.empty()) {
          table.push_back(vector<string>());
        }
        auto &record = table.back();
        record.push_back(string());
        break;
      }
      case TIn::EndOfRecord: {
        table.push_back(vector<string>());
        break;
      }
      case TIn::EndOfFile: {
        keep_going = false;
        break;
      }
    }
    ++in;
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
      EXPECT_EQ(table[1][2], "tomorrow");
    }
  }
}
