/* <test/runner.cc>

   Implements <test/runner.h>.

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

#include <test/runner.h>

#include <cassert>
#include <exception>

#include <base/demangle.h>

#include <test/fixture.h>

using namespace std;
using namespace Base;
using namespace Test;

TRunner::TExpect::~TExpect() {}

void TRunner::TExpect::PreDtor() {
  assert(this);
  GetRunner()->OnExpectDtor(this);
}

TRunner::TRunner(TApp *app, const TFixture *fixture)
    : TApp::TRunner(app), Fixture(AssertTrue(fixture)), Pass(true) {
  assert(!Runner);
  TApp::TLogger() << "begin " << fixture->GetName();
  Runner = this;
}

TRunner::~TRunner() {
  assert(this);
  assert(Runner == this);
  PreDtor();
  TApp::TLogger(!Pass)
      << "end " << Fixture->GetName()
      << "; " << (Pass ? "pass" : "fail");
  Runner = 0;
}

TRunner::operator bool() const {
  assert(this);
  return Pass;
}

void TRunner::Run() {
  assert(this);
  Pass = true;
  try {
    (*Fixture->GetFunc())();
  } catch (const exception &ex) {
    TApp::TLogger logger(true);
    logger << "exception";
    try {
      logger<<"("<<TDemangle(typeid(ex)).Get()<<")";
    } catch (const std::exception &ex) {
    }

    logger <<": "<< ex.what();
    Pass = false;
  } catch (...) {
    TApp::TLogger(true) << "unknown exception";
    Pass = false;
    throw;
  }
}

void TRunner::OnExpectDtor(const TExpect *expect) {
  assert(this);
  assert(expect);
  Pass = Pass && *expect;
}

TRunner *TRunner::Runner = 0;

void TApp::TRunner::Run(TApp *app, const TFixture *fixture) {
  Test::TRunner(app, fixture).Run();
}
