/* <util/io.h>

   I/O utilities.

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

#include <iomanip>
#include <stdexcept>

#include <base/thrower.h>
#include <util/error.h>

namespace Util {

  DEFINE_ERROR(TOpenFileError, std::runtime_error, "could not open file")

  template <typename TStrm>
  void OpenFile(TStrm &strm, const std::string &path) {
    //TODO: This should move out the stream, but we can't because libstdc++ 4.9 hasn't implemented move for at least ifstream
    assert(&path);

    strm.exceptions(std::ios_base::failbit);
    try {
      strm.open(path);
    } catch (const std::ios_base::failure &ex) {
      char temp[256];
      temp[0] = '\0';
      Util::Strerror(errno, temp, sizeof(temp));
      THROW_ERROR(TOpenFileError) << std::quoted(path) << Base::EndOfPart << temp;
    }
  }

  /* Returns true iff the fd is valid. */
  bool IsValidFd(int fd);

  /* The 'AtMost' versions of read and write are basically just wrappers around
     the OS functions.  They will transfer as much data as possible, up to the
     given max, and return the number of bytes they tranferred.  They will raise
     std::system_error if anything goes wrong or if they are interrupted. */

  /* Read at most 'max_size' bytes into 'buf', throwing appropriately. */
  size_t ReadAtMost(int fd, void *buf, size_t max_size);

  /* Write at most 'max_size' bytes from 'buf', throwing appropriately.
     If the fd is a socket, we will do this operation with send() instead of write(),
     to suppress SIGPIPE. */
  size_t WriteAtMost(int fd, const void *buf, size_t max_size);

  /* The 'TryExactly' versions of read and write will call the OS functions
     repeatedly until the full number of bytes is transferred.  If the first OS call
     results in zero bytes being transferred (indicating, for example, the the
     other end of the pipe is closed), the function will stop trying to transfer
     and return false.  If any subseqeuent transfer returns zero bytes, the function
     will throw a std::runtime_error indicating that the transfer ended
     unexpectedly.  If the full number of bytes are tranferred successfully, the
     function returns true. */

  /* An instance of this class is thrown by the 'TryExactly' functions when a
     transfer was started successfully but could not finish. */
  class TUnexpectedEnd
      : public std::runtime_error {
    public:

    /* Do-little. */
    TUnexpectedEnd()
        : std::runtime_error("unexpected end") {}

  };  // TUnexpectedEnd

  /* Try to read exactly 'size' bytes into 'buf'.
     Retry until we get enough bytes, then return true.
     If we get a zero-length return, return false.
     Throw appropriately. */
  bool TryReadExactly(int fd, void *buf, size_t size);

  /* Try to write exactly 'size' bytes from 'buf'.
     Retry until we put enough bytes, then return true.
     If we get a zero-length return, return false.
     Throw appropriately. */
  bool TryWriteExactly(int fd, const void *buf, size_t size);

  /* The 'Exactly' versions of read and write work like the 'TryExactly' versions
     (see above), except that they do not tolerate a failure to start.  If the transfer
     could start, they a throw std::runtime_error indicating so. */

  /* An instance of this class is thrown by the 'Exactly' functions when a
     transfer could not start. */
  class TCouldNotStart
      : public std::runtime_error {
    public:

    /* Do-little. */
    TCouldNotStart()
        : std::runtime_error("could not start") {}

  };  // TCouldNotStart

  /* Read exactly 'size' bytes into 'buf', throwing appropriately. */
  inline void ReadExactly(int fd, void *buf, size_t size) {
    if (!TryReadExactly(fd, buf, size)) {
      throw TCouldNotStart();
    }
  }

  /* Write exactly 'size' bytes into 'buf', throwing appropriately. */
  inline void WriteExactly(int fd, const void *buf, size_t size) {
    if (!TryWriteExactly(fd, buf, size)) {
      throw TCouldNotStart();
    }
  }

  /* Sets the given fd to non-blocking I/O. */
  void SetNonBlocking(int fd);

}  // Util
