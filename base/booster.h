/* <base/booster.h>

   Construct an instance of this class to boost your thread to high-priority, real-time scheduling.

   NOTE: The process must be running with superuser privileges for this to be allowed.  One way to
   accomlish this is to modify your executable to be owned by root and to have set-uid privilege,
   like this:

      sudo chown root <my_prog>
      sudo chmod +s <my_prog>

   Or you could run the program itself with sudo.

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

#include <stdexcept>

#include <sched.h>

#include <base/no_copy_semantics.h>

namespace Base {

  /* Construct an instance of this class to boost your thread to high-priority, real-time scheduling. */
  class TBooster {
    NO_COPY_SEMANTICS(TBooster);
    public:

    /* Thrown during construction when the boost fails to happen.
       This is probably caused by a lack of privilege.  In this case, pthread_setschedparam()
       *should* fail with EPERM; however, in actual tests, it fails silently.  We therefore
       confirm the scheduling policy change manually and throw this error when the confirmation
       fails. */
    class TIneffective final
        : public std::runtime_error {
      public:

      /* Do-little. */
      TIneffective();

    };  // TIneffective

    /* Boost!  Priority will be the maximum for the policy, minus the given backoff. */
    TBooster(int policy = SCHED_FIFO, int priority_backoff = 0);

    /* We now return you to your regularly programmed scheduling. */
    ~TBooster();

    private:

    /* Cached at construction, these are the scheduling parameters to restore when we destruct. */
    int OldPolicy;
    sched_param OldSchedParam;

    /* A cached copy of the value returned by sched_get_priority_max(SCHED_FIFO). */
    static const int MaxPriority;

  };  // TBooster

}  // Base
