/* Copyright 2015 Theoretical Chaos.

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

#include <signal.h>

#include <base/class_traits.h>


namespace Util {

/* RAII for installing a signal handler. */
class TSignalHandlerInstaller {
  NO_COPY(TSignalHandlerInstaller)

  public:
  /* Set the mask to the given set. */
  TSignalHandlerInstaller(int sig, void (*handler)(int) = DoNothing);

  /* Restore the old action. */
  ~TSignalHandlerInstaller();

  /* The signal we handle. */
  int GetSignalNumber() const;

  private:
  /* The do-nothing handler. */
  static void DoNothing(int sig);

  /* See accessor. */
  int SignalNumber;

  /* The action which we will restore. */
  struct sigaction OldAct;

};  // TSignalHandlerInstaller

}  // Signal
