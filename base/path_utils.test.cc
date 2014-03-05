/* <base/path_utils.test.cc>

   Unit test for <base/path_utils.h>.

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

#include <base/path_utils.h>

#include <sstream>
#include <string>
#include <set>

#include <fcntl.h>

#include <test/kit.h>
#include <base/dir_iter.h>
#include <base/dir_walker.h>
#include <base/error_utils.h>
#include <base/fd.h>
#include <base/tmp_dir_maker.h>

using namespace std;
using namespace placeholders;

using namespace Base;

/* Creates an empty file. */
static void CreateDummyFile(const char *path) {
  TFd(creat(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
}

/* Returns the contents of the given directory as an ordered set,
   like this: "{ file [a], dir [b], symlink [c] }".  Doesn't descend
   into directories or follow symlinks. */
static string IterDir(const char *path) {
  set<string> items;
  for (TDirIter dir_iter(path); dir_iter; ++dir_iter) {
    const char *kind_str;
    switch (dir_iter.GetKind()) {
      case TDirIter::BlockDev: {
        kind_str = "block_dev";
        break;
      }
      case TDirIter::CharDev: {
        kind_str = "char_dev";
        break;
      }
      case TDirIter::Dir: {
        kind_str = "dir";
        break;
      }
      case TDirIter::File: {
        kind_str = "file";
        break;
      }
      case TDirIter::NamedPipe: {
        kind_str = "named_pipe";
        break;
      }
      case TDirIter::Socket: {
        kind_str = "socket";
        break;
      }
      case TDirIter::SymLink: {
        kind_str = "symlink";
        break;
      }
      case TDirIter::Unknown: {
        kind_str = "unknown";
        break;
      }
    }
    ostringstream strm;
    strm << kind_str << " [" << dir_iter.GetName() << ']';
    items.insert(strm.str());
  }
  ostringstream strm;
  strm << '{';
  bool sep = false;
  for (const auto &item: items) {
    if (sep) {
      strm << ", ";
    } else {
      strm << ' ';
      sep = true;
    }
    strm << item;
  }
  strm << (sep ? " }" : "}");
  return strm.str();
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
    EXPECT_EQ(IterDir(path), "{}");
    EXPECT_EQ(WalkDir(path), "dir 0 [path_utils.test] { }");
  }
  EXPECT_FALSE(ExistsPath(path));
  /* Make the temp dir come and go with something in it. */ {
    TTmpDirMaker tmp_dir_maker(path);
    EXPECT_TRUE(ExistsPath(path));
    CreateDummyFile("/tmp/path_utils.test/dummy.txt");
    EXPECT_EQ(IterDir(path), "{ file [dummy.txt] }");
    EXPECT_EQ(WalkDir(path), "dir 0 [path_utils.test] { file [dummy.txt]; }");
  }
  EXPECT_FALSE(ExistsPath(path));
  /* Make the temp dir come and go with more somethings in it. */ {
    TTmpDirMaker tmp_dir_maker(path);
    EXPECT_TRUE(ExistsPath(path));
    CreateDummyFile("/tmp/path_utils.test/dummy1.txt");
    CreateDummyFile("/tmp/path_utils.test/dummy2.txt");
    IfLt0(symlink("/tmp/path_utils.test/dummy2.txt", "/tmp/path_utils.test/link_to_dummy2.txt"));
    EXPECT_EQ(IterDir(path), "{ file [dummy1.txt], file [dummy2.txt], symlink [link_to_dummy2.txt] }");
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
