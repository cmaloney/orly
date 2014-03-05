/* <stig/expr/assert.h>

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

#include <unordered_set>

#include <base/opt.h>
#include <stig/error.h>
#include <stig/expr/thatable_unary.h>
#include <stig/symbol/root.h>
#include <stig/type/bool.h>
#include <stig/type/unwrap.h>

namespace Stig {

  namespace Expr {

    class TAssert;

    class TAssertCase
        : public Symbol::TRoot,
          public std::enable_shared_from_this<TAssertCase> {
      NO_COPY_SEMANTICS(TAssertCase);
      public:

      typedef std::shared_ptr<TAssertCase> TPtr;

      typedef std::shared_ptr<TAssert> TAssertPtr;

      static TPtr New(
          const TAssertPtr &assert,
          const Base::TOpt<std::string> &opt_name,
          const TExpr::TPtr &expr);

      ~TAssertCase();

      TAssertPtr GetAssert() const;

      const std::string &GetName() const;

      bool HasName() const;

      TAssertPtr TryGetAssert() const;

      void TypeCheck() const;

      private:

      TAssertCase(
          const TAssertPtr &assert,
          const Base::TOpt<std::string> &opt_name,
          const TExpr::TPtr &expr);

      std::weak_ptr<TAssert> Assert;

      Base::TOpt<std::string> OptName;

    };  // TAssertCase

    class TAssert
        : public TThatableUnary {
      NO_COPY_SEMANTICS(TAssert);
      public:

      typedef std::shared_ptr<TAssert> TPtr;

      typedef std::unordered_set<TAssertCase::TPtr> TAssertCaseSet;

      static TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      void Delete(const TAssertCase::TPtr &assert_case);

      const TAssertCaseSet &GetAssertCases() const;

      virtual Type::TType GetType() const;

      void Insert(const TAssertCase::TPtr &assert_case);

      private:

      TAssert(const TExpr::TPtr &expr, const TPosRange &pos_range);

      TAssertCaseSet AssertCases;

    };  // TAssert

  }  // Expr

}  // Stig
