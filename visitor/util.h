/* <visitor/util.h>

   The base class instances can be passed around with different conventions. In particular we can pass them around by pointer or
   by something else such as by-value, or by const-ref. Either way we need to call the Accept() function in it. We use this
   function to dereference if it's a pointer and call the Accept() function.

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

#pragma once

#include <utility>

namespace Visitor {

  /* Typical case of class hierarchy. */
  template <typename T>
  T &DerefIfPtr(T *t) {
    return *t;
  }

  /* Pass through. */
  template <typename T>
  T &&DerefIfPtr(T &&t) {
    return std::forward<T>(t);
  }

}  // Visitor
