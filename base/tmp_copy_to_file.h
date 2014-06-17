/* <base/tmp_copy_to_file.h>

   Copies a string to a temporary file, which it unlinks when it is destroyed.

   See <util/path.test.cc> for tests of this unit.

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

#pragma once

#include <cassert>
#include <mutex>
#include <random>
#include <string>

#include <base/class_traits.h>

namespace Base {

  /* Copies a string to a temporary file, which it unlinks when it is destroyed. */
  class TTmpCopyToFile final {
    NO_COPY(TTmpCopyToFile);
    public:

    /* Copies the text to a temp file in the given directory. */
    TTmpCopyToFile(
        const std::string &tmp_dir, const std::string &text,
        const std::string &prefix = "",
        const std::string &ext = "");

    /* Ensures the tmp file is gone. */
    ~TTmpCopyToFile();

    /* The path to tmp file we manage. */
    const std::string &GetPath() const {
      assert(this);
      return Path;
    }

    private:

    /* See accessor. */
    std::string Path;

    /* Covers 'Rand', below. */
    static std::mutex Mutex;

    /* Used to generate unique file names. */
    static std::minstd_rand Rand;

  };  // TTmpCopyToFile

}  // Base