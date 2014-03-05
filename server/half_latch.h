/* <server/half_latch.h>

   A base class for creating active objects which react to events.

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

#include <base/fd.h>
#include <base/no_copy_semantics.h>

namespace Server {

  /* TODO */
  class THalfLatch {
    NO_COPY_SEMANTICS(THalfLatch);
    public:

    /* TODO */
    THalfLatch();

    /* TODO */
    int GetWaitHandle() const {
      assert(this);
      return RecvFd;
    }

    /* TODO */
    void Recv();

    /* TODO */
    void Send();

    private:

    /* TODO */
    bool IsSet;

    /* TODO */
    Base::TFd RecvFd, SendFd;

  };  // THalfLatch

}  // Server
