/* <base/unreachable.cc>

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

#include <base/unreachable.h>
#include <iostream>

#include <base/backtrace.h>
#include <base/exception.h>
#include <base/thrower.h>
#include <util/error.h>

using namespace Base;

void Base::Unreachable(const TCodeLocation &loc) {
  PrintBacktrace();

  std::cout << "Reached unreachable location: " << loc << std::endl;

// In release builds, don't abort
// TODO: Move this ndebug into abort?
#ifdef NDEBUG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-member-function"
  EXCEPTION(unreachable_t, std::runtime_error, "Unreachable code")
#pragma clang diagnostic pop
  TThrower<unreachable_t>{loc};
#else
  Util::Abort(loc);
#endif
}
