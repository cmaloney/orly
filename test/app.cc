/* <test/app.cc>

   Implements <test/app.h>.

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

#include <test/app.h>

#include <cstdlib>
#include <iostream>
#include <time.h>

#include <base/cmd.h>
#include <base/backtrace.h>
#include <signal/handler_installer.h>
#include <test/fixture.h>
#include <test/is_in_test.h>

using namespace std;
using namespace Base;
using namespace Test;

void Test::ExtraInit() {
  AvoidTheseWheneverPossible::MarkAsInTest();
}

TApp::TRunner::~TRunner() {}

void TApp::TRunner::PreDtor() {
  assert(this);
  App->OnRunnerDtor(this);
}

void TApp::OnRunnerDtor(const TRunner *runner) {
  assert(this);
  assert(runner);
  ++(*runner ? PassCount : FailCount);
}

int TApp::Run() {
  assert(this);
  Verbose = Cmd.VerboseMember;

  chrono::time_point<chrono::system_clock> start;

  for (const TFixture *fixture = TFixture::GetFirstFixture();
       fixture; fixture = fixture->GetNextFixture()) {
    //If timing, then start timer here
    if (Cmd.PrintTiming) {
      start = chrono::system_clock::now();
    }

    TRunner::Run(this, fixture);
    if (Cmd.PrintTiming) {
      auto elapsed = chrono::system_clock::now() - start;
      //Print ts_stop - ts_start
      TLogger(true) << fixture->GetName() << ':' << elapsed.count();
    }
  }
  bool success = !FailCount;
  Verbose = true;
  TLogger(!success) << "passed " << PassCount << ", failed " << FailCount;

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void PrintSegfaultBacktrace(int) {
  cout << "ERROR: SIGSEGV / Segfault\n"
       << "Backtrace: " << endl;
  PrintBacktrace(500);
  cerr << "SEGFAULT" << endl;
  Util::Abort(HERE);
}

static void PrintSigPipe(int) {
  cout << "ERROR: SIGPIPE" << endl;
  PrintBacktrace(500);
  cerr << "SIGPIPE" << endl;
  Util::Abort(HERE);
}

int main(int argc, char *argv[]) {
  SetBacktraceOnTerminate();
  Signal::THandlerInstaller sigsegv(SIGSEGV, &PrintSegfaultBacktrace);
  Signal::THandlerInstaller sigpipe(SIGPIPE, &PrintSigPipe);
  ExtraInit();
  TApp::TCmd cmd(argc, argv);
  TApp app(cmd);
  return app.Run();
}

bool TApp::Verbose = false;

TApp *TApp::App = nullptr;
