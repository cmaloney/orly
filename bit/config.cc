#include <bit/config.h>

#include <cassert>
#include <fstream>
#include <iterator>

#include <base/thrower.h>

using namespace Bit;
using namespace Bit::Config;
using namespace nlohmann;
using namespace std;

void Append(vector<string> &lhs, vector<string> &&rhs) {
  lhs.reserve(lhs.size() + rhs.size());
  std::move(rhs.begin(), rhs.end(), back_inserter(lhs));
}

string get_mixin_dir(string dir) { return dir + "/mixins"; }

TConfig LoadMixin(TConfig &&config, std::string MixinName);

json LoadJson(const string &filename) {
  ifstream in(filename);
  if(!in.is_open()) {
    THROWER(std::runtime_error) << "Unable to open file " << std::quoted(filename);
  }

  try {
    return json::parse(in);
  } catch(const invalid_argument &ex) {
    THROWER(runtime_error) << "in " << std::quoted(filename) << ':' << ex.what();
  }
}

TConfig LoadConfig(TConfig &&config, TValidSections allowed_sections, const std::string &filename) {
  json json = LoadJson(filename);
  if(allowed_sections.DefaultTargets) {
    Append(config.DefaultTargets, json.at("targets").get<vector<string>>());
  }
  if(allowed_sections.DefaultMixins) {
    Append(config.EnabledMixins, json.at("mixins").get<vector<string>>());
  }
}

/* Load project config.

Projects can specify
  - Mixins
  - Jobs to enable / disable
  - Targets to build

*/
TConfig LoadProjectConfig(const string &project_dir) {
  TConfig config;

  json file = LoadJson(project_dir + "bit.json");

  if(file.is_ob)

    addMixins(config, file);
  //

  return config;
}

TConfig Bit::TConfig::Load(const string &project_dir,
                           const string &user_dir,
                           const string &system_dir) {
  assert(LastNotSlash(project_dir));
  assert(LastNotSlash(user_dir));
  assert(LastNotSlash(system_dir));

  TConfig config = LoadProjectConfig(project_dir);

  json::parse(project_dir + "/bit.json");
  json::parse(user_dir + "/bit.json");
  json::parse(system_dir + "/bit.json");
  return TConfig();
}
