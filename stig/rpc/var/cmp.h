/* <stig/rpc/var/cmp.h> 

   Compare two variants for ordering.

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

#include <stig/rpc/var.h>

/* Compare the left- and right-hand variants and return based on their relative ordering:
     -1, lhs comes before rhs;
      0, lhs and rhs have equivalent ordering; or
     +1, lhs comes after rhs.
   A return of 0 indicates that the two variants are semantically equivalent.  A non-zero
   return indicates that they are semantically different.
   Variants of different types will order based on their types alone.  Variants of the
   same type will order based on their state. */
int Cmp(const Stig::Rpc::TVar &lhs, const Stig::Rpc::TVar &rhs);

/* True iff. the two variants are semantically equivalent. */
inline bool operator==(const Stig::Rpc::TVar &lhs, const Stig::Rpc::TVar &rhs) {
  return Cmp(lhs, rhs) == 0;
}

/* True iff. the two variants are not semantically equivalent. */
inline bool operator!=(const Stig::Rpc::TVar &lhs, const Stig::Rpc::TVar &rhs) {
  return Cmp(lhs, rhs) != 0;
}

/* True iff. lhs orders before rhs. */
inline bool operator<(const Stig::Rpc::TVar &lhs, const Stig::Rpc::TVar &rhs) {
  return Cmp(lhs, rhs) < 0;
}

/* True iff. lhs orders before rhs or lhs and rhs are semantically equivalent. */
inline bool operator<=(const Stig::Rpc::TVar &lhs, const Stig::Rpc::TVar &rhs) {
  return Cmp(lhs, rhs) <= 0;
}

/* True iff. lhs orders after rhs. */
inline bool operator>(const Stig::Rpc::TVar &lhs, const Stig::Rpc::TVar &rhs) {
  return Cmp(lhs, rhs) > 0;
}

/* True iff. lhs orders after rhs or lhs and rhs are semantically equivalent. */
inline bool operator>=(const Stig::Rpc::TVar &lhs, const Stig::Rpc::TVar &rhs) {
  return Cmp(lhs, rhs) >= 0;
}

namespace Stig {

  namespace Rpc {

    namespace Var {

      /* An STL helper for comparing pointers to variants. */
      struct TCmp final {
        bool operator()(const TVar *lhs, const TVar *rhs) const {
          return *lhs < *rhs;
        }
      };

    }  // Var

  }  // Rpc

}  // Stig

