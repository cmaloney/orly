#pragma once

#include <atomic>
#include <iosfwd>

#include <bit/config.h>
#include <bit/naming.h>

class TStatusTracker;

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

  // Returns the complete config if IsComplete is true, otherwise returns nullptr.
  TFileConfig *GetCompleteConfig();

  Base::TOpt<bool> IsBuildable() const;

  bool IsComplete() const;

  const std::string CmdPath;
  const TRelPath RelPath;

  private:
  // Asserts the file exists, sets ComputedConfig and marks Completed as true.
  // Attaches all critical bits from the job to the file so the file can
  // potentially be cache completed if everything checks out.
  void Complete(const TJob *producer, TJobConfig &&extra_data);
  void SetIsBuildable(bool is_buildable);

  const TFileConfig SrcConfig;
  TFileConfig FinalConfig;

  // If computerd config has been finalized (the file has been created and is completely done).
  // This is only set once and can never be unset.
  std::atomic<bool> Completed;

  // Two atomic bools is far more efficient than one atomic TOpt<> most likely.
  std::atomic<bool> Buildable;
  std::atomic<bool> BuildableSet;

  // So TStatusTracker can complete files.
  friend class ::TStatusTracker;
};

std::ostream &operator << (std::ostream &out, const TFileInfo *file_info);

}
