/* <stig/sabot/visitor_perf.cc>

   TODO

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

#include <stig/sabot/compare_types.h>

#include <iostream>
#include <sstream>
#include <string>

#include <base/timer.h>
#include <stig/native/state.h>
#include <stig/native/type.h>
//#include <stig/native/all.h>
//#include <stig/native/point.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Native;
using namespace Stig::Sabot;

template <typename TLhs, typename TRhs>
TComparison CheckTypes() {
  return CompareTypes(Native::Type::For<TLhs>::Type, Native::Type::For<TRhs>::Type);
}

int main() {
  size_t found_eq = 0UL;
  size_t num_iter = 100000UL;
  Base::TTimer timer;
  timer.Start();
  for (size_t i = 0; i < num_iter; ++i) {
    found_eq += IsEq(CheckTypes<int8_t, int8_t>()) ? 1UL : 0UL;
  }
  timer.Stop();
  std::cout << num_iter << " Comparisons took " << timer.Total() << " [" << (timer.Total() / num_iter) << " / s] " << std::endl;
  if (found_eq != num_iter) {
    throw std::runtime_error("Did not match");
  }
}