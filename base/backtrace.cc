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

#include <dlfcn.h>
#include <execinfo.h>

#include <iostream>

#include <base/as_str.h>
#include <base/demangle.h>
#include <util/error.h>

using namespace Base;
using namespace std;

// TODO(cmaloney): Add support for finding backtraces from debug information line tables
// (best source), with fallback to inspecting and indexing ELF symbol table.
/* Generate a backtrace one line at a time, calling the callback once for each backtrace frame. */
template <int max_frame_count>
void GenBacktrace(const std::function<void(const std::string &)> &cb, int skip_frames) {
  // Skip this frame, along with however many more frames were requested
  int frame_offset = skip_frames + 2;
  void *frames[max_frame_count + 1];
  int frame_count = backtrace(frames, max_frame_count + 1);
  char **symbols = backtrace_symbols(frames, frame_count);
  //frame_idx skips this frame, and however many extra were requested
  for(int frame_idx = frame_offset; frame_idx < frame_count; ++frame_idx) {
    // TODO(cmaloney): Align frame_idx + frame_count so that the symbol is always indented the same.
    auto frame_print = AsStr('[', frame_idx - frame_offset + 1, '/', frame_count - frame_offset, "] ");
    // TODO(cmaloney): Use debug information to print filename + line number.
    if(symbols) {
#ifdef USE_DLADDR
      Dl_info info;
      if (dladdr(frames[frame_idx], &info) && info.dli_sname) {
        if (info.dli_sname[0] == '_' && info.dli_sname[1] == 'Z') {
          frame_print += AsStr(Demangle(info.dli_sname));
        } else {
          frame_print += info.dli_sname;
        }
      } else
#endif
      {
        frame_print += symbols[frame_idx];
      }
    } else {
      frame_print += AsStr(frames[frame_idx]);
    }
    cb(frame_print);
  }
}

void Base::PrintBacktrace(int skip_frames) {
  // 200 is fairly arbitrary. Hopefully long enough.
  GenBacktrace<200>([](const string &msg) { cout << msg << endl; }, skip_frames + 1);
}

static bool caused_abort = false;

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
    PrintBacktrace(2);
    cerr << "TERMINATED" << endl;
    caused_abort = true;
    ABORT();
  });
}

[[noreturn]] static void PrintSegfaultBacktrace(int) {
  cout << "ERROR: SIGSEGV (Segmenatation fault)\n"
       << "Backtrace: " << endl;
  PrintBacktrace();
  cerr << "SEGFAULT" << endl;
  caused_abort = true;
  ABORT();
}

[[noreturn]] static void PrintSigPipe(int) {
  cout << "ERROR: SIGPIPE" << endl;
  PrintBacktrace();
  cerr << "SIGPIPE" << endl;
  caused_abort = true;
  ABORT();
}

static void PrintSigabrtBacktrace(int) {
  static bool first_abrt = true;
  if(caused_abort) {
    return;
  }
  if(!first_abrt) {
    return;
  }
  first_abrt = false;
  cout << "ERROR: SIGABRT" << endl;
  PrintBacktrace();
  cerr << "SIGABRT" << endl;
}

TBacktraceCatcher::TBacktraceCatcher()
    : Sigabrt(SIGABRT, &PrintSigabrtBacktrace),
      Sigsegv(SIGSEGV, &PrintSegfaultBacktrace),
      Sigpipe(SIGPIPE, &PrintSigPipe) {}

#ifndef _LIBCPP_VERSION
// Make pure virtuals print a reasonable error message
extern "C" [[noreturn]] void __cxa_pure_virtual() {
  cerr << "PURE VIRTUAL CALLED\n"
       << "BACKTRACE:" << endl;
  PrintBacktrace();
  cerr << "TERMINATED BECAUSE PURE VIRTUAL CALLED" << endl;
  caused_abort = true;
  ABORT();
}
#endif
