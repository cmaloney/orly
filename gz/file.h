/* <gz/file.h>

   A gzipped file.

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

#include <zlib.h>

#include <base/fd.h>
#include <base/no_copy_semantics.h>
#include <base/thrower.h>

namespace Gz {

  /* A gzipped file. */
  class TFile final {
    NO_COPY_SEMANTICS(TFile);
    public:

    /* The errors we throw. */
    DEFINE_ERROR(TCouldNotAdopt, std::runtime_error, "could not adopt gzip file");
    DEFINE_ERROR(TCouldNotOpen,  std::runtime_error, "could not open gzip file");
    DEFINE_ERROR(TCouldNotRead,  std::runtime_error, "could not read gzip file");
    DEFINE_ERROR(TCouldNotWrite, std::runtime_error, "could not write gzip file");

    /* Default construct as a closed file. */
    TFile()
        : Handle(nullptr) {}

    /* Move-construct from that file.  That file will end up closed. */
    TFile(TFile &&that) {
      assert(&that);
      Handle = that.Handle;
      that.Handle = nullptr;
    }

    /* Opens the given path with the given access mode(s). */
    TFile(const char *path, const char *mode);

    /* Adopts the given fd as a compressed file with the given access mode(s). */
    TFile(int fd, const char *mode);

    /* Adopts the given fd as a compressed file with the given access mode(s). */
    TFile(Base::TFd &&fd, const char *mode);

    /* Closes the file. */
    ~TFile();

    /* Move-assign from that file to this one.  That file will end up closed.
       If this file is already open, it will close. */
    TFile &operator=(TFile &&that) {
      assert(this);
      TFile temp;
      temp.Handle = Handle;
      Handle = that.Handle;
      that.Handle = nullptr;
      return *this;
    }

    /* True iff. the file is open. */
    operator bool() const {
      assert(this);
      return Handle != nullptr;
    }

    /* Read at most the given number of bytes from the compressed file.
       Return the actual number of bytes read.  On error, throw.
       The file must be open. */
    size_t ReadAtMost(void *buffer, size_t size);

    /* Write at most the given number of bytes to the compressed file.
       Return the actual number of bytes written.  On error, throw.
       The file must be open. */
    size_t WriteAtMost(const void *buffer, size_t size);

    private:

    /* The handle returned by gzopen(), q.v. */
    gzFile Handle;

  };  // TFile

}  // Gz
