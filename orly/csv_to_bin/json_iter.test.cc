/* <orly/csv_to_bin/json_iter.test.cc>

   An input stream of JSON objects.

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

#include <orly/csv_to_bin/json_iter.h>

#include <sstream>

#include <test/kit.h>

using namespace std;
using namespace Orly::CsvToBin;

FIXTURE(Typical) {
  istringstream strm("{}\n{}\n{}\n");
  int count = 0;
  for (TJsonIter iter(strm); iter; ++iter, ++count);
  EXPECT_EQ(count, 3);
}
