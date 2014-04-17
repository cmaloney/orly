/* <orly/expr/thatable_unary.h>

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

#include <memory>

#include <base/class_traits.h>
#include <orly/expr/thatable.h>
#include <orly/expr/unary.h>
#include <orly/pos_range.h>

namespace Orly {

  namespace Expr {

    class TThatableUnary
        : public TThatable, public TUnary {
      NO_COPY(TThatableUnary);
      public:

      typedef std::shared_ptr<TThatableUnary> TPtr;

      Type::TType GetThatType() const;

      protected:

      TThatableUnary(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TThatableUnary

  }  // Expr

}  // Orly
