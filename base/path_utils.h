/* <base/path_utils.h>

   Utilities for working with file system paths.

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

#include <initializer_list>
#include <string>

#include <base/string_utils.h>

#include <sys/stat.h>

namespace Base {

  /* Deletes the given file (unlink it from the filesystem) */
  void Delete(const char *path);

  /* Ensure that the given directory exists.  If any part of it doesn't exist,
     create it with the given protection mask.  Parts that already exist we
     just leave alone. */
  void EnsureDirExists(const char *path, mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  /* As above, but with the option to skip the last (right-most) part of the path.
     Useful for when the path includes the file name but you just want to create the
     directory portion. */
  void EnsureDirExists(const char *path, bool skip_last, mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  /* Removes the directory entirely, from leaf to root, disposing of all files
     along the way.  This is like "rm -rf". */
  void EnsureDirIsGone(const char *path);

  /* True iff. the given file or directory exists in the file system. */
  bool ExistsPath(const char *path);

  /* Make a path string from the given directory names and file name parts.
     Any null c-string pointers or empty strings are skipped.
     The directory names may start or end in one or more slashes, but they will be ignored.
     Slashes are inserted automatically as necessary.  The file name parts are concatenated as-is.
     If you need dots for extensions, make sure you pass them.
     If 'is_absolute' is true, the path will start with a slash; otherwise, it will not.
     This function doesn't check the individual strings to make sure they are valid in a path. */
  std::string MakePath(bool is_absolute, std::initializer_list<const char *> dirs, std::initializer_list<const char *> parts);

  /* Like above, but is absolute iff. the first entry in 'dirs' is absolute.
     If 'dirs' is empty, the path is relative. */
  std::string MakePath(std::initializer_list<const char *> dirs, std::initializer_list<const char *> parts);

}  // Base
