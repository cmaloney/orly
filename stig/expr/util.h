/* <stig/expr/util.h>

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

namespace Stig {

  namespace Expr {

    template <typename TFinal>
    const TFinal *TExpr::As() const {
      static_assert(std::is_base_of<TExpr, TFinal>::value, "TExpr::As<>() can only be used with final types");
      assert(this);
      auto result = TryAs<TFinal>();
      assert(result);
      return result;
    }

    template <typename TFinal>
    bool TExpr::Is() const {
      static_assert(std::is_base_of<TExpr, TFinal>::value, "TExpr::Is<>() can only be used with final types");
      assert(this);
      return TryAs<TFinal>();
    }

    template <typename TFinal>
    const TFinal *TExpr::TryAs() const {
      static_assert(std::is_base_of<TExpr, TFinal>::value, "TExpr::TryAs<>() can only be used with final types");
      assert(this);
      return dynamic_cast<const TFinal *>(this);
    }

  } // Expr

} // Stig