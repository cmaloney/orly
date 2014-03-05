/* <stig/reentrant.error.test.cc>

   Unit test for re-entering a nycr generated parser after an error.

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

#include <stig/stig.command.cst.h>

#include <tools/nycr/test.h>
#include <test/kit.h>

using namespace Stig::Command::Syntax;
using namespace Tools::Nycr::Test;

FIXTURE(Typical) {
  auto cst = TCommand::ParseStr("1+2a");
  auto cst2 = TCommand::ParseStr("2b1a");
  delete cst2;
  delete cst;
}