/* <base/subprocess.cc>

   Implements <base/subprocess.h>.

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

#include <base/subprocess.h>

#include <unistd.h>
#include <sys/wait.h>

#include <base/error_utils.h>

using namespace std;
using namespace Base;

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

void TSubprocess::Exec(const char *cmd) {
  static const char *sh = "/bin/sh";
  IfLt0(execlp(sh, sh, "-c", cmd, nullptr));
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
    StdInToChild.Reset();
    StdOutFromChild.Reset();
    StdErrFromChild.Reset();
  }
}
