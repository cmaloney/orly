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
using namespace Orly::CsvToBin;

using Strm::Mem::TStaticIn;

static const TLevel1::TOptions Simple = { ',', '\'', true };

FIXTURE(Typical) {
  Strm::Mem::TStaticIn mem("x");
  TLevel1 level1(&mem, Simple);
  TLevel2 level2(level1);
  TLevel3 level3(level2);
  bool x;
  level3
      >> StartOfFile >> StartOfRecord >> StartOfField
      >> x >> SkipBytes
      >> EndOfField >> EndOfRecord >> EndOfFile;
}
