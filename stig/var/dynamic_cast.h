/* <stig/var/dynamic_cast.h>

   Generic dynamic cast

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

#include <stig/var/impl.h>
#include <stig/var/mutable.h>

namespace Stig {

  namespace Var {

    template<typename TVal>
    TVal DynamicCast(const TVar &that) {
      TMutable *ptr = dynamic_cast<TMutable *>(that.Impl.get());
      if (ptr) {
        return TVar::TDt<TVal>::As(ptr->GetVal());
      }
      return TVar::TDt<TVal>::As(that);
    }

  } // Var

} // Stig