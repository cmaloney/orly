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
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <base/fd.h>
#include <base/json.h>
#include <base/not_implemented.h>
#include <base/split.h>
#include <base/subprocess.h>
#include <base/thrower.h>
#include <strm/fd.h>
#include <strm/in.h>
#include <strm/bin/in.h>

using namespace Base;
using namespace std;

ostream &operator<<(ostream &strm, const vector<string> &that) {
  return strm << Base::Join(that, ' ');
}

// TODO: mpark could probably do this cleaner.
vector<string> ToVecStr(int argc, const char *argv[], int skip) {
  assert(argc >= skip);
  vector<string> ret(argc - skip);
  for (int i = 0; skip + i < argc; ++i) {
    ret.at(i) = string(argv[skip + i]);
  }
  return ret;
}

vector<string> GetCDeps(const string &filename, bool is_cpp, const vector<string> &extra_args) {

  // Have GCC get all the dependencies for us
  ostringstream cmd;
  cmd << (is_cpp ? "g++ " : "gcc ") << extra_args << " -M -MG " << filename;
  TPump pump;
  auto subproc = TSubprocess::New(pump, cmd.str().c_str());
  auto ret = subproc->Wait();
  if (ret != 0) {
    THROW_ERROR(runtime_error) << "Non-zero (" << ret << ") exit from command " << quoted(cmd.str());
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
  for(uint32_t i=0; i < gcc_deps.length(); ++i) {
    char &c = gcc_deps[i];

    if (!eaten_start) {
      if (c == ':') {
        eaten_start = true;
      }
      continue;
    }

    if (tok_start) {
      // Still in token?
      if (isgraph(c)) {
        continue;
      }

      // Hit end of token. Grab token and submit.
      // NOTE: If token is '\', then discard (Indicates GCC's line continuation)
      string dep(tok_start, &c-tok_start);
      tok_start = nullptr;
      if (dep != "\\") {
        if (is_first_item) {
          is_first_item = false;
        } else {
          deps.emplace_back(move(dep));
        }
      }
    } else {
      // Hit start of token?
      if (isgraph(c)) {
        tok_start = &gcc_deps[i];
      }
    }
  }

  // Build a json object out of the dependency list
  return deps;
}

// TODO: this should go in a helper somewhere...
bool EndsWith(const string &target, const string &ending) {
  if (ending.length() > target.length()) {
    return false;
  }

  return target.compare(target.size() - ending.length(), ending.length(), ending) == 0;
}

TJson ToJson(vector<string> &&that) {
  vector<TJson> json_elems(that.size());
  for(uint64_t i=0; i < that.size(); ++i) {
    json_elems[i] = TJson(move(that[i]));
  }

  return TJson(move(json_elems));
}

void MakeDepFile(const string &filename, const string &out_name, const vector<string> &extra_args) {
  TJson deps;
  // Check for extension to determine how we need to scan for dependencies
  if (EndsWith(filename, ".c")) {
    deps = ToJson(GetCDeps(filename, false, extra_args));

  } else if (EndsWith(filename, ".cc")) {
    deps = ToJson(GetCDeps(filename, true, extra_args));
  } else {
    THROW_ERROR(runtime_error) << "Unknown file extension: " << quoted(filename);
  }

  ofstream out(out_name, ios_base::out | ios_base::trunc);
  if (!out.good()) {
    THROW_ERROR(runtime_error) << "Error creating/opening file for writing: " << filename + ".dep";
  }
  out << deps;
  out.close();
}

int main(int argc, const char *argv[]) {
  if (argc < 3) {
    //TODO: add '-o' to specify output filename.
    cout << "USAGE: " << argv[0] << "input_name output_name [misc_flags]\n"
         << " Produces a JSON dependency file (.dep) containing the headers/dependencies of a given source file";
    return -1;
  }
  try {
    MakeDepFile(argv[1], argv[2], ToVecStr(argc, argv, 3));
  }
  catch (const exception &ex) {
    cerr << "EXCEPTION: " << ex.what();
    return -1;
  }
  return 0;
}
