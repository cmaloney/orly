/* <stig/synth/cst_utils.test.cc>

   Unit test for <stig/synth/cst_utils.h>.

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

#include <stig/synth/cst_utils.h>

#include <base/no_construction.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/get_pos_range.h>
#include <test/kit.h>

using namespace Stig;
using namespace Stig::Package;
using namespace Stig::Synth;

FIXTURE(Typical) {
  Syntax::TOptDefSeq *root =
      new Syntax::TDefSeq(
        new Syntax::TBadDef(new Syntax::TSemi(1, 1, 1, 2, ";", 1)),
        new Syntax::TDefSeq(
          new Syntax::TBadDef(new Syntax::TSemi(2, 1, 2, 2, ";", 1)),
          new Syntax::TNoDefSeq()
        )
      );
  try {
    const Syntax::TOptDefSeq *a = root;
    int line_number = 1;
    ForEach<Syntax::TDef>(a,
        [&line_number](const Syntax::TDef *def) -> bool {
          const Syntax::TBadDef *bad_def = dynamic_cast<const Syntax::TBadDef *>(def);
          if (EXPECT_TRUE(bad_def)) {
            const Syntax::TSemi *semi = bad_def->GetSemi();
            assert(semi);
            if (EXPECT_EQ(semi->GetLexeme().GetPosRange().GetStart().GetLineNumber(), line_number)) {
              ++line_number;
            }
          }
          return true;
        });
    EXPECT_EQ(line_number, 3);
  } catch (...) {
    delete root;
    throw;
  }
  delete root;
}
