/* <util/path.cc>

   Implements <util/path.h>.

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

#include <util/path.h>

#include <cassert>
#include <cstring>
#include <memory>
#include <vector>

#include <unistd.h>

#include <base/as_str.h>
#include <base/dir_walker.h>
#include <base/split.h>
#include <util/error.h>

using namespace Base;
using namespace std;
using namespace Util;

void Util::Delete(const char *path) { IfLt0(unlink(path)); }

void Util::EnsureDirExists(const char *path, mode_t mode) { EnsureDirExists(path, false, mode); }

void Util::EnsureDirExists(const char *path, bool skip_last, mode_t mode) {
  assert(path);
  /* Make a copy of the path so we can drop c-string terminators into it where we like.
     Point at the start of the copy and to its first non-slash character, which might be
     the same. */
  string copyof_path(path);
  char *start = const_cast<char *>(copyof_path.c_str()),
       *limit = (*start == '/') ? (start + 1) : start;
  /* Walk along the copy of the path, finding all the sub-paths.  For example, from
     "/usr/include/c++", we'll find "/usr", "/usr/include", and "/usr/include/c++",
     in that order. */
  for(;; ++limit) {
    /* If we're at c-string terminator or a slash, we're at the end of a sub-path.
       If it's a c-string terminator and we're skipping the last part, we're done. */
    char c = *limit;
    if(!c && skip_last) {
      break;
    }
    if(!c || c == '/') {
      /* Temporarily make this the end of a c-string, if it wasn't already. */
      *limit = '\0';
      /* Try to create the path up to this point.  If it already exists, that's fine. */
      if(mkdir(start, mode) < 0 && errno != EEXIST) {
        ThrowSystemError(errno);
      }
      /* If this was the original end of the path, we're done. */
      if(!c) {
        break;
      }
      /* Restore the slash that we overwrote with the c-string terminator. */
      *limit = '/';
    }
  }  // for
}

void Util::EnsureDirIsGone(const char *path) {
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
    virtual TAction OnDirBegin(const TEntry &) override { return Enter; }
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
    void RmDir(const TEntry &entry) { IfLt0(rmdir(entry.AccessPath)); }
    void Unlink(const TEntry &entry) { IfLt0(unlink(entry.AccessPath)); }
  };
  dir_walker_t().Walk(path);
}

bool Util::ExistsPath(const char *path) {
  assert(path);
  struct stat st;
  bool result;
  if(stat(path, &st) < 0) {
    if(errno != ENOENT) {
      ThrowSystemError(errno);
    }
    result = false;
  } else {
    result = true;
  }
  return result;
}

string Util::GetCwd() {
  unique_ptr<char, void (*)(void *)> cwd(getcwd(nullptr, 0), &free);
  return string(cwd.get());
}

std::string Util::Normalize(std::string path) {
  // TODO(cmaloney): Replace with a more efficient implementation.

  // Replacements
  // '//' -> '/'
  // 'foo/bar/../' -> 'foo'
  // '/./' -> '/'

  // Split path by '/'. Scan from front to back, finding 'meaningful' patterns
  // and interpreting their changes.
  vector<string> pieces;
  Split("/", path, pieces);

  auto iter = pieces.begin();
  while(iter != pieces.end()) {
    // Handle './', '/./'
    if(*iter == ".") {
      iter = pieces.erase(iter);
      continue;
    }

    // Handle '//'. Allow leading and trailing to stay.
    if(iter != pieces.begin() && iter != pieces.end() - 1 && iter->empty()) {
      iter = pieces.erase(iter);
      continue;
    }

    // Handle '/../'
    if(*iter == "..") {
      if (iter == pieces.begin()) {
        // Parent of top directory is top directory.
        iter = pieces.erase(iter);
        continue;
      } else {
        // Remove parent directory and the traversal to it.
        iter = pieces.erase(iter-1, iter+1);
        continue;
      }
    }

    ++iter;
  }

  // TODO(cmaloney): Verify that at beginning of string only
  // `./////foo` -> `foo`
  // `./foo` -> `foo`

  // Join back together into result string.
  return AsStr(Join(pieces, "/"));
}
