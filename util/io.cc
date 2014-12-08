/* <util/io.cc>

   Implements <util/io.h>.

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

#include <util/io.h>

#include <cassert>
#include <cerrno>

#include <poll.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <util/error.h>

using namespace Base;
using namespace std;

bool Util::IsValidFd(int fd) {
  return fcntl(fd, F_GETFD) >= 0;
}

size_t Util::ReadAtMost(int fd, void *buf, size_t max_size) {
  return IfLt0(read(fd, buf, max_size));
}

size_t Util::WriteAtMost(int fd, const void *buf, size_t max_size) {
  struct stat stat;
  IfLt0(fstat(fd, &stat));
  if (IfLt0(S_ISSOCK(stat.st_mode))) {
#if __APPLE__
    return send(fd, buf, max_size, 0);
#else
    return send(fd, buf, max_size, MSG_NOSIGNAL);
#endif
  } else {
    return write(fd, buf, max_size);
  }
}

bool Util::TryReadExactly(int fd, void *buf, size_t size) {
  char
      *csr = static_cast<char *>(buf),
      *end = csr + size;;
  while (csr < end) {
    size_t actual_size = ReadAtMost(fd, csr, end - csr);
    if (!actual_size) {
      if (csr > buf) {
        throw TUnexpectedEnd();
      }
      return false;
    }
    csr += actual_size;
  }
  return true;
}

bool Util::TryWriteExactly(int fd, const void *buf,
    size_t size) {
  const char
      *csr = static_cast<const char *>(buf),
      *end = csr + size;;
  while (csr < end) {
    size_t actual_size = WriteAtMost(fd, csr, end - csr);
    if (!actual_size) {
      if (csr > buf) {
        throw TUnexpectedEnd();
      }
      return false;
    }
    csr += actual_size;
  }
  return true;
}

void Util::SetNonBlocking(int fd) {
  int flags;
  IfLt0(flags = fcntl(fd, F_GETFL, 0));
  IfLt0(fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}
