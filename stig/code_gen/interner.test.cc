/* <stig/code_gen/interner.test.cc>

   Unit test for <stig/code_gen/interner.h>

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

#include <stig/code_gen/interner.h>

#include <stig/code_gen/binary.h>
#include <stig/code_gen/literal.h>
#include <stig/type.h>
#include <stig/type/type_czar.h>

#include <test/kit.h>

using namespace Stig;
using namespace Stig::CodeGen;

FIXTURE(Typical) {
  Type::TTypeCzar type_czar;
  TIdScope::TPtr id_s = TIdScope::New();
  TCodeScope s(id_s);

  auto &Interner = *(s.GetInterner());

  auto lit1 = Interner.GetLiteral(nullptr, Var::TVar(1));
  auto lit2 = Interner.GetLiteral(nullptr, Var::TVar(2));
  auto lit3 = Interner.GetLiteral(nullptr, Var::TVar(3));
  auto lit4 = Interner.GetLiteral(nullptr, Var::TVar(4));
  auto lit4a = Interner.GetLiteral(nullptr, Var::TVar(4));

  auto bin1 = Interner.GetBinary(nullptr, Type::TInt::Get(), TBinary::Add, lit1, lit4);
  auto bin2 = Interner.GetBinary(nullptr, Type::TInt::Get(), TBinary::Add, lit3, lit2);
  auto bin3 = Interner.GetBinary(nullptr, Type::TInt::Get(), TBinary::Sub, lit1, lit4);
  auto bin4 = Interner.GetBinary(nullptr, Type::TInt::Get(), TBinary::Add, lit1, lit4);

  EXPECT_EQ(lit4, lit4a);
  EXPECT_EQ(bin1, bin4);
  EXPECT_NE(bin1, bin2);
  EXPECT_NE(bin1, bin3);
  EXPECT_NE(bin2, bin3);
  EXPECT_NE(bin2, bin4);
  EXPECT_NE(bin3, bin4);
  //NOTE: Locals have been disabled ATM, so the number of locals is 0.
  EXPECT_EQ(s.GetLocals().size(), 0u);

  //TODO: Test printing out the locals.

}