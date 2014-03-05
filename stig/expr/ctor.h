/* <stig/expr/ctor.h>

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

#include <base/no_copy_semantics.h>
#include <stig/expr/n_ary.h>
#include <stig/pos_range.h>

namespace Stig {

  namespace Expr {

    template <typename TContainer>
    class TCtor
        : public TNAry<TContainer> {
      NO_COPY_SEMANTICS(TCtor);
      public:

      protected:

      /* Empty constructor */
      TCtor(const TPosRange &pos_range)
          : TNAry<TContainer>(pos_range) {}

      /* Non-empty constructor */
      TCtor(const TContainer &exprs, const TPosRange &pos_range)
          : TNAry<TContainer>(exprs, pos_range) {}

    };  // TCtor

  }  // Expr

}  // Stig
