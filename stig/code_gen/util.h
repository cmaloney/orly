/* <stig/code_gen/util.h>

   Useful functions for code generation.

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
namespace Stig {
  namespace CodeGen {

    template <typename TContainer, typename TFunc>
    void All(TContainer container, const TFunc &func) {
      for(auto &it: container) {
        func(it);
      }
    }

  } // CodeGen
} // Stig