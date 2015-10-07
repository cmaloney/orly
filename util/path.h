/* <util/path.h>

   Utilities for working with file system paths.

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

#pragma once

#include <string>

#include <sys/stat.h>

namespace Util {

/* Deletes the given file (unlink it from the filesystem) */
void Delete(const char *path);

// TODO(cmaloney): bit modernize these, reduce unnecessary overloads, options.
/* Ensure that the given directory exists.  If any part of it doesn't exist,
   create it with the given protection mask.  Parts that already exist we
   just leave alone. */
void EnsureDirExists(const char *path, mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

/* As above, but with the option to skip the last (right-most) part of the path.
   Useful for when the pathz includes the file name but you just want to create the
   directory portion. */
void EnsureDirExists(const char *path,
                     bool skip_last,
                     mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

/* Removes the directory entirely, from leaf to root, disposing of all files
   along the way.  This is like "rm -rf". */
void EnsureDirIsGone(const char *path);

/* True iff. the given file or directory exists in the file system. */
bool ExistsPath(const char *path);

/* Gets the current working directory as a std::string. The cwd never ends in '/' */
std::string GetCwd();

/* Normalize the path by removing '/../', '/./', '//'.
   Does not consult the filesystem, so may change the meaning of the path. */
std::string Normalize(const std::string path);

}  // Util

