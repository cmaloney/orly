/* <stig/rt/string.cc>

   Implements <stig/rt/string.h>

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

#include <algorithm>
#include <cctype>
#include <string>

namespace Stig {

  namespace Rt {

    std::string ToLower (const std::string &str) {
      char ret[str.length() + 1];
      std::transform(str.begin(), str.end(), ret,
          [](char c) -> char {
            return static_cast<char>(std::tolower(c));
          });
      ret[str.length()] = '\0';  // C string terminator
      return std::string(ret);
    }

    std::string ToUpper (const std::string &str) {
      char ret[str.length() + 1];
      std::transform(str.begin(), str.end(), ret,
          [](char c) -> char {
            return static_cast<char>(std::toupper(c));
          });
      ret[str.length()] = '\0';  // C string terminator
      return std::string(ret);
    }
  }
}