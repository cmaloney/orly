/* <base/sigma_calc.test.cc>

   Unit test for <base/sigma_calc.h>.

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

#include <base/sigma_calc.h>

#include <cmath>
#include <sstream>

#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(Simple) {
  constexpr size_t val_count = 5;
  constexpr int    vals[val_count] = { 4, 2, 5, 8, 6 };
  TSigmaCalc calc;
  for (size_t i = 0; i < val_count; ++i) {
    calc.Push(vals[i]);
  }
  double min, max, mean, sigma;
  if (EXPECT_EQ(calc.Report(min, max, mean, sigma), 5u)) {
    EXPECT_EQ(min, 2.0);
    EXPECT_EQ(max, 8.0);
    EXPECT_EQ(mean, 5.0);
    EXPECT_EQ(sigma, sqrt(5.0));
  }
}

FIXTURE(Large) {
  constexpr size_t count = 100000000;  // 100 million
  TSigmaCalc calc;
  for (size_t i = 0; i < count; ++i) {
    calc.Push(100);
    calc.Push(102);
  }
  double min, max, mean, sigma;
  if (EXPECT_EQ(calc.Report(min, max, mean, sigma), count * 2)) {
    EXPECT_EQ(min, 100.0);
    EXPECT_EQ(max, 102.0);
    EXPECT_EQ(mean, 101.0);
    EXPECT_EQ(static_cast<int>(sigma), 1);
  }
}

FIXTURE(Write) {
  TSigmaCalc calc;
  ostringstream strm;
  strm << calc;
  EXPECT_EQ(strm.str(), "(count: 0)");
  calc.Push(1);
  strm.str("");
  strm << calc;
  EXPECT_EQ(strm.str(), "(count: 1, min: 1, max: 1, mean: 1 +/- 0)");
  calc.Push(2);
  calc.Push(3);
  strm.str("");
  strm << calc;
  EXPECT_EQ(strm.str(), "(count: 3, min: 1, max: 3, mean: 2 +/- 1)");
  calc.Reset();
  strm.str("");
  strm << calc;
  EXPECT_EQ(strm.str(), "(count: 0)");
}
