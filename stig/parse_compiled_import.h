/* <stig/parse_compiled_import.h> 

   TODO

   Copyright 2010-2014 Tagged
   
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

#include <functional>
#include <map>

#include <io/binary_input_stream.h>
#include <stig/var.h>

namespace Stig {

  /* TODO */
  bool ParseCompiledImport(
      Io::TBinaryInputStream &strm,
      const std::function<bool (size_t)> &count_cb, const std::function<bool (const std::vector<std::pair<Base::TUuid, Var::TVar>> &)> &xact_cb);

}  // Stig

