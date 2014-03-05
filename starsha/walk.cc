/* <starsha/walk.cc>

   Implements <starsha/walk.h>.

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

#include <starsha/walk.h>

#include <cassert>
#include <cstring>

#include <fts.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

void Starsha::Walk(
    const char *root,
    const function<bool (const char *)> &dir_cb,
    const function<void (const char *, time_t)> &file_cb) {
  assert(root);
  assert(&dir_cb);
  assert(&file_cb);
  size_t root_len = strlen(root) + 1;
  char *roots[2];
  roots[0] = const_cast<char *>(root);
  roots[1] = 0;
  auto tree = fts_open(roots, FTS_PHYSICAL | FTS_NOCHDIR, 0);
  try {
    for (;;) {
      auto node = fts_read(tree);
      if (!node) {
        break;
      }
      if (node->fts_info & FTS_D) {
        if (node->fts_pathlen > root_len && !dir_cb(node->fts_path + root_len)) {
          fts_set(tree, node, FTS_SKIP);
        }
      } else if (node->fts_info & FTS_F) {
        file_cb(node->fts_path + root_len, node->fts_statp->st_mtime);
      }
    }
  } catch (...) {
    fts_close(tree);
    throw;
  }
  fts_close(tree);
}
