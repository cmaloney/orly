#pragma once

#include <iosfwd>

#include <bit/config.h>
#include <bit/naming.h>

namespace Bit {

class TJob;

struct TFileConfig {
  TFileConfig(const TAbsPath &src);
  explicit TFileConfig() = default;
  TJobConfig JobConfig;
};

class TFileInfo {
  public:
  TFileInfo(TRelPath &&path, std::string &&cmd_path, TFileConfig &&src_config, bool is_src);

  bool IsComplete() const;

  // Asserts the file exists, sets ComputedConfig and marks Completed as true.
  // Attaches all critical bits from the job to the file so the file can
  // potentially be cache completed if everything checks out.
  void Complete(const TJob *producer, TJobConfig &&extra_data);

  const std::string CmdPath;
  const TRelPath RelPath;

  private:
  const TFileConfig SrcConfig;
  TFileConfig ComputedConfig;

  // If computerd config has been finalized (the file has been created and is completely done).
  bool Completed = false;
};

std::ostream &operator << (std::ostream &out, const TFileInfo *file_info);

}
