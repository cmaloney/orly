/* <tools/nycr/symbol/prec_level.cc>

   Implements <tools/nycr/symbol/prec_level.h>.

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

#include <tools/nycr/symbol/prec_level.h>

using namespace std;
using namespace Tools::Nycr::Symbol;

TPrecLevel::TPrecLevel(const TName &name, size_t idx)
  : Name(name), Idx(idx) {
  PrecLevels.push_back(this);
}

TPrecLevel::~TPrecLevel() {
  assert(this);
  /* This is over-kill, but we should only be destroying any single prec level when we're in the midst of destroying them all.  If this ever turns
     out not to be the case, we'll need to be smarter about how to clean up this vector.  And about how to reset the idx values of all the prec
     levels that bump down. */
  PrecLevels.clear();
}

TPrecLevel::TPrecLevels TPrecLevel::PrecLevels;
