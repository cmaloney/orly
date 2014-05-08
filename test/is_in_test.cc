/* <test/is_in_test.cc>

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

#include <test/is_in_test.h>

using namespace Test;

// We use a local function with private variable to get the guarantees we want around initialization of this.
static bool InTest(bool set_true) {
  static bool in_test = false;

  if (set_true) {
    if(in_test) {

    }
    in_test = true;
  }

  return in_test;
}


bool AvoidTheseWheneverPossible::AvoidTheseWheneverPossible::IsInTest() {
  return InTest(false);
}

//NOTE: Do not call this yourself. It is called once (and only once) by the test runner.
void AvoidTheseWheneverPossible::AvoidTheseWheneverPossible::MarkAsInTest() {
  InTest(true);
}