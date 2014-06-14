/* <jhm/test.cc>

   Utility functions for finding tests for JHM.

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

#include <jhm/test.h>

#include <base/dir_walker.h>
#include <base/stl_utils.h>
#include <jhm/naming.h>
#include <iostream>

using namespace Base;
using namespace Jhm;
using namespace std;

//NOTE: We override const char * to prevent excess construction of vectors.
bool Contains(const vector<string> &vec, const char *str) {
  return std::any_of(vec.begin(), vec.end(), [str](const string &s) { return s == str; });
}

bool Contains(const vector<string> &vec, const string &str) {
  return std::any_of(vec.begin(), vec.end(), [&str](const string &s) { return s == str; });
}

TSet<TFile *> Jhm::FindTests(TEnv &env) {
  TSet<TFile *> ret;
  // Walk the environment source directory, find all files which could be tests (extension list contains 'test')
  // Build a set of all the tests which could possibly exist.
  class test_walker_t final : public TDirWalker {
    NO_COPY(test_walker_t);
    NO_MOVE(test_walker_t);

    public:
    test_walker_t(TEnv &env, TSet<TFile *> &out)
        : Env(env), Out(out) {
          env.GetConfig().TryRead("test.excluded", ExcludedDirs);
        }

    TAction OnDirBegin(const TEntry &entry) final {
      // If the directory is excluded, skip it.
      if (Contains(ExcludedDirs, entry.Name)) {
        return TDirWalker::Skip;
      }
      return TDirWalker::Enter;
    }

    bool OnFile(const TEntry &entry) final {
      TFile *f = Env.TryGetFileFromPath(entry.AccessPath);
      assert(f); // We're walking in src. We must be able to get the file from the path.
      const TExtension &ext_list = f->GetPath().GetRelPath().GetName().GetExtensions();
      auto f_it = find(ext_list.begin(), ext_list.end(), "test");
      if (f_it != ext_list.end()) {
        // We found a test! Get the executable variant / actual test file
        Out.insert(
            Env.GetFile(f->GetPath().GetRelPath().DropExtension(ext_list.end() - f_it).AddExtension({"test", ""})));
      }
      return true;
    }

    private:
    TEnv &Env;
    TSet<TFile *> &Out;
    vector<string> ExcludedDirs;
  };

  test_walker_t(env, ret).Walk(('/' + env.GetSrc().Get()).c_str());
  return ret;
}