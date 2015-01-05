/* <base/fd.h>

   An RAII container for an OS file descriptor.

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

#include <string>

#include <base/class_traits.h>

namespace Base {

/* An RAII container for an OS file descriptor.

   This is a value type.  If you copy an instance of this class, it will use dup() to copy the file
   descriptor it contains (if any).

   This class implicitly casts to int, so you can use an instance of it in any call where you would
   normally pass a naked file descriptor.

   You can construct an instance of this class to capture the result of a function, such as the OS
   function socket(), which returns a newly created
   file descriptor.

   For example:

      TFd sock(HERE, socket(IF_INET, SOCK_STREAM, IPPROTO_TCP));

   If socket() fails (and so returns a negative value), the TFd constructor will throw an instance
   of std::system_error.

   You may also pass a naked file descriptor in the stdio range (0-2) to this constructor.  In this
   case, the newly constructed object will hold the
   file desciptor, but it will not attempt to close it. */
class TFd {
  public:
  NO_COPY(TFd);

  /* Default-construct as an illegal value (-1). */
  TFd();

  /* Move-construct, leaving the donor in the default-constructed state. */
  TFd(TFd &&that);

  /* Construct from a naked file descriptor, which the new instance will own.  Use this constructor
     to capture the result of an OS function, such as
     socket(), which returns a newly created file descriptor.  If the result is not a legal file
     descriptor, this function will throw the
     appropriate error.  */
  TFd(int os_handle);

  /* Close the file descriptor we own, if any.  If the descriptor is in the stdio range (0-2), then
   * don't close it. */
  ~TFd();

  /* Swaperator. */
  TFd &operator=(TFd &&that);

  /* Returns the naked file descriptor, which may be -1. */
  operator int() const;

  /* Make a copy of this file descriptor with an independent lifetime. */
  TFd Duplicate() const;

  /* True iff. this handle is open. */
  bool IsOpen() const;

  /* True iff. the file descriptor can be read from without blocking.
     Waits for at most the given number of milliseconds for the descriptor to become readable.
     A negative timeout will wait forever. */
  bool IsReadable(int timeout = 0) const;

  /* True iff this is a system fd (stdin, stdout, stderr) */
  bool IsSystem() const;

  /* Returns the naked file desciptor, which may be -1, and returns to the default-constructed
     state.  This is how to get the naked file desciptor
     away from the object without the object attempting to close it. */
  int Release();

  /* Return to the default-constructed state. */
  TFd &Reset();


  /* Construct the read- and write-only ends of a pipe. */
  static void Pipe(TFd &readable, TFd &writeable);

  private:
  /* Use to make it cheap to construct off of what we know is valid. */
  enum TNoThrow { NoThrow };

  TFd(int os_handle, TNoThrow) : OsHandle(os_handle) {}

  /* The naked file descriptor we wrap.  This can be -1. */
  int OsHandle;

};  // TFd

/* Read everything on the fd.
   Puts it into a giant buffer in a string.
   NOTE: Don't do this with untrusted sources, for obvious reasons. */
std::string ReadAll(TFd &&fd);

// TODO(cmaloney) stdin, stdout, stderr wrappers.
}  // Base
