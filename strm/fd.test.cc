/* <strm/fd.test.cc>

   Unit test for <strm/fd.h>

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

#include <strm/fd.h>

#include <sys/types.h>
#include <fcntl.h>

#include <cstring>

#include <base/path_utils.h>
#include <strm/bin/in.h>
#include <strm/bin/out.h>

#include <test/kit.h>

using namespace Strm;

const char *filename = "/tmp/stig_strm_fd.test_tmp_file";

FIXTURE(Unidirectional) {
  constexpr auto filename_len = 32;
  assert(filename_len == strlen(filename) + 1);

  /* Write to a file */ {
    TFdDefault fd(Base::TFd(creat(filename, 0666)));
    Bin::TOut out(&fd);
    out.Write(filename, filename_len);
  }

  char Buffer[filename_len];
  /* Read from a file */ {
    TFdDefault fd(Base::TFd(open(filename, O_RDONLY)));
    Bin::TIn in(&fd);
    in.Read(Buffer, filename_len);
  }
  // We should have gotten the same thing out we put in.
  // NOTE: The cast to string here is incredibly stupid... But it causes operator== to be happy, seems better than
  // strcmp()...
  EXPECT_EQ(std::string(Buffer), filename);

  // It's nice to cleanup
  Base::Delete(filename);
}