/* <base/tmp_copy_to_file.cc>

   Implements <base/tmp_copy_to_file.h>.

   See <base/path_utils.test.cc> for tests of this unit.

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

#include <base/tmp_copy_to_file.h>

#include <fcntl.h>

#include <base/fd.h>
#include <base/io_utils.h>
#include <base/path_utils.h>

using namespace std;
using namespace Base;

TTmpCopyToFile::TTmpCopyToFile(
    const string &tmp_dir, const string &text,
    const string &prefix, const string &ext) {
  auto id = to_string(Rand());
  Path = MakePath({ tmp_dir.c_str() }, { prefix.c_str(), id.c_str(), ext.c_str() });
  TFd fd(creat(Path.c_str(), S_IRWXU));
  WriteExactly(fd, text.data(), text.size());
}

TTmpCopyToFile::~TTmpCopyToFile() {
  assert(this);
  Delete(Path.c_str());
}

mutex TTmpCopyToFile::Mutex;

minstd_rand TTmpCopyToFile::Rand((random_device())());
