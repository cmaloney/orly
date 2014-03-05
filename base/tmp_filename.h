/* <base/tmp_filename.h>

   Generates a temporary filename.

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

#include <cstdlib>
#include <cstring>

#include <base/assert_true.h>
#include <base/os_error.h>

/* FIXME: This leaks file descriptors!!! */
namespace Base {
  class TTmpFilename {
    public:
    /* Construct a new temp filename. See man 3 mkstemp. */
    TTmpFilename(const char *filename_template="/tmp/stig_general_tmpXXXXXX")
          : Filename(Base::TOsError::IfNull(HERE, strdup(Base::AssertTrue(filename_template))))  {

      if(mkstemp(Filename) == -1) {
        free(Filename);
        throw Base::TOsError(HERE);
      }
    }

    ~TTmpFilename() {
      free(Filename);
    }

    const char *Get() {
      assert(this);
      return Filename;
    }

    private:
    char *Filename;
  };
}