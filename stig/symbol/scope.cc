/* <stig/symbol/scope.cc>

   Implements <stig/symbol/scope.h>

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

#include <stig/symbol/scope.h>

#include <algorithm>

#include <stig/symbol/function.h>
#include <stig/symbol/test/test.h>

using namespace Stig;
using namespace Stig::Symbol;

TScope::TScope() {}

TScope::~TScope() {}

void TScope::Add(const TFunction::TPtr &function) {
  assert(this);
  assert(&function);
  assert(function);
  auto result = Functions.insert(function);
  assert(result.second);
}

void TScope::Add(const Test::TTest::TPtr &test) {
  assert(this);
  assert(&test);
  assert(test);
  Tests.push_back(test);
}

const TScope::TFunctionSet &TScope::GetFunctions() const {
  assert(this);
  return Functions;
}

const TScope::TTests &TScope::GetTests() const {
  assert(this);
  return Tests;
}

void TScope::Remove(const TFunction::TPtr &function) {
  assert(this);
  assert(&function);
  assert(function);
  Functions.erase(function);
}

void TScope::Remove(const Test::TTest::TPtr &test) {
  assert(this);
  assert(&test);
  assert(test);
  auto result = std::find(Tests.begin(), Tests.end(), test);
  if (result != Tests.end()) {
    Tests.erase(result);
  }
}

void TScope::TypeCheck() const {
  assert(this);
  for (const auto &function : Functions) {
    function->GetType();
  }
  for (const auto &test : Tests) {
    test->TypeCheck();
  }
}
