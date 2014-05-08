/* <base/mlock.h>

   Wraps the mlock() syscall. making it throw an exception if the mlock fails and we aren't in a test.

   In a test if it fails, we silently ignore the failure since it degrades perf but doesn't make our behavior incorrect.

   It also makes mlock a little more convenient to use / keeps you from doing the size math by hand unless you really,
   really want to.

   NOTE: Doing things differently in tests is generally a bad idea. In this case it's a workaround to allow unit test
   machines to have lower amounts of memory sacrificing performance and some guarantees potentially (Linux OOM Killer).

   NOTE: Doesn't setup RAII objects to unlock the memory.

   NOTE: Doing things differently in tests is generally a bad idea. In this case it's a workaround to allow unit test
   machines to have lower amounts of memory sacrificing performance and some guarantees potentially (Linux OOM Killer).

   Just making the mlock be special (and all the logic isolated to one translation unit) gives us what we need with a
   relatively low implementation cost. By all means, feel free to implement suggstion 1 above and pipe it through all
   the tools we use for running tests for all time. Patches welcome (although there is a lot you'll have to patch)

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

  //TODO: Unify these into just 'Mlock' which auto-selects the right behavior.

  /* Pass-through wrapper to mlock() which makes the call's mandated success conditional to being in a unit test. */
  void MlockRaw(const void *val, uint64_t size);

  /* Mlock an object */
  template <typename TVal>
  void Mlock(const TVal &val) {
    MlockRaw(&val, sizeof(TVal));
  }

  /* Mlock count objects */
  template <typename TVal>
  void MlockN(const TVal &val, uint64_t count) {
    MlockRaw(&val, sizeof(TVal) * count);
  }

} // Base