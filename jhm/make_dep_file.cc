/* <jhm/make_dep_file.cc>

   Reads the given file and writes out a JSON dependency file ('.dep') for it.

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

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <base/json.h>
#include <base/split.h>
#include <base/subprocess.h>
#include <base/thrower.h>
#include <cmd/main.h>
#include <cmd/parse.h>

using namespace Base;
using namespace Cmd;
using namespace std;

vector<string> GetCDeps(const string &filename, vector<string> cmd) {
  cmd.push_back("-M");
  cmd.push_back("-MG");
  cmd.push_back(filename);
  TPump pump;
  auto subproc = TSubprocess::New(pump, cmd);
  auto ret = subproc->Wait();
  if(ret != 0) {
    EchoOutput(subproc->TakeStdOutFromChild());
    EchoOutput(subproc->TakeStdErrFromChild());
    // TODO: Join the arguments some less-mistrewn way than ' '
    THROWER(runtime_error) << "Non-zero (" << ret << ") exit from command "
                           << quoted(AsStr(Join(cmd, ' ')));
  }

  // Read in the whole file / all the text from gcc
  string gcc_deps = ReadAll(subproc->TakeStdOutFromChild());

  vector<string> deps;

  char *tok_start = nullptr;
  bool eaten_start = false;
  bool is_first_item = true;

  // Convert it to a happy format
  // Remove leading .o:
  // Remove leading source file (Comes right after the ':')
  // Grab each token as a string.
  // If the string is '\' then it's a linebreak GCC added...
  for(uint32_t i = 0; i < gcc_deps.length(); ++i) {
    char &c = gcc_deps[i];

    if(!eaten_start) {
      if(c == ':') {
        eaten_start = true;
      }
      continue;
    }

    if(tok_start) {
      // Still in token?
      if(isgraph(c)) {
        continue;
      }

      // Hit end of token. Grab token and submit.
      // NOTE: If token is '\', then discard (Indicates GCC's line continuation)
      string dep(tok_start, size_t(&c - tok_start));
      tok_start = nullptr;
      if(dep != "\\") {
        if(is_first_item) {
          is_first_item = false;
        } else {
          deps.emplace_back(move(dep));
        }
      }
    } else {
      // Hit start of token?
      if(isgraph(c)) {
        tok_start = &gcc_deps[i];
      }
    }
  }

  return deps;
}

// TODO: this should go in a helper somewhere...
bool EndsWith(const string &target, const string &ending) {
  if(ending.length() > target.length()) {
    return false;
  }

  return target.compare(target.size() - ending.length(), ending.length(), ending) == 0;
}

TJson ToJson(vector<string> &&that) {
  vector<TJson> json_elems(that.size());
  for(uint64_t i = 0; i < that.size(); ++i) {
    json_elems[i] = TJson(move(that[i]));
  }

  return TJson(move(json_elems));
}

void MakeDepFile(const string &filename, const string &out_name, const vector<string> &extra_args) {
  TJson deps;
  // Check for extension to determine how we need to scan for dependencies
  if(EndsWith(filename, ".c") || EndsWith(filename, ".cc")) {
    deps = ToJson(GetCDeps(filename, extra_args));
  } else {
    THROWER(runtime_error) << "Unknown file extension: " << quoted(filename);
  }

  ofstream out(out_name, ios_base::out | ios_base::trunc);
  if(!out.good()) {
    THROWER(runtime_error) << "Error creating/opening file for writing: " << filename + ".dep";
  }
  out << deps;
  out.close();
}

struct TOptions {
  std::string Input;
  std::string Output;
  std::string Tool;
  std::vector<std::string> MiscFlags;
};

int Main(int argc, char *argv[]) {
  TOptions options = Parse(
      TArgs<TOptions>{Required(&TOptions::Input, "input", "Filename to get dependencies of"),
                      Required(&TOptions::Output, "output", "Filename to write dependencies to"),
                      Optional(&TOptions::MiscFlags,
                               "misc_flags",
                               "Additional flags to pass to the dependency generation tool.")},
      argc,
      argv);

  MakeDepFile(options.Input, options.Output, options.MiscFlags);
  return 0;
}
