/* <stig/client/program/throw_syntax_errors.cc>

   Implements <stig/client/program/throw_syntax_errors.h>.

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

#include <stig/client/program/throw_syntax_errors.h>

#include <sstream>

#include <base/thrower.h>
#include <tools/nycr/error.h>

using namespace std;
using namespace Stig::Client::Program;

void Stig::Client::Program::ThrowSyntaxErrors() {
  if (Tools::Nycr::TError::GetFirstError()) {
    ostringstream strm;
    Tools::Nycr::TError::PrintSortedErrors(strm);
    DEFINE_ERROR(error_t, runtime_error, "syntax error(s)");
    THROW_ERROR(error_t) << strm.str();
  }
}
