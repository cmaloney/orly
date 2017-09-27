/* <util/error.test.cc>

   Unit test for <util/error.h>.

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <util/error.h>

#include <unistd.h>

#include <thread>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Util;

FIXTURE(LibraryGenerated) {
  EXPECT_THROW(system_error, [&]() { thread().detach(); });
}

FIXTURE(UtilsGenerated) {
  EXPECT_THROW(system_error, [&]() { IfLt0(read(-1, nullptr, 0)); });
}
