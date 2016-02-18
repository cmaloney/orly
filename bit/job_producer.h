#pragma once

#include <memory>
#include <string>
#include <vector>

#include <base/class_traits.h>
#include <base/opt.h>
#include <bit/file_type.h>
#include <bit/naming.h>
#include <bit/job.h>
#include <bit/job_config.h>

namespace Bit {

class TJob;

struct TJobProducer {
  std::string Name;
  TFileType InType;
  std::unordered_set<TFileType> OutTypes;
  std::function<Base::TOpt<TRelPath>(const TRelPath &name)> TryGetInputName;
  std::function<std::unordered_set<TRelPath>(const TRelPath &input)> GetOutputName;
  std::function<std::unique_ptr<TJob>(TJob::TMetadata &&metadata)> MakeJob;
};

}  // namespace Bit
