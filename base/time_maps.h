/* <base/time_maps.h>

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

#include <base/safe_global.h>
#include <stig/type/bool.h>
#include <stig/type/int.h>
#include <stig/type/obj.h>

namespace Base {

  namespace Chrono {

    /* These maps are used for checking the structural type of objects later in the compiler.
       For use with implementing time_obj, which converts time_pnts AND time_diffs into objects. */

    extern const TSafeGlobal<Stig::Type::TObj::TElems> TimeDiffMap;

    extern const TSafeGlobal<Stig::Type::TObj::TElems> TimePntMap;

    /* Helper function to check if an object is a time object */
    extern bool IsTimeObj(const Stig::Type::TObj *type);

  }  // Chrono

}  // Base