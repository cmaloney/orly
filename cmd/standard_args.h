/* Standard arguments whichc can be given to practically all programs. */
#pragma once

#include <cmd/args.h>

namespace Cmd {

struct TStandardOptions {
  bool Help = false;
};

inline TArgs<TStandardOptions> GetStandardArgs() {
  return {
    Optional({"help","h"}, &TStandardOptions::Help, "Print description of command and arguments message")
  };
};

}
