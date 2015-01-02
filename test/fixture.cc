/* <test/fixture.cc>

   Implements <test/fixture.h>.

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

#include <test/fixture.h>

using namespace Test;


TFixture::TFixture(Base::TCodeLocation code_location, const char *name, const TFunc func)
      : CodeLocation(code_location), Name(name), Func(func) {
  AddFixture(this);
}

static std::vector<const TFixture*> &Fixtures() {
  static std::vector<const TFixture*> Fixtures;

  return Fixtures;
}

const std::vector<const TFixture*> &Test::GetFixtures() {
  return Fixtures();
}

void Test::AddFixture(const TFixture *fixture) {
  Fixtures().push_back(fixture);
}
