/* <base/mlock.cc>

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

#include <base/mlock.h>

#include <sys/mman.h>

#include <base/error_utils.h>
#include <test/is_in_test.h>

void Base::MlockRaw(const void *val, uint64_t len) {
  int res = mlock(val, len);

  // If we're in a test and mlock fails, ignore the failure silently
  // The failure only changes perf, not correctness
  if (Test::AvoidTheseWheneverPossible::IsInTest()) {
    return;
  }

  Base::IfLt0(res);
}