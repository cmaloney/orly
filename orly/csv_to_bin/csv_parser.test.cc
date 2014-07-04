/* <orly/csv_to_bin/csv_parser.test.cc>

   Unit test for <orly/csv_to_bin/csv_parser.h>.

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

#include <orly/csv_to_bin/csv_parser.h>

#include <string>
#include <vector>

#include <base/class_traits.h>
#include <strm/mem/static_in.h>
#include <test/kit.h>

using namespace std;
using namespace Orly::CsvToBin;

using Strm::Mem::TStaticIn;

using TTable = vector<vector<string>>;

static const TIn::TOptions Simple = { ',', '\'', true };

/* TODO */
class TTableBuilder final
    : public TCsvParser {
  NO_COPY(TTableBuilder);
  public:

  /* TODO */
  TTableBuilder(TTable &table)
      : Table(table) {}

  private:

  /* TODO */
  virtual void OnRecordStart() override {
    Table.push_back(vector<string>());
  }

  /* TODO */
  virtual void OnFieldStart() override {
    Table.back().push_back(string());
  }

  /* TODO */
  virtual void OnByte(uint8_t byte) override {
    Table.back().back() += byte;
  }

  TTable &Table;

};  // TTableBuilder

/* TODO */
void Parse(TTable &table, const char *text) {
  Strm::Mem::TStaticIn mem(text);
  TIn strm(&mem, Simple);
  TTableBuilder(table).Parse(strm);
}

FIXTURE(Empty) {
  TTable table;
  Parse(table, "");
  EXPECT_EQ(table.size(), 0u);
}

FIXTURE(AlmostEmpty) {
  TTable table;
  Parse(table, "x");
  if (EXPECT_EQ(table.size(), 1u)) {
    if (EXPECT_EQ(table[0].size(), 1u)) {
      EXPECT_EQ(table[0][0], "x");
    }
  }
}

FIXTURE(OneBadlyWrittenEmptyRecord) {
  TTable table;
  Parse(table, "\n");
  if (EXPECT_EQ(table.size(), 1u)) {
    EXPECT_EQ(table[0].size(), 1u);
  }
}

FIXTURE(SeveralBadlyWrittenEmptyRecords) {
  TTable table;
  Parse(table, "\n\n\n");
  if (EXPECT_EQ(table.size(), 3u)) {
    EXPECT_EQ(table[0].size(), 1u);
    EXPECT_EQ(table[1].size(), 1u);
    EXPECT_EQ(table[2].size(), 1u);
  }
}

FIXTURE(EmptinessGalore) {
  TTable table;
  Parse(table, ",,\n,,\n,,");
  if (EXPECT_EQ(table.size(), 3u)) {
    EXPECT_EQ(table[0].size(), 3u);
    EXPECT_EQ(table[1].size(), 3u);
    EXPECT_EQ(table[2].size(), 3u);
  }
}

FIXTURE(Typical) {
  TTable table;
  Parse(table, "hello,'doctor,name',\ncontinue,'''yesterday''',tomorrow\n");
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
