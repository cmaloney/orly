/* <test/is_in_test.h>

   Allows code to see if it's being run inside a unit test.

   Doing this is almost always a bad idea.

   Protects this from happening unintenionally through making the function private in class and sharing it via
   friendship with explicitly approved friendship.

   Other alternatives to allowing code to see if it's in a test:
   1) LD_PRELOAD (or link regularly) in the functions rather than using the standard ones
      - Requires a lot more extensive changes.
      - Gives very nice isolation / prevents code from having to have non-unit-testable code for checking if we're in a test
      - Allows us to write Mocks and Stubs easily.
   2) A generally available "IsInTest"
      - Has a high probabiltiy of being abused / cargo culted around the codebase. We want it to be non-trivial to make
        more things behave differently in unit tests

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

#pragma once

#include <cstdint>

namespace Base {
  void MlockRaw(const void *, uint64_t);
}

namespace Test {

  void ExtraInit();

  /* Hide the IsInTest() function with friendship / private class, as well as forcing people to explicitly type out this
     shouldn't be done */
  class AvoidTheseWheneverPossible {
    private: // Being extra explicit

    /* Returns true iff we're in a unit test. */
    static bool IsInTest();

    //NOTE: Do not call this yourself. It is called once (and only once) by the test runner.
    static void MarkAsInTest();

    friend void Test::ExtraInit();
    friend void Base::MlockRaw(const void *, uint64_t);
  };

} // Base