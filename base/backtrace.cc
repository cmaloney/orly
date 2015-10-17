/* <base/backtrace.cc>

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/backtrace.h>

#include <iostream>

#include <execinfo.h>

#include <base/as_str.h>
#include <util/error.h>

using namespace Base;
using namespace std;

/* Generate a backtrace one line at a time, calling the callback once for each backtrace frame. */
template <int max_frame_count>
void GenBacktrace(const std::function<void(const std::string &)> &cb) {
  void *frames[max_frame_count + 1];
  int frame_count = backtrace(frames, max_frame_count + 1);
  char **symbols = backtrace_symbols(frames, frame_count);
  for(int frame_idx = 1; frame_idx < frame_count; ++frame_idx) {
    // TODO(cmaloney): Align frame_idx + frame_count so that the symbol is always indented the same.
    auto frame_print = AsStr('[', frame_idx, '/', frame_count - 1, "] ");
    // TODO(cmaloney): Use debug information to print filename + line number.
    if(symbols) {
      frame_print += symbols[frame_idx];
    } else {
      frame_print += AsStr(frames[frame_idx]);
    }
    cb(frame_print);
  }
}

void Base::PrintBacktrace() {
  // 200 is fairly arbitrary. Hopefully long enough.
  GenBacktrace<200>([](const string &msg) { cout << msg << endl; });
}

void Base::SetBacktraceOnTerminate() {
  std::set_terminate([]() {
    static bool tried_throw = false;
    try {
      bool old_tried_throw = tried_throw;
      tried_throw |= true;
      if(!old_tried_throw)
        throw;
      cerr << "TERMINATE (no exception)" << endl;
    } catch(const std::exception &ex) {
      cerr << "TERMINATE (standard exception): " << ex.what() << endl;
    } catch(...) {
      cerr << "TERMINATE (unknown exception): " << endl;
    }
    cerr << "BACKTRACE" << endl;
    PrintBacktrace();
    cerr << "TERMINATED" << endl;
  });
}

[[noreturn]] static void PrintSegfaultBacktrace(int) {
  cout << "ERROR: SIGSEGV / Segfault\n"
       << "Backtrace: " << endl;
  PrintBacktrace();
  cerr << "SEGFAULT" << endl;
  ABORT();
}

[[noreturn]] static void PrintSigPipe(int) {
  cout << "ERROR: SIGPIPE" << endl;
  PrintBacktrace();
  cerr << "SIGPIPE" << endl;
  ABORT();
}

static void PrintSigabrtBacktrace(int) {
  static bool first_abrt = true;
  if(!first_abrt) {
    return;
  }
  first_abrt = false;
  cout << "ERROR: SIGABRT" << endl;
  PrintBacktrace();
  cerr << "SIGABRT" << endl;
  ABORT();
}

TBacktraceCatcher::TBacktraceCatcher()
    : Sigabrt(SIGABRT, &PrintSigabrtBacktrace),
      Sigsegv(SIGSEGV, &PrintSegfaultBacktrace),
      Sigpipe(SIGPIPE, &PrintSigPipe) {}

// Make pure virtuals print a reasonable error message
[[noreturn]] extern "C" void __cxa_pure_virtual() {
  cerr << "PURE VIRTUAL CALLED\n"
       << "BACKTRACE:" << endl;
  PrintBacktrace();
  cerr << "TERMINATED BECAUSE PURE VIRTUAL CALLED" << endl;
  ABORT();
}
