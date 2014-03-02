/* <stig/rpc/type/cmp.h> 

   Compare two types for ordering.

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

#include <stig/rpc/type.h>

/* Compare the left- and right-hand types and return based on their relative ordering:
     -1, lhs comes before rhs;
      0, lhs and rhs have equivalent ordering; or
     +1, lhs comes after rhs.
   A return of 0 indicates that the two types are semantically equivalent.  A non-zero
   return indicates that they are semantically different. */
int Cmp(const Stig::Rpc::TType &lhs, const Stig::Rpc::TType &rhs);

/* True iff. the two types are semantically equivalent. */
inline bool operator==(const Stig::Rpc::TType &lhs, const Stig::Rpc::TType &rhs) {
  return Cmp(lhs, rhs) == 0;
}

/* True iff. the two types are not semantically equivalent. */
inline bool operator!=(const Stig::Rpc::TType &lhs, const Stig::Rpc::TType &rhs) {
  return Cmp(lhs, rhs) != 0;
}

/* True iff. lhs orders before rhs. */
inline bool operator<(const Stig::Rpc::TType &lhs, const Stig::Rpc::TType &rhs) {
  return Cmp(lhs, rhs) < 0;
}

/* True iff. lhs orders before rhs or lhs and rhs are semantically equivalent. */
inline bool operator<=(const Stig::Rpc::TType &lhs, const Stig::Rpc::TType &rhs) {
  return Cmp(lhs, rhs) <= 0;
}

/* True iff. lhs orders after rhs. */
inline bool operator>(const Stig::Rpc::TType &lhs, const Stig::Rpc::TType &rhs) {
  return Cmp(lhs, rhs) > 0;
}

/* True iff. lhs orders after rhs or lhs and rhs are semantically equivalent. */
inline bool operator>=(const Stig::Rpc::TType &lhs, const Stig::Rpc::TType &rhs) {
  return Cmp(lhs, rhs) >= 0;
}

