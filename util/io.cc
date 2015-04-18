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

#include <unistd.h>
#include <sys/fcntl.h>

#include <util/error.h>

using namespace Base;
using namespace std;

bool Util::IsValidFd(int fd) { return fcntl(fd, F_GETFD) >= 0; }

ssize_t Util::ReadAtMost(int fd, void *buf, size_t max_size) {
  return IfLt0(read(fd, buf, max_size));
}

ssize_t Util::WriteAtMost(int fd, const void *buf, size_t max_size) {
  if(max_size == 0) {
    return 0;
  }
  return write(fd, buf, max_size);
}

bool Util::TryReadExactly(int fd, void *buf, size_t size) {
  char *cursor = static_cast<char *>(buf), *end = cursor + size;
  ;
  while(cursor < end) {
    ssize_t actual_size = ReadAtMost(fd, cursor, size_t(end - cursor));
    if(!actual_size) {
      if(cursor > buf) {
        throw TUnexpectedEnd();
      }
      return false;
    }
    cursor += actual_size;
  }
  return true;
}

bool Util::TryWriteExactly(int fd, const void *buf, size_t size) {
  const char *cursor = static_cast<const char *>(buf), *end = cursor + size;
  ;
  while(cursor < end) {
    ssize_t actual_size = WriteAtMost(fd, cursor, size_t(end - cursor));
    if(!actual_size) {
      if(cursor > buf) {
        throw TUnexpectedEnd();
      }
      return false;
    }
    cursor += actual_size;
  }
  return true;
}

void Util::SetNonBlocking(int fd) {
  int flags;
  IfLt0(flags = fcntl(fd, F_GETFL, 0));
  IfLt0(fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}
