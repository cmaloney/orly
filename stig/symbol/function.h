/* <stig/symbol/function.h>

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

#include <stig/pos_range.h>
#include <stig/symbol/any_function.h>
#include <stig/symbol/root.h>

namespace Stig {

  namespace Symbol {

    class TScope;

    class TFunction
        : public TRoot,
          public TAnyFunction,
          public std::enable_shared_from_this<TFunction> {
      NO_COPY_SEMANTICS(TFunction);
      public:

      typedef std::shared_ptr<TFunction> TPtr;

      class TParamDef
          : public TDef {
        NO_COPY_SEMANTICS(TParamDef);
        public:

        typedef std::shared_ptr<TParamDef> TPtr;

        virtual Type::TType GetType() const = 0;

        protected:

        TParamDef(const std::string &name, const TPosRange &pos_range);

      };  // TParamDef

      typedef std::shared_ptr<TScope> TScopePtr;

      typedef std::set<TParamDef::TPtr> TParamDefSet;

      static TPtr New(const TScopePtr &scope, const std::string &name, const TPosRange &pos_range);

      virtual ~TFunction();

      virtual void Accept(const TVisitor &visitor) const;

      void Add(const TParamDef::TPtr &param_def);

      const TParamDefSet &GetParamDefs() const;

      virtual Type::TObj::TElems GetParams() const;

      const TPosRange &GetPosRange() const;

      virtual Type::TType GetReturnType() const;

      TScopePtr GetScope() const;

      void Remove(const TParamDef::TPtr &param_def);

      TScopePtr TryGetScope() const;

      private:

      TFunction(const TScopePtr &scope, const std::string &name, const TPosRange &pos_range);

      mutable bool IsRecursive;

      TParamDefSet ParamDefs;

      std::weak_ptr<TScope> Scope;

      const TPosRange PosRange;

    };  // TFunction

  }  // Symbol

}  // Stig
