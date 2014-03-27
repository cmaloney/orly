/* <strm/mem/static_in.cc>

   Implements <strm/mem/static_in.h>.

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

#include <strm/mem/static_in.h>

using namespace std;
using namespace Strm::Mem;

bool TStaticIn::Cycle(
    size_t release_count, const uint8_t **start, const uint8_t **limit) {
  assert(this);
  /* If the consumer is releasing a workspace, we must have previously provided
     it.  We have only one workspace, however, so the count being released must
     be exactly 1. */
  if (release_count) {
    assert(release_count == 1);
    assert(Start < Limit);
    Start = nullptr;
    Limit = nullptr;
  }
  /* If the consumer is requesting more data, we will try to provide it;
     otherwise, we have nothing else to do. */
  bool success;
  if (start) {
    assert(limit);
    /* If we not yet reported workspace, set the out-params to point at it and
       return true; otherwise, leave the out-params alone and return false. */
    success = (Start < Limit);
    if (success) {
      *start = Start;
      *limit = Limit;
    }
  } else {
    success = true;
  }
  return success;
}
