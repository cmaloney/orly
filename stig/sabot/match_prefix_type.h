/* <stig/sabot/match_prefix_type.h>

   TODO

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

#include <cassert>
#include <ostream>

#include <stig/sabot/type.h>
#include <stig/sabot/compare_states.h>

namespace Stig {

  namespace Sabot {

    /* The result of prefix matching 2 types. */
    enum class TMatchResult {
      NoMatch,
      PrefixMatch,
      Unifies
    };  // TMatchResult

    /* True for NoMatch */
    inline bool IsNoMatch(TMatchResult result) {
      switch (result) {
        case TMatchResult::NoMatch: {
          return true;
        }
        case TMatchResult::PrefixMatch:
        case TMatchResult::Unifies: {
          return false;
        }
      }
      throw;
    }

    /* True for PrefixMatch || Unifies */
    inline bool IsPrefixMatch(TMatchResult result) {
      switch (result) {
        case TMatchResult::NoMatch: {
          return false;
        }
        case TMatchResult::PrefixMatch:
        case TMatchResult::Unifies: {
          return true;
        }
      }
      throw;
    }

    /* True for Unifies */
    inline bool IsUnifies(TMatchResult result) {
      switch (result) {
        case TMatchResult::NoMatch:
        case TMatchResult::PrefixMatch: {
          return false;
        }
        case TMatchResult::Unifies: {
          return true;
        }
      }
      throw;
    }

    /* TODO */
    TMatchResult MatchPrefixType(const Type::TAny &lhs_any, const Type::TAny &rhs_any);

  }  // Sabot

}  // Stig