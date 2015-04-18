/* <base/fd.cc>

   Implements <base/fd.h>.

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

#include <base/fd.h>

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <cassert>

#include <util/error.h>
#include <util/io.h>

using namespace Base;

TFd::TFd() : OsHandle(-1) {}

TFd::TFd(TFd &&that) {
  assert(&that);
  OsHandle = that.OsHandle;
  that.OsHandle = -1;
}

TFd::TFd(int os_handle) { OsHandle = Util::IfLt0(os_handle); }

TFd::~TFd() {
  assert(this);
  if(!IsSystem() && OsHandle != -1) {
    close(OsHandle);
  }
}

TFd &TFd::operator=(TFd &&that) {
  assert(this);
  assert(&that);
  std::swap(OsHandle, that.OsHandle);
  return *this;
}

TFd::operator int() const {
  assert(this);
  return OsHandle;
}

TFd TFd::Duplicate() const {
  return TFd(IsSystem() ? OsHandle : dup(OsHandle));
}

bool TFd::IsOpen() const {
  assert(this);
  return OsHandle >= 0;
}

bool TFd::IsReadable(int timeout) const {
  assert(this);
  pollfd p;
  p.fd = OsHandle;
  p.events = POLLIN;
  int result;
  Util::IfLt0(result = poll(&p, 1, timeout));
  return result != 0;
}

bool TFd::IsSystem() const {
  assert(this);
  return OsHandle < 3 && OsHandle >= 0;
}

int TFd::Release() {
  assert(this);
  int result = OsHandle;
  OsHandle = -1;
  return result;
}

TFd &TFd::Reset() {
  assert(this);
  return *this = TFd();
}

void TFd::Pipe(TFd &readable, TFd &writeable) {
  assert(&readable);
  assert(&writeable);
  int fds[2];
  Util::IfLt0(pipe(fds) < 0);
#ifdef __APPLE__
  Util::IfLt0(fcntl(fds[0], F_SETNOSIGPIPE, 1));
  Util::IfLt0(fcntl(fds[1], F_SETNOSIGPIPE, 1));
#endif
  readable = TFd(fds[0], NoThrow);
  writeable = TFd(fds[1], NoThrow);
}



// TODO: This is a utility that should live in base/
/* Read all the data at fd into one giant buffer in string. Not super efficient, but should be good
   enough, and
   sufficiently dangerous if the data is coming from an untrustworthy source */
std::string Base::ReadAll(TFd &&fd) {
  std::string out;
  uint8_t buf[4096];
  while(ssize_t read = Util::ReadAtMost(fd, buf, 4096)) {
    out.append(reinterpret_cast<char *>(buf), size_t(read));
  }

  return out;
}
