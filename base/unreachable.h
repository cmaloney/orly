/* <base/unreachable.h>

   Use this to indicate a position which is unreachable.

   Gives us the advantage of a backtrace if reached, as well as telling GCC the location is indeed
   unreachable.

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

#include <base/code_location.h>

namespace Base {

[[noreturn]] void Unreachable(const TCodeLocation &loc);

}  // Base
