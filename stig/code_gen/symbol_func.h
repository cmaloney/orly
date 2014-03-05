/* <stig/code_gen/symbol_func.h>

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

#include <stig/code_gen/function.h>
#include <stig/symbol/function.h>

namespace Stig {

  namespace CodeGen {

    class TSymbolFunc : public virtual TFunction {
      NO_COPY_SEMANTICS(TSymbolFunc);
      public:

      static TFunction::TPtr Find(const Symbol::TFunction *symbol);

      std::string GetName() const;
      Type::TType GetReturnType() const;
      Type::TType GetType() const;

      protected:
      TSymbolFunc(const L0::TPackage *package, const Symbol::TFunction::TPtr &symbol, const TIdScope::TPtr &id_scope);

      private:

      //TODO: Kill the static. We can really handle this using proper scoping and a tree of function maps.
      static std::unordered_map<const Symbol::TFunction*, TSymbolFunc*> Functions;

      //Note we would use an InlineScope here, but we need to attach the function arguments to the CodeScope.
      TInline::TPtr Body;

      Symbol::TFunction::TPtr Symbol;

    }; // TSymbolFunc

  } // CodeGen

} // Stig