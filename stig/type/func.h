/* <stig/type/func.h>

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
#include <stig/type/util.h>

namespace Stig {

  namespace Type {

    class TFunc : public TInternedType<TFunc, TType, TType> {
      NO_COPY_SEMANTICS(TFunc);
      public:

      const TType &GetParamObject() const {
        assert(this);
        return std::get<0>(this->GetKey());
      }
      const TType &GetReturnType() const {
        assert(this);
        return std::get<1>(this->GetKey());
      }
      static TType Get(const TType &param_obj, const TType &return_type) {
        assert(param_obj.Is<TObj>());
        return TInternedType::Get(param_obj, return_type);
      }

      private:
      TFunc(const TType &param_object, const TType &return_type) : TInternedType(param_object, return_type) {}
      virtual ~TFunc();

      virtual void Write(std::ostream &) const;

      friend class TInternedType;
    };  // TFunc

  }  // Type

}  // Stig