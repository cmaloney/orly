/* <stig/rt/unknown.h>

   The 'unknown' runtime value.

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
#include <cstddef>
#include <unordered_map>

namespace Stig {

  namespace Rt {

    class TUnknown {
      public:

      /* TODO */
      virtual size_t GetHash() const {
        assert(this);
        return 0;
      }

      /* TODO */
      TUnknown() {}

      /* TODO */
      ~TUnknown() {}

      /* TODO */
      bool operator==(const TUnknown &that) const {
        assert(this);
        assert(&that);
        return true;
      }

      /* TODO */
      bool operator!=(const TUnknown &that) const {
        assert(this);
        assert(&that);
        return false;
      }

      private:

    };  // TUnknown

  }  // Rt

}  // Stig

namespace std {

  /* A standard hasher for Stig::Rt::TUnknown. */
  template <>
  struct hash<Stig::Rt::TUnknown> {

    typedef size_t result_type;
    typedef Stig::Rt::TUnknown argument_type;

    result_type operator()(const argument_type &that) const {
      return that.GetHash();
    }

  };  // hash<Stig::Rt::TUnknown

}  // std
