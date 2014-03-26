/* <starsha/runner.cc>

   Implements <starsha/runner.h>.

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

#include <starsha/runner.h>

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

#include <unistd.h>
#include <sys/wait.h>

#include <base/os_error.h>
#include <base/thrower.h>

using namespace std;
using namespace Base;
using namespace Starsha;

extern bool PrintCmds;

TRunner::TRunner(const string &cmd, size_t buffer_size)
    : Poller(EPOLL_CLOEXEC), OutParser(buffer_size), ErrParser(buffer_size) {
  if (PrintCmds) {
    cout << cmd << '\n';
  }
  TOsError::IfLt0(HERE, ChildId = fork());
  if (ChildId) {
    InPipe.Close(TPipe::In);
    OutPipe.Close(TPipe::Out);
    ErrPipe.Close(TPipe::Out);
    Poller.Add(&OutPipe);
    Poller.Add(&ErrPipe);
  } else {
    try {
      ErrPipe.MoveFd(TPipe::Out, 2);
      OutPipe.MoveFd(TPipe::Out, 1);
      InPipe.MoveFd(TPipe::In, 0);
      TOsError::IfLt0(HERE, execlp("/bin/sh", "/bin/sh", "-c", cmd.c_str(), NULL));
    } catch (const exception &ex) {
      cerr << "exception: " << ex.what() << endl;
      abort();
    } catch (...) {
      cerr << "exception: unknown error" << endl;
      abort();
    }
  }
}

TRunner::~TRunner() {}

bool TRunner::ForEachLine(const function<bool (bool is_err, const char *line)> &cb) {
  assert(this);
  assert(&cb);
  struct {
    TParser *Parser;
    bool IsErr;
  } items[] = { { &OutParser, false }, { &ErrParser, true }, { 0, false } };
  TPipe *pipe;
  for (;;) {
    for (auto item = items; item->Parser; ++item) {
      for (;;) {
        auto line = item->Parser->Peek();
        if (!line) {
          break;
        }
        if (!cb(item->IsErr, line)) {
          return false;
        }
        item->Parser->Pop();
      }
    }
    pipe = Poller.Wait();
    if (!pipe) {
      break;
    }
    ((pipe == &OutPipe) ? OutParser : ErrParser).Push(pipe);
  }
  return true;
}

void TRunner::Send(const char *data, size_t size) {
  assert(this);
  InPipe.Write(data, size);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
int TRunner::Wait() {
  assert(this);
  int status;
  TOsError::IfLt0(HERE, waitpid(ChildId, &status, 0));
  return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
#pragma GCC diagnostic pop

TRunner::TParser::TParser(size_t buffer_size) {
  Start = new char[buffer_size];
  Limit = Start + buffer_size;
  PushStart = Start;
  Done = false;
  PeekStart = Start;
  PeekLimit = Start;
  CanPeek = false;
}

TRunner::TParser::~TParser() {
  assert(this);
  delete [] Start;
}

const char *TRunner::TParser::Peek() const {
  assert(this);
  Freshen();
  return CanPeek ? PeekStart : 0;
}

void TRunner::TParser::Pop() {
  assert(this);
  Freshen();
  PeekStart = PeekLimit + 1;
  PeekLimit = PeekStart;
  CanPeek = false;
}

void TRunner::TParser::Push(TPipe *pipe) {
  assert(this);
  assert(pipe);
  if (PushStart >= Limit && PeekStart > Start) {
    memmove(Start, PeekStart, PeekLimit - PeekStart);
    size_t shift_size = PeekStart - Start;
    PeekStart = Start;
    PeekLimit -= shift_size;
    PushStart -= shift_size;
  }
  size_t max_size = Limit - PushStart;
  if (!max_size) {
    THROW << "parser buffer overflow";
  }
  size_t size = pipe->Read(PushStart, max_size);
  if (size) {
    PushStart += size;
  } else {
    *PushStart = '\0';
    Done = true;
  }
}

void TRunner::TParser::Freshen() const {
  assert(this);
  if (!CanPeek) {
    for (; PeekLimit < PushStart; ++PeekLimit) {
      if (*PeekLimit == '\n') {
        *PeekLimit = '\0';
        CanPeek = true;
        break;
      }
    }
    if (!CanPeek && Done && PeekStart < PushStart) {
      PeekLimit = PushStart;
      CanPeek = true;
    }
  }
}

void Starsha::Run(const string &cmd, vector<string> &lines) {
  assert(&lines);
  vector<string> errors;
  int status;
  /* extra */ {
    TRunner runner(cmd);
    runner.ForEachLine([&lines, &errors](bool is_err, const char *line) {
      (is_err ? errors : lines).push_back(string(line));
      return true;
    });
    status = runner.Wait();
  }
  if (status || !errors.empty()) {
    ostringstream strm;
    if (!errors.empty()) {
      bool sep_flag = false;
      for (const string &error: errors) {
        if (sep_flag) {
          strm << endl;
        } else {
          sep_flag = true;
        }
        strm << error;
      }
    } else {
      strm << "status = " << status;
    }
    throw runtime_error(strm.str());
  }
}
