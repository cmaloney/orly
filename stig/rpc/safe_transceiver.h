/* <stig/rpc/safe_transceiver.h> 

   Performs scatter/gather I/O which is interruptable (in case of system shutdown, for example)
   and which can also be limited by deadlines (to detect slow peers).

   To use a deadline, set the desired deadline interval by calling SetDeadlineInterval(),
   then call ArmDeadline().  The deadline will start counting down the next time you call
   Recv() or Send().  The deadline will continue to count down until it is disarmed.

   If you call Recv() or Send() after the deadline has passed, you will get an exception.
   To stop that from happening, you must disarm the deadline.

   To disarm a the deadline, call DisarmDeadline().  If you called ArmDeadline() multiple
   times, you'll have to call DisarmDeadline() an equal number of times before the deadline
   will actually be disarmed.

   Copyright 2010-2014 Tagged
   
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
#include <chrono>
#include <cstddef>
#include <stdexcept>

#include <poll.h>

#include <base/no_copy_semantics.h>
#include <stig/rpc/transceiver.h>

namespace Stig {

  namespace Rpc {

    /* Exchanges messages over a connected streaming socket. */
    class TSafeTransceiver final {
      NO_COPY_SEMANTICS(TSafeTransceiver);
      public:

      /* Thrown when an I/O is intentionally interrupted. */
      class TInterrupted
          : public std::runtime_error {
        public:

        /* Do-little. */
        TInterrupted();

      };  // TSafeTransceiver::TInterrupted;

      /* Thrown when the socket is unready for too long. */
      class TTimedOut
          : public std::runtime_error {
        public:

        /* Do-little. */
        TTimedOut();

      };  // TSafeTransceiver::TTimedOut;

      /* Caches the interrupt fd, which must be open.
         The default deadline interval is 2 seconds and the deadline starts out disarmed. */
      TSafeTransceiver(int interrupt_fd = -1);

      /* Closes the deadline timer, if any. */
      ~TSafeTransceiver();

      /* Arms the deadline. */
      void ArmDeadline();

      /* Disarms the deadline. */
      void DisarmDeadline();

      /* Return a pointer to the start of an array of iovecs containing as many elements as requested.
         The array is owned by the tranceiver object, so don't delete it.  The contents of the array are maintained
         even if you call this function multiple times, so you can expand the array if you need to. */
      iovec *GetIoVecs(size_t size) {
        assert(this);
        return Transceiver.GetIoVecs(size);
      }

      /* Scatter-read into the buffers currently in the iovec array.  The fd must be an open socket.
         Flags are as for recvmsg().  If there is an I/O error, or if our peer hangs up, or if the deadline
         passes, we throw. */
      void Recv(int sock_fd, int flags = 0);

      /* Gather-write out of the buffers currently in the iovec array.  The fd must be an open socket.
         Flags are as for recvmsg().  If there is an I/O error, or if our peer hangs up, or if the deadline
         passes, we throw.  NOTE: This function always sets the MSG_NOSIGNAL. */
      void Send(int sock_fd, int flags = 0);

      /* Set the deadline interval to the given value, which must be non-zero.
         This will take effect the next time the deadline is armed. */
      void SetDeadlineInterval(const std::chrono::milliseconds &deadline_interval) {
        assert(this);
        assert(deadline_interval.count());
        DeadlineInterval = deadline_interval;
      }

      private:

      /* Wait for the socket to become ready to read and/or write.
         The given flags determine what we're waiting for.  They must be POLLIN, POLLOUT, or both or'd together.
         If the interrupt event occurs or the deadline passes while we're waiting, we will throw appropriately. */
      void WaitForSocket(int sock_fd, int flags);

      /* Manages the iovec array and performs partial scatter/gather I/O. */
      TTransceiver Transceiver;

      /* When non-zero, WaitForSocket() will include the deadline timer in its polling operation. */
      size_t ArmingCount;

      /* The amount of time to wait before timing out. */
      std::chrono::milliseconds DeadlineInterval;

      /* If true, then WaitForSocket() will reset the deadline timer to a point in the near future and start
         it counting down.  This is set by the first call to ArmDeadline() while the deadline is unarmed and
         then cleared by WaitForSocket() after the reset is accomplished. */
      bool ResetIsRequired;

      /* Wait() uses this poller to wait for:
            event #0, the interrupt event has occurred; or
            event #1, the socket to is ready to read and/or write;
            event #2, the deadline has passed. */
      pollfd Poller[3];

    };  // TSafeTransceiver

  }  // Rpc

}  // Stig

