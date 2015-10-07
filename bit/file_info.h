#pragma once

#include <bit/config.h>
#include <bit/naming.h>

namespace Bit {

struct TFileConfig {
  TFileConfig(const TAbsPath &src);
  explicit TFileConfig() = default;
  TJobConfig JobConfig;
};

class TFileInfo {
  public:
  TFileInfo(TRelPath &&path, std::string &&cmd_path, TFileConfig &&src_config);

  bool IsComplete() const;

  // Asserts the file exists, sets ComputedConfig and marks Completed as true.
  void Complete(TFileConfig &&computed_config);

  const std::string CmdPath;
  const TRelPath RelPath;

  private:
  const TFileConfig SrcConfig;
  TFileConfig ComputedConfig;

  // If computerd config has been finalized (the file has been created and is completely done).
  bool Completed = false;
};
}
