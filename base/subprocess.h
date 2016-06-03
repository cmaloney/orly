/* <base/subprocess.h>

   Run a subprocess and communicate with it.

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

#pragma once

#include <string>
#include <vector>

#include <base/cyclic_buffer.h>

namespace Base {

// A simple event loop, see <base/pump.h>.
class TPump;

namespace Subprocess {

//  Execute the given command in a shell.  This will replace the calling process
//  entirely with the shell process (and hence never return).
[[noreturn]] void Exec(const std::vector<std::string> &cmd);

struct TResult {
  int ExitCode;
  std::shared_ptr<TCyclicBuffer> Output, Error;
};

// TODO(cmaloney): Add a variant which doesn't block until completion but
// instead returns immediately and adds all bits of the subprocess to just run
// inside of the given event loop, as well as returning stdout, stderr which can
// be used well / effectively for piping into other things that read off of
// write events.

// Run the subprocess and return it's exit state + buffers which contain its
// output.
TResult Run(const std::vector<std::string> &cmd);

// Run the subprocess and return it's exit state + buffers which contain its
// output. Use an existing event loop to pump through the input / output.
TResult Run(TPump &pump, const std::vector<std::string> &cmd);

// Returns the pid of the process which exited / work us up.
int WaitAll();

void SetEchoCommands(bool enable);

};  // namespace Subprocess

}  // Base
