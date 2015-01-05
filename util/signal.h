/* Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <signal.h>

#include <base/class_traits.h>
#include <base/zero.h>
#include <util/error.h>

namespace Util {

/* RAII for installing a signal handler. */
class TSignalHandlerInstaller {
  NO_COPY(TSignalHandlerInstaller);

  public:
  /* Set the mask to the given set. */
  TSignalHandlerInstaller(int sig, void (*handler)(int) = DoNothing) : SignalNumber(sig) {
    struct sigaction new_act;
    Base::Zero(new_act);
    new_act.sa_handler = handler;
    Util::IfLt0(sigaction(sig, &new_act, &OldAct));
  }

  /* Restore the old action. */
  ~TSignalHandlerInstaller() {
    assert(this);
    sigaction(SignalNumber, &OldAct, nullptr);
  }

  /* The signal we handle. */
  int GetSignalNumber() const {
    assert(this);
    return SignalNumber;
  }

  private:
  /* The do-nothing handler. */
  static void DoNothing(int sig);

  /* See accessor. */
  int SignalNumber;

  /* The action which we will restore. */
  struct sigaction OldAct;

};  // TSignalHandlerInstaller

}  // Signal
