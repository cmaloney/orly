/* <base/glob.test.cc>

   Unit test for <base/glob.h>.

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

#include <base/glob.h>

#include <cstdio>
#include <cstring>
#include <set>
#include <sstream>
#include <string>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>

#include <base/error_utils.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

/* Create an empty file with the given name in the given directory. */
static void CreateFile(const char *dir, const char *name) {
  char path[PATH_MAX];
  sprintf(path, "%s/%s", dir, name);
  int fd;
  IfLt0(fd = creat(path, 0777));
  close(fd);
}

/* Create a 'log' directory in the given parent directory and for the given day in March. */
static void CreateLogDir(const char *dir, int day) {
  char path[PATH_MAX];
  sprintf(path, "%s/2013.03.%02d", dir, day);
  IfLt0(mkdir(path, 0777));
  CreateFile(path, "1001.log");
  CreateFile(path, "1002.log");
  CreateFile(path, "1003.log");
}

/* Create a directory with the give name under the temp root.
   Leave the name in the given buffer. */
static void CreateTopDir(char *buf, const char *root, const char *name) {
  sprintf(buf, "%s/%s", root, name);
  IfLt0(mkdir(buf, 0777));
  CreateFile(buf, "info.dat");
  CreateFile(buf, "info.txt");
  CreateFile(buf, "readme.txt");
}

/* Wipe out the whole temp tree. */
static void RemoveAll(const char *root) {
  char cmd[PATH_MAX];
  sprintf(cmd, "rm -rf %s", root);
  system(cmd);
}

/* Run Glob() against the temp tree, searching for the given relative pattern.
   If 'is_abs' is true, convert the pattern to an absolute pattern before running Glob().
   Return the result as a string like this: "[ a, b, c ]". */
static string RunGlob(const char *root, bool is_abs, const char *pattern) {
  char temp[PATH_MAX];
  if (is_abs) {
    sprintf(temp, "%s/%s", root, pattern);
  } else {
    strcpy(temp, pattern);
  }
  size_t skip = strlen(root) + 1;
  set<string> matches;
  Glob(
      temp,
      [skip, &matches](const char *name) {
        matches.insert(name + skip);
        return true;
      }
  );
  bool sep = false;
  ostringstream strm;
  strm << '[';
  for (const string &match: matches) {
    if (sep) {
      strm << ", ";
    } else {
      strm << ' ';
      sep = true;
    }
    strm << match;
  }
  strm << (sep ? " ]" : "]");
  return strm.str();
}

FIXTURE(Typical) {
  /* To test Glob(), we'll create a directory tree in /tmp and search through it with various patterns.
     The test tree looks like this:

        logs/
          info.dat
          info.txt
          readme.txt
          2013.03.12/
            1001.log
            1002.log
            1003.log
          2013.03.13/
            1001.log
            1002.log
            1003.log
          2013.03.14/
            1001.log
            1002.log
            1003.log
        tables/
          info.dat
          info.txt
          readme.txt
          is_friend_of.tbl
          person.tbl
          system.tbl

     We'll remove the whole tree before we leave so /tmp won't get full of test junk. */
  char root[PATH_MAX];
  strcpy(root, "/tmp/fs_glob_XXXXXX");
  mkdtemp(root);
  try {
    /* Create the sub-directories and files we'll test against. */
    char path[PATH_MAX];
    CreateTopDir(path, root, "logs");
    CreateLogDir(path, 12);
    CreateLogDir(path, 13);
    CreateLogDir(path, 14);
    CreateTopDir(path, root, "tables");
    CreateFile(path, "is_friend_of.tbl");
    CreateFile(path, "person.tbl");
    CreateFile(path, "system.tbl");
    /* Run all the tests twice: once as relative patterns and once as absolute patterns. */
    for (bool is_abs: { false, true }) {
      /* Change the directory to be at the root for relative pattern tests, but somewhere else for absolute pattern tests. */
      if (!is_abs) {
        IfLt0(chdir(root));
      }
      else {
        IfLt0(chdir("/home/"));
      }
      EXPECT_EQ(RunGlob(root, is_abs, "logs/info.*"), "[ logs/info.dat, logs/info.txt ]");
      EXPECT_EQ(RunGlob(root, is_abs, "tables/info.*"), "[ tables/info.dat, tables/info.txt ]");
      EXPECT_EQ(RunGlob(root, is_abs, "*/info.*"), "[ logs/info.dat, logs/info.txt, tables/info.dat, tables/info.txt ]");
      EXPECT_EQ(RunGlob(root, is_abs, "logs/*.tbl"), "[]");
      EXPECT_EQ(RunGlob(root, is_abs, "tables/*.tbl"), "[ tables/is_friend_of.tbl, tables/person.tbl, tables/system.tbl ]");
      EXPECT_EQ(RunGlob(root, is_abs, "*/*.tbl"), "[ tables/is_friend_of.tbl, tables/person.tbl, tables/system.tbl ]");
      EXPECT_EQ(
          RunGlob(root, is_abs, "logs/2013.03.*/*.log"),
          "[ logs/2013.03.12/1001.log, logs/2013.03.12/1002.log, logs/2013.03.12/1003.log"
          ", logs/2013.03.13/1001.log, logs/2013.03.13/1002.log, logs/2013.03.13/1003.log"
          ", logs/2013.03.14/1001.log, logs/2013.03.14/1002.log, logs/2013.03.14/1003.log ]"
      );
      EXPECT_EQ(
          RunGlob(root, is_abs, "logs/2013.03.12/*.log"),
          "[ logs/2013.03.12/1001.log, logs/2013.03.12/1002.log, logs/2013.03.12/1003.log ]"
      );
      EXPECT_EQ(
          RunGlob(root, is_abs, "logs/2013.03.*/1002.log"),
          "[ logs/2013.03.12/1002.log, logs/2013.03.13/1002.log, logs/2013.03.14/1002.log ]"
      );
      EXPECT_EQ(RunGlob(root, is_abs, "logs/2013.03.13/1002.log"), "[ logs/2013.03.13/1002.log ]");
    }
  } catch (...) {
    RemoveAll(root);
    throw;
  }
  RemoveAll(root);
}
