#include <bit/config.h>

#include <cassert>

using namespace bit;
using namespace std;

bool LastNotSlash(const string &s) {
  return s.empty() ? true : s.back() != '/';
}

string get_mixin_dir(string dir) {
  return dir + "/mixins";
}

TConfig bit::TConfig::Load(string project_dir, string user_dir, string system_dir) {
  assert(LastNotSlash(project_dir));
  assert(LastNotSlash(user_dir));
  assert(LastNotSlash(system_dir));

  nlohmann::json::parse(project_dir + "/bit.json");
  nlohmann::json::parse(user_dir + "/bit.json");
  nlohmann::json::parse(system_dir + "/bit.json");
  return TConfig();
}
