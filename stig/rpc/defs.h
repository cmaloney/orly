/* <stig/rpc/defs.h> 

   Definitions in common between the type and variant systems.

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

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <base/chrono.h>
#include <base/opt.h>
#include <base/uuid.h>

namespace Stig {

  namespace Rpc {

    /* These are kinds of types in Stig.
       We'll use these to differentiate template specializations and as codes in the wire format. */
    enum class TKind : uint8_t {
      Addr     = 'a',
      Bool     = 'b',
      Dict     = 'd',
      Id       = 'u',
      Int      = 'i',
      List     = 'l',
      Obj      = 'x',
      Opt      = 'o',
      Real     = 'r',
      Set      = 'e',
      Str      = 's',
      TimeDiff = 'f',
      TimePnt  = 'p'
    };

    /* These specify the sorting order for fields within an Addr.
       These also appear in the wire format. */
    enum class TDir : uint8_t {
      Asc  = '+',
      Desc = '-'
    };

    /* Compare the left- and right-hand ordering directions:
         -1, lhs comes before rhs;
          0, lhs and rhs are equal; or
         +1, lhs comes after rhs. */
    int CmpDirs(TDir lhs, TDir rhs);

    /* A human-readable name for the sorting order. */
    const char *GetDirName(TDir dir);

    /* Aliases for the C++ native types which map to the monomorphic Stig types:
       bool, id, int, real, str, time_diff, and time_pnt. */
    using TBool     = bool;
    using TId       = Base::TUuid;
    using TInt      = int64_t;
    using TReal     = double;
    using TStr      = std::string;
    using TTimeDiff = Base::Chrono::TTimeDiff;
    using TTimePnt  = Base::Chrono::TTimePnt;

    /* Aliases for the C++ native types which map to the arity-1 polymorphic Stig types:
       list, opt, and set. */
    template <typename TElem> using TList = std::vector<TElem>;
    template <typename TElem> using TOpt  = Base::TOpt<TElem>;
    template <typename TElem> using TSet  = std::set<TElem>;

    /* Aliases for the C++ native types which map to the arity-2 polymorphic Stig types:
       just dict, for now. */
    template <typename TKey, typename TVal> using TDict = std::map<TKey, TVal>;

    /* NOTE: The arity-n polymorphic types are not represented by native C++ types.
             These can only exist as variants. */

  }  // Rpc

}  // Stig

