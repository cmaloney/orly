/* <base/path_utils.cc>

   Implements <base/path_utils.h>.

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

#include <cassert>
#include <cstring>
#include <sstream>

#include <unistd.h>

#include <base/error_utils.h>
#include <base/dir_walker.h>

using namespace std;
using namespace Base;

void Base::EnsureDirExists(const char *path, mode_t mode) {
  EnsureDirExists(path, false, mode);
}

void Base::EnsureDirExists(const char *path, bool skip_last, mode_t mode) {
  assert(path);
  /* Make a copy of the path so we can drop c-string terminators into it where we like.
     Point at the start of the copy and to its first non-slash character, which might be
     the same. */
  string copyof_path(path);
  char
      *start = const_cast<char *>(copyof_path.c_str()),
      *limit = (*start == '/') ? (start + 1) : start;
  /* Walk along the copy of the path, finding all the sub-paths.  For example, from
     "/usr/include/c++", we'll find "/usr", "/usr/include", and "/usr/include/c++",
     in that order. */
  for (;; ++limit) {
    /* If we're at c-string terminator or a slash, we're at the end of a sub-path.
       If it's a c-string terminator and we're skipping the last part, we're done. */
    char c = *limit;
    if (!c && skip_last) {
      break;
    }
    if (!c || c == '/') {
      /* Temporarily make this the end of a c-string, if it wasn't already. */
      *limit = '\0';
      /* Try to create the path up to this point.  If it already exists, that's fine. */
      if (mkdir(start, mode) < 0 && errno != EEXIST) {
        ThrowSystemError(errno);
      }
      /* If this was the original end of the path, we're done. */
      if (!c) {
        break;
      }
      /* Restore the slash that we overwrote with the c-string terminator. */
      *limit = '/';
    }
  }  // for
}

void Base::EnsureDirIsGone(const char *path) {
  assert(path);
  class dir_walker_t final : public TDirWalker {
    virtual bool OnBlockDev(const TEntry &entry) override {
      Unlink(entry);
      return true;
    }
    virtual bool OnCharDev(const TEntry &entry) override {
      Unlink(entry);
      return true;
    }
    virtual TAction OnDirBegin(const TEntry &) override {
      return Enter;
    }
    virtual bool OnDirCycle(const TEntry &entry, const TEntry &) override {
      RmDir(entry);
      return true;
    }
    virtual bool OnDirEnd(const TEntry &entry) override {
      RmDir(entry);
      return true;
    }
    virtual bool OnFile(const TEntry &entry) override {
      Unlink(entry);
      return true;
    }
    virtual bool OnNamedPipe(const TEntry &entry) override {
      Unlink(entry);
      return true;
    }
    virtual bool OnSocket(const TEntry &entry) override {
      Unlink(entry);
      return true;
    }
    virtual TAction OnSymLink(const TEntry &entry) override {
      Unlink(entry);
      return Skip;
    }
    virtual bool OnUnknown(const TEntry &entry) override {
      Unlink(entry);
      return true;
    }
    void RmDir(const TEntry &entry) {
      IfLt0(rmdir(entry.AccessPath));
    }
    void Unlink(const TEntry &entry) {
      IfLt0(unlink(entry.AccessPath));
    }
  };
  dir_walker_t().Walk(path);
}

bool Base::ExistsPath(const char *path) {
  assert(path);
  struct stat st;
  bool result;
  if (stat(path, &st) < 0) {
    if (errno != ENOENT) {
      ThrowSystemError(errno);
    }
    result = false;
  } else {
    result = true;
  }
  return result;
}

string Base::MakePath(bool is_absolute, initializer_list<const char *> dirs, initializer_list<const char *> parts) {
  ostringstream strm;
  for (const char *start: dirs) {
    if (start) {
      const char *limit = start + strlen(start) - 1;
      while (start <= limit && *start == '/') {
        ++start;
      }
      while (start <= limit && *limit == '/') {
        --limit;
      }
      if (start <= limit) {
        if (is_absolute) {
          strm << '/';
        } else {
          is_absolute = true;
        }
        strm.write(start, limit - start + 1);
      }
    }
  }
  if (is_absolute) {
    strm << '/';
  }
  for (const char *c_str: parts) {
    if (c_str) {
      strm << c_str;
    }
  }
  return strm.str();
}

string Base::MakePath(initializer_list<const char *> dirs, initializer_list<const char *> parts) {
  bool is_absolute = false;
  if (dirs.begin() != dirs.end()) {
    const char *dir = *dirs.begin();
    is_absolute = (dir && *dir == '/');
  }
  return MakePath(is_absolute, dirs, parts);
}
