/* <stig/type/dict.h>

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

#include <stig/type/managed_type.h>

namespace Stig {

  namespace Type {

    class TDict : public TInternedType<TDict, TType, TType> {
      NO_COPY_SEMANTICS(TDict);
      public:

      const TType &GetKey() const {
        assert(this);
        return std::get<0>(TInternedType::GetKey());
      }

      const TType &GetVal() const {
        assert(this);
        return std::get<1>(TInternedType::GetKey());
      }

      static TType Get(const TType &key, const TType &val) {
        return TInternedType::Get(key, val);
      }

      private:
      TDict(const TType &key, const TType &val) : TInternedType(key, val) {}
      virtual ~TDict();

      virtual void Write(std::ostream &) const;
      friend class TInternedType;
    };  // TDict

  }  // Type

}  // Stig