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
`   See the License for the specific language governing permissions and
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

size_t Util::ReadAtMost(int fd, void *buf, size_t max_size, std::chrono::milliseconds timeout) {
  if (timeout >= chrono::milliseconds::zero()) {
    struct pollfd event;
    event.fd = fd;
    event.events = POLLIN;
    event.revents = 0;
    int ret = IfLt0(poll(&event, 1, timeout.count()));

    if (ret == 0) {
      ThrowSystemError(ETIMEDOUT);
    }
  }

  return ReadAtMost(fd, buf, max_size);
}

size_t Util::WriteAtMost(int fd, const void *buf, size_t max_size) {
  struct stat stat;
  IfLt0(fstat(fd, &stat));
  return IfLt0(S_ISSOCK(stat.st_mode) ? send(fd, buf, max_size, MSG_NOSIGNAL) : write(fd, buf, max_size));
}

size_t Util::WriteAtMost(int fd, const void *buf, size_t max_size, chrono::milliseconds timeout) {
  if (timeout >= chrono::milliseconds::zero()) {
    struct pollfd event;
    event.fd = fd;
    event.events = POLLOUT;
    event.revents = 0;
    int ret = IfLt0(poll(&event, 1, timeout.count()));

    if (ret == 0) {
      ThrowSystemError(ETIMEDOUT);
    }
  }

  return WriteAtMost(fd, buf, max_size);
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

bool Util::TryReadExactly(int fd, void *buf, size_t size, chrono::milliseconds timeout) {
  if (timeout < chrono::milliseconds::zero()) {
    return TryReadExactly(fd, buf, size);
  }

  if (size == 0) {
    return true;
  }

  char
      *csr = static_cast<char *>(buf),
      *end = csr + size;

  auto deadline = chrono::steady_clock::now() + timeout;
  auto time_left = timeout;

  for (; ; ) {
    size_t actual_size = ReadAtMost(fd, csr, end - csr, time_left);

    if (!actual_size) {
      if (csr > buf) {
        throw TUnexpectedEnd();
      }

      return false;
    }

    csr += actual_size;

    if (csr >= end) {
      assert(csr == end);
      break;
    }

    time_left = chrono::duration_cast<chrono::milliseconds>(max(deadline - chrono::steady_clock::now(), chrono::steady_clock::duration::zero()));
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

bool Util::TryWriteExactly(int fd, const void *buf, size_t size, chrono::milliseconds timeout) {
  if (timeout < chrono::milliseconds::zero()) {
    return TryWriteExactly(fd, buf, size);
  }

  if (size == 0) {
    return true;
  }

  const char
      *csr = static_cast<const char *>(buf),
      *end = csr + size;;
  auto deadline = chrono::steady_clock::now() + timeout;
  auto time_left = timeout;

  for (; ; ) {
    size_t actual_size = WriteAtMost(fd, csr, end - csr, time_left);

    if (!actual_size) {
      if (csr > buf) {
        throw TUnexpectedEnd();
      }

      return false;
    }

    csr += actual_size;

    if (csr >= end) {
      assert(csr == end);
      break;
    }

    time_left = chrono::duration_cast<chrono::milliseconds>(max(deadline - chrono::steady_clock::now(), chrono::steady_clock::duration::zero()));
  }

  return true;
}

void Util::SetCloseOnExec(int fd) {
  int flags;
  IfLt0(flags = fcntl(fd, F_GETFD, 0));
  IfLt0(fcntl(fd, F_SETFD, flags | O_CLOEXEC));
}

void Util::SetNonBlocking(int fd) {
  int flags;
  IfLt0(flags = fcntl(fd, F_GETFL, 0));
  IfLt0(fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}
