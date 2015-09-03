#pragma once

#include <memory>
#include <string>
#include <vector>

#include <base/class_traits.h>
#include <base/opt.h>
#include <bit/naming.h>
#include <util/time.h>

namespace Bit {

class TJob;

struct TJobProducer {

  template<typename TVal>
  using TSet = std::unordered_set<TVal>;

  std::string Name;
  // TODO: Should really be a set...
  std::vector<std::vector<std::string>> OutExtensions;
  std::function<Base::TOpt<TRelPath>(const TRelPath &name)> TryGetInputName;
  std::function<TSet<TRelPath>(const TRelPath &input)> GetOutput;
  std::function<std::unique_ptr<TJob>(const TJobProducer *producer, TFileInfo *input, TSet<TFileInfo *> &&output)> MakeJob;
  Util::TTimestamp CmdTimestamp;
};

}  // namespace Bit
