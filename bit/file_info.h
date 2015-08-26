#pragma once

#include <bit/config.h>
#include <bit/naming.h>

namespace Bit {

class TFileConfig {
  TJobConfig JobConfig;
};

class TFileInfo {
  public:
  TFileInfo(TRelPath &&path, const TTree *tree, bool is_src, TFileConfig src_config);

  bool IsComplete() const;

  // Asserts the file exists.
  void Complete(TFileConfig &&computed_config);

  private:
  const TRelPath Path;
  bool Completed;
  bool IsSrc;
  TFileConfig Config;
};
}
