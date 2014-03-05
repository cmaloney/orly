/* <stig/symbol/built_in_function.h>

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

#include <memory>
#include <string>
#include <vector>

#include <base/safe_global.h>
#include <stig/symbol/any_function.h>

namespace Stig {

  namespace Symbol {

    class TBuiltInFunction
        : public TAnyFunction,
          public std::enable_shared_from_this<TBuiltInFunction> {
      NO_COPY_SEMANTICS(TBuiltInFunction);
      public:

      typedef std::vector<std::string> TOrderedParams;

      typedef std::shared_ptr<TBuiltInFunction> TPtr;

      static const Base::TSafeGlobal<TPtr> TimeDiff;

      static const Base::TSafeGlobal<TPtr> TimePnt;

      static const Base::TSafeGlobal<TPtr> RandomInt;

      static const Base::TSafeGlobal<TPtr> Replace;

      static TPtr New(
          const std::string &name,
          const std::vector<std::string> &ordered_params,
          const Type::TObj::TElems &params,
          const Type::TType &return_type);

      virtual void Accept(const TVisitor &visitor) const;

      /* Get a vector of parameter names in output printing order. */
      const TOrderedParams &GetOrderedParams() const;

      virtual Type::TObj::TElems GetParams() const;

      std::shared_ptr<const TBuiltInFunction> GetPtr() const;

      virtual Type::TType GetReturnType() const;

      private:

      TBuiltInFunction(
          const std::string &name,
          const TOrderedParams &ordered_praams,
          const Type::TObj::TElems &params,
          const Type::TType &return_type);

      TOrderedParams OrderedParams;

      Type::TObj::TElems Params;

      Type::TType ReturnType;

    };  // TBuiltInFunction

  }  // Symbol

}  // Stig
