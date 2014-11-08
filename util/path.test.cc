/* <util/path.test.cc>

   Unit test for <util/path.h>.

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

#include <util/path.h>

#include <sstream>
#include <string>
#include <set>

#include <fcntl.h>

#include <test/kit.h>
#include <base/dir_walker.h>
#include <base/fd.h>
#include <base/tmp_copy_to_file.h>
#include <base/tmp_dir_maker.h>
#include <util/error.h>
#include <util/io.h>

using namespace Base;
using namespace std;
using namespace std::placeholders;
using namespace Util;

/* Creates an empty file. */
static void CreateDummyFile(const char *path) {
  TFd(creat(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
}

/* Returns the contents of the given directory, ordering entries within each directory by name.
   Descends into subdirectories but doesn't follow symlinks.  Gives the depth of directories.
   Looks like this: "dir 0 [root] { file [a]; dir 1 [b] { file [b1]; } file [c]; }". */
static string WalkDir(const char *path) {
  class dir_walker_t final : public TDirWalker {
    public:
    dir_walker_t(ostream &strm) : Strm(strm) {}
    private:
    virtual bool OnBlockDev(const TEntry &entry) override {
      Strm << " block_dev [" << entry.Name << "];";
      return true;
    }
    virtual bool OnCharDev(const TEntry &entry) override {
      Strm << " char_dev [" << entry.Name << "];";
      return true;
    }
    virtual TAction OnDirBegin(const TEntry &entry) override {
      Strm << " dir " << entry.Depth << " [" << entry.Name << "] {";
      return Enter;
    }
    virtual bool OnDirCycle(const TEntry &entry, const TEntry &cycle_entry) override {
      Strm << " cycle_dir [" << entry.Name << "] [" << cycle_entry.RootPath << "];";
      return true;
    }
    virtual bool OnDirEnd(const TEntry &) override {
      Strm << " }";
      return true;
    }
    virtual bool OnFile(const TEntry &entry) override {
      Strm << " file [" << entry.Name << "];";
      return true;
    }
    virtual bool OnNamedPipe(const TEntry &entry) override {
      Strm << " named_pipe [" << entry.Name << "];";
      return true;
    }
    virtual bool OnSocket(const TEntry &entry) override {
      Strm << " socket [" << entry.Name << "];";
      return true;
    }
    virtual TAction OnSymLink(const TEntry &entry) override {
      Strm << " symlink [" << entry.Name << "];";
      return Skip;
    }
    virtual bool OnUnknown(const TEntry &entry) override {
      Strm << " unknown [" << entry.Name << "];";
      return true;
    }
    ostream &Strm;
  };
  ostringstream strm;
  dir_walker_t(strm).Walk(path);
  return strm.str().substr(1);
}

FIXTURE(TmpIterAndWalk) {
  const char *path = "/tmp/path_utils.test";
  /* Make the temp dir come and go with nothing in it. */ {
    TTmpDirMaker tmp_dir_maker(path);
    EXPECT_TRUE(ExistsPath(path));
    EXPECT_EQ(WalkDir(path), "dir 0 [path_utils.test] { }");
  }
  EXPECT_FALSE(ExistsPath(path));
  /* Make the temp dir come and go with something in it. */ {
    TTmpDirMaker tmp_dir_maker(path);
    EXPECT_TRUE(ExistsPath(path));
    CreateDummyFile("/tmp/path_utils.test/dummy.txt");
    EXPECT_EQ(WalkDir(path), "dir 0 [path_utils.test] { file [dummy.txt]; }");
  }
  EXPECT_FALSE(ExistsPath(path));
  /* Make the temp dir come and go with more somethings in it. */ {
    TTmpDirMaker tmp_dir_maker(path);
    EXPECT_TRUE(ExistsPath(path));
    CreateDummyFile("/tmp/path_utils.test/dummy1.txt");
    CreateDummyFile("/tmp/path_utils.test/dummy2.txt");
    IfLt0(symlink("/tmp/path_utils.test/dummy2.txt", "/tmp/path_utils.test/link_to_dummy2.txt"));
    EXPECT_EQ(WalkDir(path), "dir 0 [path_utils.test] { file [dummy1.txt]; file [dummy2.txt]; symlink [link_to_dummy2.txt]; }");
  }
  EXPECT_FALSE(ExistsPath(path));
}

FIXTURE(MakePath) {
  EXPECT_EQ(MakePath(true, { "usr", "include" }, { "syslog", ".h" }), "/usr/include/syslog.h");
  EXPECT_EQ(MakePath(true, { "///usr///", "/include/" }, { "syslog", ".h" }), "/usr/include/syslog.h");
  EXPECT_EQ(MakePath(false, {}, { "just_this" }), "just_this");
  EXPECT_EQ(MakePath(true, {}, { "abs_just_this" }), "/abs_just_this");
}

FIXTURE(TmpCopyToFile) {
  const string expected = "I'm safe in the tmp file.";
  TTmpDirMaker tmp_dir_maker("/tmp/path_utils.test/tmp_copy_to_file");
  string path;
  /* extra */ {
    TTmpCopyToFile tmp_copy_to_file(tmp_dir_maker.GetPath(), expected);
    path = tmp_copy_to_file.GetPath();
    if (EXPECT_TRUE(ExistsPath(path.c_str()))) {
      TFd fd(open(path.c_str(), O_RDONLY));
      char actual[128];
      size_t size = ReadAtMost(fd, actual, sizeof(actual) - 1);
      actual[size] = '\0';
      EXPECT_EQ(string(actual), expected);
    }
  }
  EXPECT_FALSE(ExistsPath(path.c_str()));
}

FIXTURE(GetCwd) {
  // Basic sanity check
  EXPECT_NE(GetCwd().back(), '/');
}