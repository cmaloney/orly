/* <test/perf.example.cc>

   Example usage of the perf testing kit.

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

#include <test/perf.h>

using namespace std;
using namespace Test;

//Name, max test time
PERF(Name, 140) {
  METRIC(TTimer, FooBar, TMetric::Decreasing);
  METRIC(TCounter, Counter, TMetric::Steady);

  FooBar.Start();

  for(int i=0; i < 10000000; ++i) {
    ++Counter;
  }

  FooBar.Stop();
}

PERF(Sample2, chrono::minutes(5)) {
  TIMING(FooBar, TMetric::Steady);
  COUNTER(Counter, TMetric::Increasing);

  FooBar.Start();

  for(int i=0; i < 10000000; ++i) {
    ++Counter;
    --Counter;
    ++Counter;
  }

  //NOTE: FooBar implicitly stops at the end of scope.
}