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

  // Asserts the file exists, sets ComputedConfig and marks Completed as true.
  void Complete(TFileConfig &&computed_config);

  const TRelPath RelPath;

  private:
  bool Completed;
  bool IsSrc;
  TFileConfig SrcConfig;
  TFileConfig ComputedConfig;
};
}
