/* <stig/synth/test_kv_entry.h>

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
#include <stig/stig.package.cst.h>
#include <stig/symbol/stmt/new_and_delete.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/scope_and_def.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TTestKvEntry {
      NO_COPY_SEMANTICS(TTestKvEntry);
      public:

      /* TODO */
      TTestKvEntry(
          const TExprFactory *expr_factory,
          const Package::Syntax::TTestKvEntry *test_kv_entry);

      /* TODO */
      ~TTestKvEntry();

      /* TODO */
      Symbol::Stmt::TNew::TPtr Build() const;

      /* TODO */
      void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      /* TODO */
      const Package::Syntax::TTestKvEntry *TestKvEntry;

      /* TODO */
      TExpr *Key;

      /* TODO */
      TExpr *Val;

    };  // TTestKvEntry

  }  // Synth

}  // Stig
