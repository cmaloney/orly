/* <base/backtrace.cc>

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

#include <base/backtrace.h>

#include <iostream>

#include <execinfo.h>

#include <base/as_str.h>

using namespace Base;
using namespace std;

void Base::PrintBacktrace(int max_frame_count) {
  GenBacktrace(max_frame_count, [] (const string &msg) {
    cout << msg << endl;
    return true;
  });
}

void Base::GenBacktrace(int max_frame_count, const function<void (const string &)> &cb) {
  void *frames[max_frame_count+1];
  int frame_count = backtrace(frames, max_frame_count+1);
  char **symbols = backtrace_symbols(frames, frame_count);
  for (int frame_idx = 1; frame_idx < frame_count; ++frame_idx) {
    auto frame_print = AsStr('[', frame_idx, '/', frame_count-1, "][");
    if (symbols) {
      frame_print += symbols[frame_idx];
    } else {
      frame_print += AsStr(frames[frame_idx]);
    }
    cb(frame_print);
  }
}

void Base::SetBacktraceOnTerminate() {
  std::set_terminate([]() {
    static bool tried_throw = false;
    try {
      if (!tried_throw++) throw;
      cerr << "TERMINATE (no exception)" << endl;
    } catch (const std::exception &ex) {
      cerr << "TERMINATE (standard exception): " << ex.what() << endl;
    } catch (...) {
      cerr << "TERMINATE (unknown exception): " << endl;
    }
    cerr << "BACKTRACE" << endl;
    PrintBacktrace(100);
    cerr << "TERMINATED" << endl;
  });
}