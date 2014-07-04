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

static const TLevel1::TOptions Simple = { ',', '\'', true };

FIXTURE(OneLiner) {
  Strm::Mem::TStaticIn mem("true,false,1b4e28ba-2fa1-11d2-883f-b9a761bde3fb,'hello ''doctor'' name',-123,98.6");
  TLevel1 level1(&mem, Simple);
  TLevel2 level2(level1);
  TLevel3 level3(level2);
  bool a, b;
  TUuid c;
  string d;
  int64_t e;
  double f;
  level3
      >> StartOfFile >> StartOfRecord
      >> StartOfField >> a >> EndOfField
      >> StartOfField >> b >> EndOfField
      >> StartOfField >> c >> EndOfField
      >> StartOfField >> d >> EndOfField
      >> StartOfField >> e >> EndOfField
      >> StartOfField >> f >> EndOfField
      >> EndOfRecord >> EndOfFile;
  EXPECT_TRUE(a);
  EXPECT_FALSE(b);
  EXPECT_TRUE(c);
  EXPECT_EQ(d, "hello 'doctor' name");
  EXPECT_EQ(e, -123);
  EXPECT_EQ(f, 98.6);
}
