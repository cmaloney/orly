#include <bit/file_info.h>

#include <base/not_implemented.h>

using namespace Base;
using namespace Bit;
using namespace std;

TFileConfig::TFileConfig(const TAbsPath &src)
    : JobConfig(ReadJobConfig(TJson::TryRead(src.Path), src.Path)) {}

TFileInfo::TFileInfo(TRelPath &&path, std::string &&cmd_path, TFileConfig &&src_config, bool is_src)
    : CmdPath(move(cmd_path)),
      RelPath(move(path)),
      SrcConfig(),
      // TODO(cmaloney): Support SrcConfig, SrcConfig(move(src_config)),
      Completed(is_src),
      Buildable(is_src),
      BuildableSet(is_src) {
  assert(src_config.JobConfig.empty());  // No src config is currently supported
}

std::optional<bool> TFileInfo::IsBuildable() const {
  if (BuildableSet) {
    return std::optional<bool>(Buildable);
  }
  return std::optional<bool>();
}

bool TFileInfo::IsComplete() const { return Completed; }

// TODO(cmaloney): Switch to opt
const TFileConfig *TFileInfo::GetCompleteConfig() const {
  if (Completed) {
    return &FinalConfig;
  } else {
    return nullptr;
  }
}

void TFileInfo::Complete(const TJob *, TJobConfig &&file_config) {
  FinalConfig.JobConfig = move(file_config);
  Completed = true;
}

void TFileInfo::SetIsBuildable(bool is_buildable) {
  Buildable = is_buildable;
  BuildableSet = true;
}

std::ostream &Bit::operator<<(std::ostream &out, const TFileInfo *file_info) {
  // TODO(cmaloney): Make a more interesting representation for file_info based
  // on trees?
  out << file_info->CmdPath;

  return out;
}
