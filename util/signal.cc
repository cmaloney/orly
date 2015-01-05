#include <util/signal.h>

#include <cassert>

#include <base/zero.h>
#include <util/error.h>

using namespace Util;

void TSignalHandlerInstaller::DoNothing(int) {}

TSignalHandlerInstaller::TSignalHandlerInstaller(int sig, void (*handler)(int))
    : SignalNumber(sig) {
  struct sigaction new_act;
  Base::Zero(new_act);
  new_act.sa_handler = handler;
  Util::IfLt0(sigaction(sig, &new_act, &OldAct));
}

TSignalHandlerInstaller::~TSignalHandlerInstaller() {
  assert(this);
  sigaction(SignalNumber, &OldAct, nullptr);
}

int TSignalHandlerInstaller::GetSignalNumber() const {
  assert(this);
  return SignalNumber;
}
