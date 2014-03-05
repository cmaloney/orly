/* <stig/var/mutation.test.cc>

   Unit test for <stig/var/mutation.h>

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

#include <stig/var/mutation.h>

#include <stig/type/type_czar.h>
#include <stig/var.h>

#include <test/kit.h>

using namespace Stig;
using namespace Stig::Var;

Type::TTypeCzar TypeCzar;

FIXTURE(Typical) {
  TVar val(std::vector<int64_t>{1, 2, 3});
  TListChange::New(0, TMutation::New(TMutator::Assign, TVar(32l)))->Apply(val);

  EXPECT_EQ(val.As<TList>()->GetVal().at(0), TVar(32l));
  EXPECT_EQ(val.As<TList>()->GetVal().at(1), TVar(2l));
  EXPECT_EQ(val.As<TList>()->GetVal().at(2), TVar(3l));
}