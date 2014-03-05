/* <base/max_sizeof.test.cc>

   Unit test for <base/max_sizeof.h>.

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

#include <base/max_sizeof.h>

#include <algorithm>
#include <string>

#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(Typical) {
  EXPECT_EQ((MaxSizeof<>::Get()), 0u);
  EXPECT_EQ((MaxSizeof<int>::Get()), sizeof(int));
  EXPECT_EQ((MaxSizeof<char, double, string>::Get()), max(max(sizeof(char), sizeof(double)), sizeof(string)));
}
