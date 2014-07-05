/* <orly/csv_to_bin/level3.test.cc>

   Unit test for <orly/csv_to_bin/level3.h>.

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

#include <orly/csv_to_bin/level3.h>

#include <strm/mem/static_in.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Orly::CsvToBin;

using Strm::Mem::TStaticIn;

static const TLevel1::TOptions Simple = { ',', '\'', true, true, '\\', true };

FIXTURE(OneLiner) {
  Strm::Mem::TStaticIn mem(
      "true,false,1b4e28ba-2fa1-11d2-883f-b9a761bde3fb,"
      "'hello ''doctor'' name',-123,98.6,"
      "2014-07-04 04:03:09.102-0800,,101");
  TLevel1 level1(&mem, Simple);
  TLevel2 level2(level1);
  TLevel3 level3(level2);
  bool a, b;
  TUuid c;
  string d;
  int64_t e;
  double f;
  Chrono::TTimePnt g;
  Orly::Rt::TOpt<int64_t> h, i;
  level3
      >> StartOfFile >> StartOfRecord
      >> StartOfField >> a >> EndOfField
      >> StartOfField >> b >> EndOfField
      >> StartOfField >> c >> EndOfField
      >> StartOfField >> d >> EndOfField
      >> StartOfField >> e >> EndOfField
      >> StartOfField >> f >> EndOfField
      >> StartOfField >> g >> EndOfField
      >> StartOfField >> h >> EndOfField
      >> StartOfField >> i >> EndOfField
      >> EndOfRecord >> EndOfFile;
  EXPECT_TRUE(a);
  EXPECT_FALSE(b);
  EXPECT_TRUE(c);
  EXPECT_EQ(d, "hello 'doctor' name");
  EXPECT_EQ(e, -123);
  EXPECT_EQ(f, 98.6);
  EXPECT_TRUE(
      g == Chrono::CreateTimePnt(2014, 7, 4, 4, 3, 9, 102000000, -480));
  EXPECT_FALSE(h.IsKnown());
  EXPECT_TRUE(i.IsKnown());
  EXPECT_EQ(i.GetVal(), 101);
}

FIXTURE(Scanner) {
  Strm::Mem::TStaticIn mem(
      "   ,   ,   \n"
      "\n"
      ",");
  TLevel1 level1(&mem, Simple);
  TLevel2 level2(level1);
  TLevel3 level3(level2);
  level3 >> StartOfFile;
  size_t record_count = 0, field_count = 0;
  while (level3.AtRecord()) {
    level3 >> StartOfRecord;
    while (level3.AtField()) {
      level3 >> StartOfField;
      level3 >> SkipBytes >> EndOfField;
      ++field_count;
    }
    level3 >> EndOfRecord;
    ++record_count;
  }
  level3 >> EndOfFile;
  EXPECT_EQ(record_count, 3u);
  EXPECT_EQ(field_count, 6u);
}
