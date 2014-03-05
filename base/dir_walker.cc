/* <base/dir_walker.cc>

   Implements <base/dir_walker.h>.

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

#include <base/dir_walker.h>

#include <cassert>
#include <cstring>

#include <sys/stat.h>

#include <base/error_utils.h>
#include <base/no_default_case.h>

using namespace std;
using namespace Base;

TDirWalker::~TDirWalker() {}

bool TDirWalker::Walk(const char *root) {
  assert(this);
  bool result = true;  // Captures the result of the walker's event handlers.  If it ever becomes false, we abort the walk.
  TEntry
      entry,        // Filled in and passed for all event handlers.
      cycle_entry;  // Filled in and passed only for OnDirCycle().
  /* The OS walk function allows multiple roots, but we really just want one. */
  char *roots[2];
  roots[0] = const_cast<char *>(root);
  roots[1] = 0;
  /* Start walking, sorting at equal depth entries by name. */
  auto tree = fts_open(
      roots, FTS_COMFOLLOW | FTS_PHYSICAL | FTS_NOCHDIR,
      [](const FTSENT **lhs, const FTSENT **rhs) {
        return strcmp((*lhs)->fts_name, (*rhs)->fts_name);
      }
  );
  try {
    /* Loop until we're done walking or until an event handler tells us to abort. */
    do {
      /* Read the next entry.  If there isn't one, we're done. */
      auto node = fts_read(tree);
      if (!node) {
        break;
      }
      /* What kind of entry is this? */
      switch (node->fts_info) {
        /* A directory being entered. */
        case FTS_D: {
          InitEntry(entry, node);
          switch (OnDirBegin(entry)) {
            case Enter: {
              break;
            }
            case Skip: {
              IfLt0(fts_set(tree, node, FTS_SKIP));
              break;
            }
            case Abort: {
              result = false;
              break;
            }
          }
          break;
        }
        /* A directory that forms a cycle. */
        case FTS_DC: {
          InitEntry(entry, node);
          InitEntry(cycle_entry, node->fts_cycle);
          result = OnDirCycle(entry, cycle_entry);
          break;
        }
        /* Something not otherwise specified. */
        case FTS_DEFAULT: {
          /* The mode bits in stat can tell us what this entry really is. */
          switch(node->fts_statp->st_mode & S_IFMT) {
            /* It's a block device. */
            case S_IFBLK: {
              InitEntry(entry, node);
              result = OnBlockDev(entry);
              break;
            }
            /* It's a character device. */
            case S_IFCHR: {
              InitEntry(entry, node);
              result = OnCharDev(entry);
              break;
            }
            /* It's a named pipe. */
            case S_IFIFO: {
              InitEntry(entry, node);
              result = OnNamedPipe(entry);
              break;
            }
            /* It's a socket. */
            case S_IFSOCK: {
              InitEntry(entry, node);
              result = OnSocket(entry);
              break;
            }
            /* These cases should have been reported as something other than FTS_DEFAULT,
               so we're in serious trouble here. */
            case S_IFDIR:
            case S_IFLNK:
            case S_IFREG: {
              TError::Abort(HERE);
              break;
            }
            /* We have no clue what this entry is, but report it anyway. */
            default: {
              InitEntry(entry, node);
              result = OnUnknown(entry);
            }
          }
          break;
        }
        /* A directory we couldn't cannot be read. */
        case FTS_DNR: {
          ThrowSystemError(node->fts_errno);
        }
        /* A "." or ".." directory. */
        case FTS_DOT: {
          break;
        }
        /* A directory being exited. */
        case FTS_DP: {
          InitEntry(entry, node);
          result = OnDirEnd(entry);
          break;
        }
        /* An non-entry containing just an error code. */
        case FTS_ERR: {
          ThrowSystemError(node->fts_errno);
        }
        /* Just a file. */
        case FTS_F: {
          InitEntry(entry, node);
          result = OnFile(entry);
          break;
        }
        /* A file we couldn't stat, which is an error. */
        case FTS_NS: {
          ThrowSystemError(node->fts_errno);
        }
        /* A symlink. */
        case FTS_SL: {
          InitEntry(entry, node);
          switch (OnSymLink(entry)) {
            case Enter: {
              IfLt0(fts_set(tree, node, FTS_FOLLOW));
              break;
            }
            case Skip: {
              break;
            }
            case Abort: {
              result = false;
              break;
            }
          }
          break;
        }
        NO_DEFAULT_CASE;
      }  // switch
    } while (result);
  } catch (...) {
    fts_close(tree);
    throw;
  }
  fts_close(tree);
  return result;
}

bool TDirWalker::OnBlockDev(const TEntry &/*entry*/) {
  return true;
}

bool TDirWalker::OnCharDev(const TEntry &/*entry*/) {
  return true;
}

TDirWalker::TAction TDirWalker::OnDirBegin(const TEntry &/*entry*/) {
  return Enter;
}

bool TDirWalker::OnDirCycle(const TEntry &/*entry*/, const TEntry &/*cycle_entry*/) {
  return true;
}

bool TDirWalker::OnDirEnd(const TEntry &/*entry*/) {
  return true;
}

bool TDirWalker::OnFile(const TEntry &/*entry*/) {
  return true;
}

bool TDirWalker::OnNamedPipe(const TEntry &/*entry*/) {
  return true;
}

bool TDirWalker::OnSocket(const TEntry &/*entry*/) {
  return true;
}

bool TDirWalker::OnUnknown(const TEntry &/*entry*/) {
  return true;
}

TDirWalker::TAction TDirWalker::OnSymLink(const TEntry &/*entry*/) {
  return Skip;
}

void TDirWalker::InitEntry(TEntry &out, const FTSENT *in) {
  assert(&out);
  assert(in);
  auto *st = in->fts_statp;
  assert(st);
  out.AccessPath = in->fts_accpath;
  out.RootPath = in->fts_path;
  out.Name = in->fts_name;
  out.Depth = in->fts_level;
  out.Mode = st->st_mode;
  out.OwnerId = st->st_uid;
  out.GroupId = st->st_gid;
  out.Size = st->st_size;
  out.TimeAccessed = st->st_atime;
  out.TimeModified = st->st_mtime;
  out.GetTimeStatusChanged = st->st_ctime;
}
