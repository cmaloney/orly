/* <starsha/pipe.h>

   TODO

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
#include <string>

#include <sys/epoll.h>

#include <base/no_copy_semantics.h>

namespace Starsha {

  /* TODO */
  class TPipe {
    NO_COPY_SEMANTICS(TPipe);
    public:

    /* TODO */
    static const int In = 0, Out = 1;

    /* TODO */
    class TPoller {
      NO_COPY_SEMANTICS(TPoller);
      public:

      /* TODO */
      TPoller(int flags = 0);

      /* TODO */
      ~TPoller();

      /* TODO */
      void Add(TPipe *pipe);

      /* TODO */
      void Remove(TPipe *pipe);

      /* TODO */
      TPipe *Wait();

      private:

      /* TODO */
      size_t PipeCount;

      /* TODO */
      int Fd;

    };  // TPoller

    /* TODO */
    TPipe();

    /* TODO */
    ~TPipe();

    /* TODO */
    void Close(int idx);

    /* TODO */
    void MoveFd(int idx, int as_fd);

    /* TODO */
    size_t Read(void *buf, size_t max_size);

    /* TODO */
    size_t Write(const void *buf, size_t max_size);

    private:

    /* TODO */
    bool IsOpen[2];

    /* TODO */
    int Fds[2];

  };  // TPipe

}  // Starsha
