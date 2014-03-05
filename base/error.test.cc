/* <base/error.test.cc>

   Unit test for <base/error.h>.

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

#include <base/error.h>

#include <test/kit.h>

using namespace Base;

class TMyError : public TFinalError<TMyError> {
  public:

  TMyError(const TCodeLocation &code_location, const char *details = 0) {
    PostCtor(code_location, details);
  }
  TMyError(const TCodeLocation &code_location, const char *details_start, const char *details_end) {
    PostCtor(code_location, details_start, details_end);
  }
};

FIXTURE(Typical) {
  TMyError my_error(HERE);
}