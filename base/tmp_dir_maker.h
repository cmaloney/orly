/* <base/tmp_dir_maker.h>

   Creates a directory path on construction and destroys it on destruction.

   See <base/path_utils.test.cc> for tests of this unit.

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

#pragma once

#include <cassert>
#include <string>
#include <utility>

#include <base/no_copy_semantics.h>
#include <base/path_utils.h>

namespace Base {

  /* Creates a directory path on construction and destroys it on destruction. */
  class TTmpDirMaker final {
    NO_COPY_SEMANTICS(TTmpDirMaker);
    public:

    /* Ensures the dir exists. */
    template <typename TPath>
    explicit TTmpDirMaker(TPath &&path)
        : Path(std::forward<TPath>(path)) {
      EnsureDirExists(Path.c_str());
    }

    /* Ensures the dir is gone. */
    ~TTmpDirMaker() {
      assert(this);
      EnsureDirIsGone(Path.c_str());
    }

    /* The path to the dir we manage. */
    const std::string &GetPath() const {
      assert(this);
      return Path;
    }

    private:

    /* See accessor. */
    std::string Path;

  };  // TTmpDirMaker

}  // Base
