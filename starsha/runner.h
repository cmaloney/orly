/* <starsha/runner.h>

   TODO

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

#pragma once

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

#include <base/class_traits.h>

namespace Starsha {

  class TRunError : public std::runtime_error {
    public:
     TRunError(int exit_code, const std::string &&stderr)
         : std::runtime_error("Error running subprocess"), ExitCode(exit_code), StdErr(std::move(stderr)) {}

    const char *what() const noexcept final {
      //TODO: Make this be more useful. / include the return code.
      return StdErr.c_str();
    }

    int GetExitCode() const {
      assert(this);
      return ExitCode;
    }

    const std::string &GetStdErr() const {
      assert(this);
      return StdErr;
    }

    private:
    int ExitCode;
    std::string StdErr;

  };

  /* Runs the given command, returning the lines from std::out one by one. Throws if anything in stderr or return code
     is non-zero. */
  void Run(const std::string &cmd, std::vector<std::string> &lines, bool print_cmd);

}  // Starsha
