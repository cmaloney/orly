/* <stig/indy/disk/util/hash_util.cc>

   Implements <stig/indy/disk/util/hash_util.h>.

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

#include <cmath>
#include <gmpxx.h>

#include <stig/indy/disk/util/hash_util.h>

using namespace std;
using namespace Stig::Indy::Disk::Util;

const set<size_t> Stig::Indy::Disk::Util::GenSizeSet {64,
                                                     4096,
                                                     32768,
                                                     262144,
                                                     2097152,
                                                     8388608,
                                                     33554432,
                                                     134217728,
                                                     268435456,
                                                     536870912,
                                                     1073741824,
                                                     2147483648,
                                                     4294967296,
                                                     8589934592,
                                                     34359738368,
                                                     137438953472,
                                                     549755813888,
                                                     2199023255552};

/* TODO */
const set<size_t> Stig::Indy::Disk::Util::HashSizeSet {97UL,
                                                      193UL,
                                                      389UL,
                                                      457UL,
                                                      569UL,
                                                      709UL,
                                                      887UL,
                                                      1109UL,
                                                      1381UL,
                                                      1723UL,
                                                      2153UL,
                                                      2693UL,
                                                      3371UL,
                                                      4211UL,
                                                      5261UL,
                                                      6569UL,
                                                      8209UL,
                                                      10259UL,
                                                      12823UL,
                                                      16033UL,
                                                      20047UL,
                                                      25057UL,
                                                      31307UL,
                                                      39133UL,
                                                      48907UL,
                                                      61141UL,
                                                      76421UL,
                                                      95527UL,
                                                      119417UL,
                                                      149249UL,
                                                      186569UL,
                                                      233201UL,
                                                      291503UL,
                                                      364373UL,
                                                      455466UL,
                                                      569369UL,
                                                      711679UL,
                                                      889581UL,
                                                      HighestPrecalculatedHashSize,
};



size_t Stig::Indy::Disk::Util::SuggestHashSize(size_t num_keys) {
  auto ideal_hash_size = floor(num_keys / MaximumLoadFactor);

  /* If the num keys will fit nicely in to one of our precalculated hash sizes, just return that */
  if (ideal_hash_size < HighestPrecalculatedHashSize) {
    return *(HashSizeSet.upper_bound(ideal_hash_size));
  }

  mpz_class next_prime, ihs = ideal_hash_size;
  auto tmp_np = next_prime.get_mpz_t();
  auto tmp_ihs = ihs.get_mpz_t();
  mpz_nextprime(tmp_np, tmp_ihs);

  return mpz_get_ui(next_prime.get_mpz_t());
}

size_t Stig::Indy::Disk::Util::SuggestGeneration(size_t num_keys) {
  auto upper = GenSizeSet.upper_bound(num_keys);
  return *upper;
}
