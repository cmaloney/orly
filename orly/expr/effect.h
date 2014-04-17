/* <orly/expr/effect.h>

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
#include <orly/expr/thatable_unary.h>
#include <orly/expr/visitor.h>
#include <orly/symbol/stmt/stmt_block.h>

namespace Orly {

  namespace Expr {

    class TEffect
        : public TThatableUnary {
      NO_COPY(TEffect);
      public:

      typedef std::shared_ptr<TEffect> TPtr;

      static TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      const Symbol::Stmt::TStmtBlock::TPtr &GetStmtBlock() const;

      virtual Type::TType GetType() const;

      void SetStmtBlock(const Symbol::Stmt::TStmtBlock::TPtr &stmt_block);

      private:

      TEffect(const TExpr::TPtr &expr, const TPosRange &pos_range);

      Symbol::Stmt::TStmtBlock::TPtr StmtBlock;

    };  // TEffect

  }  // Expr

}  // Orly
