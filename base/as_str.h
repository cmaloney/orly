/* <base/as_str.h>

   Convert an object to a string, either by using the operator<< overload which is defined for it, or calling the
   provided function which takes an ostream as it's first parameter.

   This prevents us from having to write the exact same code to make an ostringstream and then get the string out all
   over the place

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

#pragma once

#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Base {

  template <typename... TArgs>
  std::string AsStr(TArgs&&... args) {
    std::ostringstream strm;
    int x[] = { (strm << args, 0)... };
    (void)x;
    return strm.str();
  }

  template <typename TFunc, typename... TArgs>
  std::string AsStrFunc(const TFunc &func, TArgs&&... args) {
    std::ostringstream strm;
    func(strm, std::forward<TArgs>(args)...);
    return strm.str();
  }
} // Base
