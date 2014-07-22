/* <base/repl.h>

   Gives an interactive shell.

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

#include <functional>
#include <string>
#include <vector>

#include <base/thrower.h>

namespace Base {

  DEFINE_ERROR(TReplInUse,
               std::runtime_error,
               "Repl already in use. May not be run multiple times simultaneously (linenoise library isn't re-entrant)")

  //TODO: string_view
  /* Command info for autocompletion. */
  struct TInfo {

    /* Prefix of a command. Used for auto-complete. */
    const std::string Name;

    /* Example. Shown in the help message. */
    const std::string Example;

    /* Description of what the command does. */
    const std::string Desc;

  };  // TInfo

  /* Run an interactive read / eval / print loop with history and completion. */
  bool Repl(const std::string &ps1,
            const std::vector<TInfo> &completions,
            const std::function<bool(const std::string &cmd)> &process_cmd);
}
