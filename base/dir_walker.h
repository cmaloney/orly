/* <base/dir_walker.h>

   Walks directories in the file system.

   See <base/path_utils.test.cc> for tests of this unit.

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

#pragma once

#include <cstdint>

#include <fts.h>

namespace Base {

  /* Walks a dir in the file system. */
  class TDirWalker {
    public:

    /* Do-little. */
    virtual ~TDirWalker();

    /* Walk the given dir, sending events to the given walker.  If the root is a symlink,
       we'll follow it immediately, but any other symlinks we encounter we will call back for.
       We do not call back for "." or ".." entries, except when that's our starting point. */
    bool Walk(const char *root);

    protected:

    /* An entry in the file system. */
    struct TEntry {

      /* The path to this entry from the current working directory.  Never null. */
      const char *AccessPath;

      /* Our current depth in the walk.  The entry given as the root is at depth 0, its
         immediate children are at depth 1, etc. */
      uint16_t Depth;

      /* The id of the group to which this entry belongs. */
      gid_t GroupId;

      /* The protection mask of the entry. */
      mode_t Mode;

      /* The name of the entry within its directory.  Never null. */
      const char *Name;

      /* The id of the user to which this entry belongs. */
      uid_t OwnerId;

      /* The path to the entry from the root of the dire being walked.  Never null. */
      const char *RootPath;

      /* The size of the entry, in bytes. */
      off_t Size;

      /* The time the entry was last accessed. */
      time_t TimeAccessed;

      /* The time the entry was last modified. */
      time_t TimeModified;

      /* The time the entry last had its status changed. */
      time_t GetTimeStatusChanged;

    };  // TDirWalker::Entry

    /* The action to taken upon return from OnDirBegin() or OnSymLink(), q.q.v. */
    enum TAction {

      /* Continue the walk by descending into the directory or following the symlink. */
      Enter,

      /* Continue the walk by advancing to the next entry in the current directory;
         that is, don't descend into the directory or follow the symlink. */
      Skip,

      /* Abort the walk altogether. */
      Abort

    };  // TDirWalker::TDirAction

    /* Do-little. */
    TDirWalker() {}

    /* Called when the walk encounters a block device.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnBlockDev(const TEntry &entry);

    /* Called when the walk encounters a character device.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnCharDev(const TEntry &entry);

    /* Called when the walk is about to descend into a new directory.
       See TDirAction for more information.  The default returns Enter. */
    virtual TAction OnDirBegin(const TEntry &entry);

    /* Called when the walk refuses to descend into a new directory because of a cycle in the dir.
       The cycle might be caused by a symlink or a hard link.
       The second argument describes the other end of the cycle.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnDirCycle(const TEntry &entry, const TEntry &cycle_entry);

    /* Called when the walk is about to ascend out of a directory.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnDirEnd(const TEntry &entry);

    /* Called when the walk encounters a normal file.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnFile(const TEntry &entry);

    /* Called when the walk encounters a named pipe.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnNamedPipe(const TEntry &entry);

    /* Called when the walk encounters a socket.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnSocket(const TEntry &entry);

    /* Called when the walk encounters a symlink.
       See TLinkAction for more information.  The default returns Skip. */
    virtual TAction OnSymLink(const TEntry &entry);

    /* Called when the walk encounters an entry of a kind it doesn't recognize.
       On a file system which doesn't support kinds of files, all entries will be reported this way.
       Return true to continue the walk, false to abort.  The default returns true. */
    virtual bool OnUnknown(const TEntry &entry);

    private:

    /* Initialize an entry structure from an OS cursor. */
    static void InitEntry(TEntry &out, const FTSENT *in);

  };  // TDirWalker

}  // Base
