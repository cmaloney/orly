/* <base/tmp_file.h>

   Generates a temporary file.

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

#include <cassert>

#include <base/class_traits.h>
#include <base/fd.h>

namespace Base {

  class TTmpFile final {
    NO_COPY(TTmpFile);
    public:

    TTmpFile(const char *name_template = "/tmp/orly_general_XXXXXX.tmp",
             bool delete_on_destroy = false);

    ~TTmpFile();

    const char *GetName() const {
      return Name.data();
    }

    const TFd &GetFd() const {
      return Fd;
    }

    /* Allows taking ownership of the fd so it has a independent lifetime
       from the deletion of the file. */
    TFd MoveFd() {
      return TFd(std::move(Fd));
    }

    void SetDeleteOnDestroy(bool delete_on_destroy) {
      DeleteOnDestroy = delete_on_destroy;
    }

    private:

    /* Name of the temporary file. This is a std::vector<char> instead of a std::string, because the data gets passed to
       mkstemps() call which modifies the data we pass it. std::string's data() function returns a non-const pointer, which if we
       const_cast<> it away we land in undefined behavior. */
    std::vector<char> Name;

    /* Whether we unlink the file on destruction of the object or not. */
    bool DeleteOnDestroy;

    /* Fd associated to the file. */
    TFd Fd;
  };  // TTmpFile

}  // Base