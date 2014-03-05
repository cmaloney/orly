/* <stig/synth/test_def.h>

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
#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/scope_and_def.h>

namespace Stig {

  namespace Synth {

    /* Forward declarations */
    class TWithClause;
    class TTestCaseBlock;

    /* TODO */
    class TTestDef
        : public TDef {
      NO_COPY_SEMANTICS(TTestDef);
      public:

      /* TODO */
      TTestDef(TScope *scope, const Package::Syntax::TTestDef *test_def);

      /* TODO */
      virtual ~TTestDef();

      /* TODO */
      virtual TAction Build(int pass);

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      static unsigned int TestId;

      static TName GenName(const TPosRange &pos_range);

      /* TODO */
      const Package::Syntax::TTestDef *TestDef;

      /* TODO */
      TWithClause *OptWithClause;

      /* TODO */
      TTestCaseBlock *TestCaseBlock;

    };  // TTestDef

  }  // Synth

}  // Stig
