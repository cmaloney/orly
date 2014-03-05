/* <stig/atom/defs.h>

   Typedefs used to standardize the mapping between Stig tycons and C++ types and templates.

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

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <base/chrono.h>
#include <base/opt.h>
#include <base/uuid.h>
#include <stig/asc.h>
#include <stig/desc.h>

namespace Stig {

  namespace Atom {

    /* bool */
    typedef bool TBool;

    /* int */
    typedef int64_t TInt;

    /* real */
    typedef double TReal;

    /* id */
    typedef Base::TUuid TId;

    /* time_pnt */
    typedef Base::Chrono::TTimePnt TTimePnt;

    /* time_diff */
    typedef Base::Chrono::TTimeDiff TTimeDiff;

    /* str */
    typedef std::string TStr;

    /* opt */
    template <typename TChild>
    using TOpt = Base::TOpt<TChild>;

    /* set */
    template <typename TChild>
    using TSet = std::unordered_set<TChild>;

    /* list */
    template <typename TChild>
    using TList = std::vector<TChild>;

    /* asc */
    template <typename TChild>
    using TAsc = Stig::TAsc<TChild>;

    /* desc */
    template <typename TChild>
    using TDesc = Stig::TDesc<TChild>;

    /* dict */
    template <typename TLhs, typename TRhs>
    using TDict = std::unordered_map<TLhs, TRhs>;

    /* tuple */
    template <typename... TArgs>
    using TTuple = std::tuple<TArgs...>;

    /* TODO: obj */

  }  // Atom

}  // Stig
