#pragma once

#include <string>

#include <base/json.h>

namespace Bit {
  // TODO(cmaloney): Currently this is a flat store, mix of parameters for
  // commands and computed extra data. Should split out the different data
  // channels and ideally set it up so the data can be stored in a native format
  // then only serialized to json when the job config is being written to disk.
  using TJobConfig = std::map<std::string, Base::TJson>;
}
