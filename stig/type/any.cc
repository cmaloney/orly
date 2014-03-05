/* <stig/type/any.cc>

   Implements <stig/type/any.h>

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

#include <stig/type/any.h>

#include <base/not_implemented_error.h>

using namespace Stig::Type;

IMPL_SINGLETON_TYPE(TAny);

TAny::~TAny() {}

void TAny::Write(std::ostream &) const {
  throw Base::TNotImplementedError(HERE, "Attempted to print out the TAny type.");
}
