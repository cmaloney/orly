/* <stig/code_gen/obj.test.cc>

   Unit test for <stig/code_gen/obj.h>.

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

#include <stig/code_gen/obj.h>

#include <stig/type.h>
#include <stig/type/type_czar.h>

#include <test/kit.h>

using namespace Stig;
using namespace Stig::CodeGen;
using namespace Stig::Type;

FIXTURE(GenObjHeader) {
  TTypeCzar type_czar;

  //TODO: Should really make sure the genned code is valid here, but oh well.
  //A basic object
  auto obj1 = TObj::Get({{"a", TInt::Get()}, {"b", TBool::Get()}});
  GenObjHeader(Jhm::TAbsBase("/tmp/"), obj1);

  //An object full of complex types.
  auto obj2 = TObj::Get({{"a", TDict::Get(TInt::Get(), TBool::Get())}, {"b", TSet::Get(TBool::Get())}, {"c", TList::Get(TInt::Get())}});
  GenObjHeader(Jhm::TAbsBase("/tmp/"), obj2);

  //An object with more interesting variable
  auto obj3 = TObj::Get({{"mutable", obj2}, {"irtual", TInt::Get()}, {"float", TInt::Get()}});
  GenObjHeader(Jhm::TAbsBase("/tmp/"), obj3);
}