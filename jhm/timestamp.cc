/* <jhm/timestamp.cc>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <jhm/timestamp.h>

#include <base/split.h>

using namespace Base;
using namespace std;
using namespace Util;

timespec Jhm::GetTimestampSearchingPath(const string &name) {
  char *path = getenv("PATH");

  vector<string> potential;
  Split(":", path, potential);

  struct stat st;

  for (string &prefix : potential) {
    if (!prefix.empty() && prefix.back() != '/') {
      prefix += '/';
    }
    if (stat((prefix + name).c_str(), &st) != 0) {
      if (errno == ENOENT) {
        continue;
      } else {
        ThrowSystemError(errno);
      }
    } else {
      return st.st_mtim;
    }
  }
  ThrowSystemError(ENOENT);
}

