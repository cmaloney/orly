/* <base/subprocess.cc>

   Implements <base/subprocess.h>.

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

#include <base/subprocess.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <optional>

#include <base/pump.h>
#include <base/split.h>
#include <util/error.h>
#include <util/io.h>

using namespace std;
using namespace Base;
using namespace Base::Subprocess;
using namespace Util;

/* Run a subprocess and communicate with it. */
class TSubprocess final {
  public:
  // Destroying this object won't stop the child process, we'll just stop talking to it.
  ~TSubprocess() = default;

  // A CyclicBuffer which gets the child's stderr streamed to it while the
  // child is running.
  std::shared_ptr<TCyclicBuffer> GetStdError() {
    assert(this);
    return Error;
  }

  // A CyclicBuffer which gets the child's stdout streamed to it while the
  // child is running.
  std::shared_ptr<TCyclicBuffer> GetStdOutput() {
    assert(this);
    return Output;
  }

  // Wait for the child to complete and return its exit code. When this call
  // returns StdOutput and StdError will contain all data. (There is some
  // internal logic on top of just waiting for the pid).
  int Wait() const {
    assert(this);
    int status;

    // Wait for the child process.
    IfLt0(waitpid(ChildId, &status, 0));

    // Wait for stdout / stderr fds to be finished reading from.
    OutputWait.wait();
    ErrorWait.wait();

/* The Linux macros used to work with waitpid() use C-style casts, so we'll have to
   temporarily ignore their behavior. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
/* We now return you to the world of modern programming style. */
#pragma GCC diagnostic pop
  }

  // Run the command as a subprocess. Returns a TSubprocess for communicating to
  //  the child process.
  static std::unique_ptr<TSubprocess> New(TPump &pump, const std::vector<std::string> &cmd) {
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2070
    auto subprocess = std::unique_ptr<TSubprocess>(new TSubprocess(pump));
    if (!subprocess->ChildId) {
      subprocess.reset();
      Exec(cmd);
    }
    return subprocess;
  }

  /* Construct the pipes and fork. */
  TSubprocess(TPump &pump)
      : Output(make_shared<TCyclicBuffer>()), Error(make_shared<TCyclicBuffer>()) {
    assert(&pump);

    /* Make the in/out/err pipes, then fork. */
    // TODO(cmaloney): Make the input buffer passable by callers.
    auto stdin_buffer = make_shared<TCyclicBuffer>();
    TFd stdin = pump.NewReadFromBuffer(stdin_buffer);

    TFd stdout, stderr;
    tie(stdout, OutputWait) = pump.NewWriteToBuffer(Output);
    tie(stderr, ErrorWait) = pump.NewWriteToBuffer(Error);
    IfLt0(ChildId = fork());
    if (!ChildId) {
      // child. duplicate the pipes into their normal fd numbers.
      IfLt0(dup2(stdin, 0));
      IfLt0(dup2(stdout, 1));
      IfLt0(dup2(stderr, 2));

      // Close the extra / duplicate fds.
      stdin.Reset();
      stdout.Reset();
      stderr.Reset();
    }
  }

  int ChildId;

  std::shared_ptr<TCyclicBuffer> Output, Error;
  std::future<void> OutputWait, ErrorWait;
};  // TSubprocess

static bool InternalEchoCommands(optional<bool> enable) {
  static bool Enable = false;

  if (enable.has_value()) {
    Enable = *enable;
  }

  return Enable;
}

void Base::Subprocess::Exec(const vector<string> &cmd) {
  assert(&cmd);

  // TODO(cmaloney): de-dup this between here and dep_clang.cc::GetDepsUsingClang.
  // NOTE: This is definitely less efficient than strictly necessary, but we
  // don't actually copy the strings, just pointers, so not too bad.
  unique_ptr<const char *[]> argv(new const char *[cmd.size() + 1]);
  argv[cmd.size()] = nullptr;
  for (uint64_t i = 0; i < cmd.size(); ++i) {
    argv[i] = cmd[i].c_str();
  }
  // NOTE: const_cast is unsafe. In this case though, we're going out of existence, so who cares.
  IfLt0(execvp(cmd[0].c_str(), const_cast<char **>(argv.get())));
  throw;
}

TResult Base::Subprocess::Run(TPump &pump, const std::vector<std::string> &cmd) {
  if (InternalEchoCommands(optional<bool>())) {
    // TODO(cmaloney): Make it so copy / paste will get exact set of arguments from bash.
    cout << AsStr("Base::Subprocess::Exec([", Join(cmd, " "), "])\n");
  }

  auto subprocess = TSubprocess::New(pump, cmd);
  int exit_code = subprocess->Wait();

  return TResult{exit_code, subprocess->GetStdOutput(), subprocess->GetStdError()};
}

TResult Base::Subprocess::Run(const std::vector<std::string> &cmd) {
  TPump pump;
  return Run(pump, cmd);
}

int Base::Subprocess::WaitAll() {
  siginfo_t status;
  IfNe0(waitid(P_ALL, 0, &status, WEXITED | WNOWAIT));
  return status.si_pid;
}

void Base::Subprocess::SetEchoCommands(bool enable) { InternalEchoCommands(enable); }
