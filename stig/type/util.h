/* <stig/type/util.h>

   Functions to test if a type is a specific type.

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

#include <type_traits>

#include <stig/type.h>

namespace Stig {

  namespace Type {


    template <typename TTarget>
    const TTarget *TType::As() const {
      static_assert(std::is_base_of<TType::TImpl, TTarget>::value, "TType::As<>() can only be used with final types");
      assert(this);

      auto res = TryAs<TTarget>();
      assert(res);
      return res;
    }

    template <typename TTarget>
    bool TType::Is() const {
      static_assert(std::is_base_of<TType::TImpl, TTarget>::value, "TType::Is<>() can only be used with final types");
      assert(this);

      return TryAs<TTarget>();
    }

    template <typename TTarget>
    const TTarget *TType::TryAs() const {
      static_assert(std::is_base_of<TType::TImpl, TTarget>::value, "TType::TryAs<>() can only be used with final types");
      assert(this);

      return dynamic_cast<const TTarget*>(Impl.get());
    }

  } // Type

} // Stig