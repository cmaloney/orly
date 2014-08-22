/* <base/shutting_down.h>

   Functions and classes for helping notify and deal with in individual subsystems shutting down a process gracefully.

   The core functionality is being able to call ShutDown. This notifys all the registered shutdown callbacks that a
   shutdown is occuring, as well as making IsShuttingDown return true.

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

#include <cstdint>
#include <functional>

#include <base/class_traits.h>

namespace Base {

  /* Let subsystems know a shut down is happening */
  void ShutDown();

  /* Returns true iff. ShutDown() has been called. */
  bool IsShuttingDown();

  /* RAII insertion of a function which should be called when shut down is initiated.

     NOTE: This uses a unique integer to identify the callback. As such, the most callbacks you can ever
     register during the lifetime of the program / number of TShutdownCallbacks that can be constructed is
     the max value of uint64_t */
  class TShutdownCallback {
    NO_COPY(TShutdownCallback);
    NO_MOVE(TShutdownCallback);
    public:
    TShutdownCallback(std::function<void()> callback);
    ~TShutdownCallback();

    private:
    uint64_t Id;
  };
}  // Base
