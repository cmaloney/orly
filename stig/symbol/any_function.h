/* <stig/symbol/any_function.h>

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
#include <memory>
#include <set>
#include <vector>

#include <base/assert_true.h>
#include <base/no_copy_semantics.h>
#include <stig/symbol/def.h>
#include <stig/symbol/result_def.h>
#include <stig/type/func.h>
#include <stig/type/obj.h>

namespace Stig {

  namespace Symbol {

    class TFunction;
    class TBuiltInFunction;

    class TAnyFunction {
      NO_COPY_SEMANTICS(TAnyFunction);
      public:

      class TVisitor {
        NO_COPY_SEMANTICS(TVisitor);
        public:

        virtual ~TVisitor() {}

        virtual void operator()(const TFunction *that) const = 0;
        virtual void operator()(const TBuiltInFunction *that) const = 0;

        protected:

        TVisitor() {}

      };  // TVisitor

      typedef std::shared_ptr<TAnyFunction> TPtr;

      typedef std::vector<TResultDef::TPtr> TResultDefVec;

      virtual void Accept(const TVisitor &visitor) const = 0;

      void Add(const TResultDef::TPtr &result_def);

      const std::string &GetName() const;

      virtual Type::TObj::TElems GetParams() const = 0;

      const TResultDefVec &GetResultDefs() const;

      virtual Type::TType GetReturnType() const = 0;

      Type::TType GetType() const;

      void Remove(const TResultDef::TPtr &result_def);

      protected:

      TAnyFunction(const std::string &name);

      private:

      std::string Name;

      TResultDefVec ResultDefs;

    };  // TAnyFunction

  }  // Symbol

}  // Stig
