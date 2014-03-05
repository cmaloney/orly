/* <stig/synth/with_clause.h>

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

#include <functional>
#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/stig.package.cst.h>
#include <stig/symbol/test/with_clause.h>
#include <stig/synth/scope_and_def.h>

namespace Stig {

  namespace Synth {

    class TExprFactory;
    class TTestKvEntry;

    class TWithClause {
      NO_COPY_SEMANTICS(TWithClause);
      public:

      TWithClause(
          const TExprFactory *expr_factory,
          const Package::Syntax::TWithClause *with_clause);

      ~TWithClause();

      Symbol::Test::TWithClause::TPtr Build() const;

      void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      typedef std::vector<TTestKvEntry *> TTestKvEntryVec;

      void Cleanup();

      const Package::Syntax::TWithClause *WithClause;

      TTestKvEntryVec TestKvEntries;

    };  // TWithClause

  }  // Synth

}  // Stig
