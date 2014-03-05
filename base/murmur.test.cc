/* <base/murmur.test.cc>

   Unit test for <base/murmur.h>.

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

#include <base/murmur.h>

#include <base/sigma_calc.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(Typical) {
  /* We will test the numbers in the series f(n) = 2 ^ n - 1 for all values of n from 0 to 63;
     that is, 0, 1, 3, 7, etc.  For each, we'll compute a murmur hash.  We'll then compare each
     hash to the one before it, to see how many have changed.  Our input values are changing by
     only one bit at a time, but we want to see wildly different hashes. */
  TSigmaCalc diff_calc, bit_calc[64];
  uint64_t
      word      = 0,
      prev_hash = 0;
  for (int i = 0; i < 64; ++i) {
    uint64_t this_hash = Murmur(&word, 1);
    if (i) {
      int diff = 0;
      for (int j = 0; j < 64; ++j) {
        uint64_t mask = 1 << j;
        bool is_flipped = ((prev_hash & mask) != (this_hash & mask));
        bit_calc[j].Push(is_flipped ? 1.0 : 0.0);
        if (is_flipped) {
          ++diff;
        }
      }
      diff_calc.Push(diff);
    }
    prev_hash = this_hash;
    word = word * 2 + 1;
  }
  /* Compute the overall bit-flippingness. */
  double min, max, mean, sigma;
  diff_calc.Report(min, max, mean, sigma);
  EXPECT_GE(min,      16.00);  // We always flipped at least a quarter of the bits.
  EXPECT_GE(mean,     32.00);  // On average, we flipped at least half of the bits.
  EXPECT_LE(sigma,     8.00);  // We have a reasonably tight clustering around the mean.
  /* Look for sticky bits. */
  bit_calc[0].Report(min, max, mean, sigma);
  double min_mean = mean;
  for (int i = 1; i < 64; ++i) {
    bit_calc[i].Report(min, max, mean, sigma);
    min_mean = std::min(mean, min_mean);
  }
  EXPECT_GE(min_mean,  0.33);  // Every bit flipped at least a third of the time.  (No sticky bits.)
}
