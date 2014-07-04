/* <orly/csv_to_bin/level2.test.cc>

   Unit test for <orly/csv_to_bin/level2.h>.

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

#include <orly/csv_to_bin/level2.h>

#include <string>
#include <vector>

#include <strm/mem/static_in.h>
#include <test/kit.h>

using namespace std;
using namespace Orly::CsvToBin;

using Strm::Mem::TStaticIn;

using TTable = vector<vector<string>>;

static const TLevel1::TOptions Simple = { ',', '\'', true };

static void BuildTable(TTable &table, const char *text) {
  Strm::Mem::TStaticIn mem(text);
  TLevel1 level1(&mem, Simple);
  TLevel2 level2(level1);
  bool keep_going = true;
  do {
    switch (level2->State) {
      case TLevel2::StartOfFile: {
        ++level2;
        break;
      }
      case TLevel2::StartOfRecord: {
        table.push_back(vector<string>());
        ++level2;
        break;
      }
      case TLevel2::StartOfField: {
        table.back().push_back(string());
        ++level2;
        break;
      }
      case TLevel2::Bytes: {
        table.back().back().append(level2->Start, level2->Limit);
        ++level2;
        break;
      }
      case TLevel2::EndOfField: {
        ++level2;
        break;
      }
      case TLevel2::EndOfRecord: {
        ++level2;
        break;
      }
      case TLevel2::EndOfFile: {
        keep_going = false;
        break;
      }
    }
  } while (keep_going);
}

FIXTURE(Empty) {
  TTable table;
  BuildTable(table, "");
  EXPECT_EQ(table.size(), 0u);
}

FIXTURE(AlmostEmpty) {
  TTable table;
  BuildTable(table, "x");
  if (EXPECT_EQ(table.size(), 1u)) {
    if (EXPECT_EQ(table[0].size(), 1u)) {
      EXPECT_EQ(table[0][0], "x");
    }
  }
}

FIXTURE(OneBadlyWrittenEmptyRecord) {
  TTable table;
  BuildTable(table, "\n");
  if (EXPECT_EQ(table.size(), 1u)) {
    EXPECT_EQ(table[0].size(), 1u);
  }
}

FIXTURE(SeveralBadlyWrittenEmptyRecords) {
  TTable table;
  BuildTable(table, "\n\n\n");
  if (EXPECT_EQ(table.size(), 3u)) {
    EXPECT_EQ(table[0].size(), 1u);
    EXPECT_EQ(table[1].size(), 1u);
    EXPECT_EQ(table[2].size(), 1u);
  }
}

FIXTURE(EmptinessGalore) {
  TTable table;
  BuildTable(table, ",,\n,,\n,,");
  if (EXPECT_EQ(table.size(), 3u)) {
    EXPECT_EQ(table[0].size(), 3u);
    EXPECT_EQ(table[1].size(), 3u);
    EXPECT_EQ(table[2].size(), 3u);
  }
}

FIXTURE(Typical) {
  TTable table;
  BuildTable(
      table,
      "hello,'doctor,name',\ncontinue,'''yesterday''',tomorrow\n");
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
