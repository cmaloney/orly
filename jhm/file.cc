#include <jhm/file.h>

#include <base/as_str.h>

using namespace Jhm;

TFile::TFile(TRelPath &&path, const TTree *tree, bool is_src, const std::string &config_filename)
    : IsSrc_(is_src),
      Path(std::move(path)),
      CmdPath(is_src ? Base::AsStr(Path) : Base::AsStr(tree->GetAbsPath(Path))),
      Config(config_filename) {}

const TRelPath &TFile::GetRelPath() const { return Path; }
const std::string &TFile::GetPath() const { return CmdPath; }

Util::TOptTimestamp TFile::GetTimestamp() const {
  return Util::Newest(Util::TryGetTimestamp(Base::AsStr(Path)), Config.GetTimestamp());
}

void TFile::PushComputedConfig(Base::TJson &&config) {
  assert(this);
  Config.AddComputed(std::move(config));
}

void TFile::WriteConfig(std::ostream &out) {
  assert(this);
  Config.WriteComputed(out);
}

void TFile::LoadConfig(const std::string &filename) {
  assert(this);
  Config.LoadComputed(filename);
}

void TFile::SetComputed(std::vector<Base::TJson> &&config) {
  assert(this);
  Config.SetComputed(move(config));
}

const TConfig &TFile::GetConfig() const { return Config; }

bool TFile::IsSrc() const { return IsSrc_; }

std::ostream &Jhm::operator<<(std::ostream &out, TFile *f) {
  out << f->GetRelPath();
  return out;
}
