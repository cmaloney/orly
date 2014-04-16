/* <orly/expr/ref.h>

   TODO

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <base/class_traits.h>
#include <orly/expr/leaf.h>
#include <orly/symbol/def.h>

namespace Orly {

  namespace Expr {

    class TRef
        : public TLeaf {
      NO_COPY(TRef);
      public:

      typedef std::shared_ptr<TRef> TPtr;

      static TPtr New(const Symbol::TDef::TPtr &def, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      const Symbol::TDef *GetDef() const;

      void ImplicitFunctionAppCheck() const;

      private:

      TRef(const Symbol::TDef *def, const TPosRange &pos_range);

      const Symbol::TDef *Def;

    };  // TRef

  }  // Expr

}  // Orly
