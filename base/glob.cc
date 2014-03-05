/* <base/glob.cc>

   Implements <base/glob.h>.

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

#include <cassert>
#include <cstring>
#include <unordered_map>
#include <string>
#include <syslog.h>

#include <fnmatch.h>
#include <fts.h>
#include <unistd.h>
#include <linux/limits.h>

#include <base/error_utils.h>

using namespace std;
using namespace Base;

bool Base::Glob(const char *pattern, const function<bool (const char *)> &cb) {
  assert(pattern);
  assert(&cb);
  /* Get the current working directory as an absolute path.
     This will start with a slash but will NOT end with a slash. */
  char root[PATH_MAX + 2];
  if (!getcwd(root, PATH_MAX)) {
    ThrowSystemError(errno);
  }
  assert(*root = '/');
  char *root_end = root + strlen(root);
  assert(root_end > root);
  assert(root_end[-1] != '/');
  bool pattern_is_relative = (*pattern != '/');
  if (pattern_is_relative) {
    /* The pattern is relative.  If it starts with './' or '../', adjust the pattern and the working directory accordingly.
       The result will be an absolute root from which to start searching (which will NOT end with '/') and
       a pattern which starts with '/' or with a file name. */
    for (;;) {
      if (strncmp(pattern, "./", 2) == 0) {
        pattern += 2;
        continue;
      }
      if (strncmp(pattern, "../", 3) == 0) {
        for (;;) {
          --root_end;
          if (root_end <= root) {
            THROW_ERROR(TBadGlobPattern) << "pattern contains too many \"../\" operations for current directory";
          }
          if (*root_end == '/') {
            *root_end = '\0';
            break;
          }
        }
        pattern += 3;
        continue;
      }
      break;
    }
    if (strlen(pattern) == 0) {
      THROW_ERROR(TBadGlobPattern) << "pattern contains only \"./\" and/or \"../\"";
    }
    /* Add a trailing '/' to the root so we can use it as a prefix. */
    strcat(root, "/");
  }
  else {
    /* Change root so that it is the constant part of pattern (which is an absolute path). */
    memset(root, '\0', PATH_MAX + 2);
    const char *p = pattern + 1;
    /* Move p until we find a wilcard. */
    for (; *p != '*' && *p != '\0'; ++p) {}
    /* Move p back to most recent '/'. */
    for (; *p != '/'; --p) {}
    /* Copy the absolute path that begins the pattern in to root. */
    memcpy(root, pattern, p - pattern + 1);
  }
  size_t root_len = strlen(root);
  /* Count the directory levels in the pattern. */
  int pattern_level = 0;
  for (const char *csr = pattern; *csr; ++csr) {
    if (*csr == '/') {
      ++pattern_level;
    }
  }
  std::unordered_map<int, std::string> truncated_patterns;
  int root_level = 0;
  for (const char *csr = root; *csr; ++csr) {
    if (*csr == '/') {
      ++root_level;
    }
  }
  /* Open a scan of the file tree, starting from the root. */
  char *root_array[2];
  root_array[0] = root;
  root_array[1] = 0;
  auto tree = fts_open(root_array, FTS_PHYSICAL | FTS_NOCHDIR, 0);
  if (!tree) {
    ThrowSystemError(errno);
  }
  bool result = true;
  try {
    /* Loop until we're out of files or until the callback tells us to stop. */
    do {
      /* Get the next node in the tree. */
      auto node = fts_read(tree);
      if (!node) {
        if (errno) {
          ThrowSystemError(errno);
        }
        /* There is no next node.  We're done. */
        break;
      }
      /* Get the directory level of the current node. */
      int file_level = node->fts_level - 1;
      if (!pattern_is_relative) {
        file_level += root_level;
      }
      if (file_level == pattern_level) {
        /* We have reached the directory level we're looking for. */
        if (node->fts_info & FTS_F) {
          /* The node is a file.  Check it against the pattern. */
          const char *name = pattern_is_relative ? (node->fts_path + root_len) : node->fts_path;
          switch (fnmatch(pattern, name, 0)) {
            case 0: {
              /* The file matches the pattern, so call back. */
              // syslog(LOG_INFO, "Glob cb for: %s", node->fts_path);
              result = cb(node->fts_path);
              break;
            }
            case FNM_NOMATCH: {
              /* The file does not match the pattern, so ignore it. */
              break;
            }
            default: {
              /* Badness occurred. */
              ThrowSystemError(errno);
            }
          }
        } else if (node->fts_info & FTS_D) {
          /* The node is a directory.  We're already as deep as we want to go, so skip it. */
          fts_set(tree, node, FTS_SKIP);
        }
      }
      else {
        /* We are not yet deep enough in the tree to match anything, but we want to skip directories which won't match. */
        if (node->fts_info & FTS_D) {
        /* In order to do this, if the node is a directory, we create a pattern up to the level the node is at and
           then check to see if the node matches the pattern.  If it doesn't, we skip it so that we don't search through
           its children which we know will also not match the pattern. */
          if (truncated_patterns.find(file_level) == truncated_patterns.end()) {
            size_t pos = 0;
            std::stringstream truncated_pattern;
            for(int truncated_pattern_level = 0; pattern[pos] != '\0' && file_level > 0; ++pos) {
              if (pattern[pos] == '/') {
                ++truncated_pattern_level;
                if(truncated_pattern_level > file_level) {
                  /* Don't add the last '/' to the truncated pattern. */
                  break;
                }
              }
              truncated_pattern << pattern[pos];
            }
            /* Store the pattern for this file_level so that we can re-use it. */
            truncated_patterns[file_level] = truncated_pattern.str();
          }
          const char *name = (pattern_is_relative ? (node->fts_path + root_len) : node->fts_path);
          if(truncated_patterns[file_level] != "\0" /* When node is root and path is relative, truncated_pattern will be empty. */
            && fnmatch(truncated_patterns[file_level].c_str(), name, FNM_PATHNAME) == FNM_NOMATCH
            && strcmp(name, root) != 0) { /* We need to check this because when name is root it still ends in a '/' and doesn't match. */
            fts_set(tree, node, FTS_SKIP);
          }
        }
      }
    }  while (result);
  } catch (...) {
    fts_close(tree);
    throw;
  }
  fts_close(tree);
  return result;
}
