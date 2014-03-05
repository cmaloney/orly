/* <base/false.h>

   Use this class to generate a nice error message at compile time for the generic version of the template that would
   otherwise be undefined.

   For example, instead of doing the following,

     // Generic version left undefined.
     template <typename TResult, typename... TArgs>
     class TFunction;

     // Specialized version expected to be used.
     template <typename TResult, typename... TArgs>
     class TFunction<TResult (TArgs...)> {
       public:

       // class definition
     };

   Define the generic version as:

     template <typename TResult, typename... TArgs>
     class TFunction {
       public:

       // Compile time error with useful information.
       static_assert(Base::TFalse<TResult, TArgs...>::Val,
                     "Generic version of the TFunction template is undefined. Use TFunction<TResult (TArgs...)> version.");
     };

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

#include <base/no_construction.h>

namespace Base {

  template <typename... T>
  struct TFalse {
    NO_CONSTRUCTION(TFalse);

    static constexpr bool Val = false;
  };
}