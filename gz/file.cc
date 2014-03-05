/* <gz/file.cc>

   Implements <gz/file.h>.

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

#include <gz/file.h>

#include <cerrno>

#include <base/error_utils.h>

using namespace std;
using namespace Base;
using namespace Gz;

TFile::TFile(const char *path, const char *mode) {
  assert(path);
  assert(mode);
  Handle = gzopen(path, mode);
  if (!Handle) {
    if (errno) {
      ThrowSystemError(errno);
    }
    THROW_ERROR(TCouldNotOpen) << '"' << path << '"';
  }
}

TFile::TFile(int fd, const char *mode) {
  assert(fd >= 0);
  assert(mode);
  Handle = gzdopen(fd, mode);
  if (!Handle) {
    if (errno) {
      ThrowSystemError(errno);
    }
    THROW_ERROR(TCouldNotAdopt);
  }
}

TFile::TFile(TFd &&fd, const char *mode) {
  assert(&fd);
  assert(fd.IsOpen());
  assert(mode);
  Handle = gzdopen(fd, mode);
  if (!Handle) {
    if (errno) {
      ThrowSystemError(errno);
    }
    THROW_ERROR(TCouldNotAdopt);
  }
  fd.Release();
}

TFile::~TFile() {
  assert(this);
  if (Handle) {
    gzclose(Handle);
  }
}

size_t TFile::ReadAtMost(void *buffer, size_t size) {
  assert(this);
  assert(buffer || !size);
  assert(Handle);
  ssize_t actl = gzread(Handle, buffer, size);
  //Note: gzread returns 0 for eof, which is what we manually do with the gzeof check for older gzips.
  if (actl < 0) {
    if (gzeof(Handle)) {
      actl = 0;
    } else {
      int code;
      const char *msg = gzerror(Handle, &code);
      if (code == Z_ERRNO) {
        ThrowSystemError(errno);
      }
      THROW_ERROR(TCouldNotRead) << msg;
    }
  }
  return actl;
}

size_t TFile::WriteAtMost(const void *buffer, size_t size) {
  assert(this);
  assert(buffer || !size);
  assert(Handle);
  ssize_t actl = gzwrite(Handle, buffer, size);
  if (actl <= 0) {
    int code;
    const char *msg = gzerror(Handle, &code);
    if (code == Z_ERRNO) {
      ThrowSystemError(errno);
    }
    THROW_ERROR(TCouldNotWrite) << msg;
  }
  return actl;
}
