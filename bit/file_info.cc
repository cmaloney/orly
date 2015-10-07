#include <bit/file_info.h>

using namespace Base;
using namespace Bit;
using namespace std;

TFileConfig::TFileConfig(const TAbsPath &src) : JobConfig(ReadJobConfig(TJson::Read(src.Path), src.Path)) {}

TFileInfo::TFileInfo(TRelPath &&path, std::string &&cmd_path, TFileConfig &&src_config)
  : CmdPath(move(cmd_path)), RelPath(move(path)), SrcConfig(move(src_config))  {}
