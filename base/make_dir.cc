/* <base/make_dir.cc>

   Implements <base/make_dir.h>

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

#include <base/make_dir.h>
#include <base/thrower.h>

#include <cstdlib>
#include <cstring>

#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void Base::MakeDirs(const char *path) {
  assert(path);
  char buf[PATH_MAX];
  strcpy(buf, path);
  char
    *start = buf,
    *limit = buf + strlen(buf);
  while (start < limit) {
    char *delim = strchr(start, '/');
    if (!delim) {
      break;
    }
    size_t size = delim - buf;
    if (size) {
      *delim = '\0';
      if (mkdir(buf, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0 && errno != EEXIST) {
        THROW << "cannot make directory \"" << buf << '"';
      }
      *delim = '/';
    }
    start = delim + 1;
  }
}
