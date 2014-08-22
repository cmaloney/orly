/* <base/subprocess.cc>

   Implements <base/subprocess.h>.

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

#include <base/subprocess.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <iostream>

#include <strm/bin/in.h>
#include <strm/fd.h>
#include <util/error.h>

using namespace std;
using namespace Base;
using namespace Util;

int TSubprocess::WaitAll() {
  siginfo_t status;
  IfNe0(waitid(P_ALL, 0, &status, WEXITED | WNOWAIT));
  return status.si_pid;
}

int TSubprocess::Wait() const {
  assert(this);
  int status;
  IfLt0(waitpid(ChildId, &status, 0));
  /* The Linux macros used to work with waitpid() use C-style casts, so we'll have to
     temporarily ignore their bad behavior. */
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wold-style-cast"
  return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
  /* We now return you to the world of modern programming style. */
  #pragma GCC diagnostic pop
}

void TSubprocess::ExecStr(const char *cmd) {
  static const char *sh = "/bin/sh";
  IfLt0(execl(sh, sh, "-c", cmd, nullptr));
  throw;
}

void TSubprocess::Exec(const vector<string> &cmd) {
  assert(&cmd);
  // NOTE: This is lots of copies. We're going to get all overwritten / eaten by the child though, so no big worries.
  // Build an array

  const char *argv[cmd.size() + 1];
  argv[cmd.size()] = nullptr;
  for (uint64_t i = 0; i < cmd.size(); ++i) {
    argv[i] = cmd[i].c_str();
  }
  // NOTE: const_cast is unsafe. In this case though, we're going out of existence, so who cares.
  IfLt0(execvp(cmd[0].c_str(), const_cast<char **>(argv)));
  throw;
}

TSubprocess::TSubprocess(TPump &pump) {
  assert(&pump);
  /* Make the in/out/err pipes, then fork. */
  TFd stdin, stdout, stderr;
  pump.NewPipe(stdin, StdInToChild);
  pump.NewPipe(StdOutFromChild, stdout);
  pump.NewPipe(StdErrFromChild, stderr);
  IfLt0(ChildId = fork());
  if (!ChildId) {
    /* We're the child.  Dupe the pipes into their normal positions. */
    IfLt0(dup2(stdin,  0));
    IfLt0(dup2(stdout, 1));
    IfLt0(dup2(stderr, 2));
    /* Get rid of the extra fds. */
    stdin.Reset();
    stdout.Reset();
    stderr.Reset();
  }
}

void Base::EchoOutput(TFd &&fd) {
  Strm::TFdDefault in_prod(move(fd));
  Strm::Bin::TIn in_cons(&in_prod);
  uint8_t buf[4096];

  // Copy everything to cout error message
  while(size_t read = in_cons.TryRead(buf, 4096)) {
    WriteExactly(STDOUT_FILENO, buf, read);
  }
}