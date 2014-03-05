/* <stig/rt/lazy_random_list.h>

   Implements a lazy list which generates random numbers.

   TODO: The "Lazy" portion should probably be separated from teh "random numbers" portion.

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

#include <random>
#include <vector>

#include <base/no_copy_semantics.h>

namespace Stig {
  namespace Rt {
    class TLazyRandomList {
      NO_COPY_SEMANTICS(TLazyRandomList);
      public:

      TLazyRandomList(int64_t seed) : Prng(seed) {}

      TLazyRandomList(int64_t seed, int64_t min, int64_t max) : Prng(seed), UniformIntDistribution(min, max) {
        assert(min < max);
      }

      int64_t Get(int64_t idx) const {
        assert(idx >= 0);
        int64_t count_read = CachedValues.size();

        // If we haven't read off the generator to the requested index, do it now.
        if(idx >= count_read) {
          CachedValues.reserve(idx);
          for(;idx >= count_read; ++count_read) {
            CachedValues.emplace_back(UniformIntDistribution(Prng));
          }
        }

        //Guaranteed to be in bounds because we just expanded to cover
        return CachedValues[idx];
      }

      private:
      // People shouldn't see that we're lazy internally
      // They should use us like any other list.
      mutable std::vector<int64_t> CachedValues;
      mutable std::mt19937_64 Prng;
      mutable std::uniform_int_distribution<int64_t> UniformIntDistribution;
    };
  }
} // Stig