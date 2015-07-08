#include <bit/config.h>

#include <cassert>
#include <deque>
#include <fstream>
#include <iterator>

#include <base/json_util.h>
#include <base/not_implemented.h>
#include <base/thrower.h>
#include <util/stl.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Config;
using namespace std;
using namespace Util;

bool LastNotSlash(const string &s) { return s.back() != '/'; }

std::string ReadCacheDir(const TJson &json, const TCoreDirs &core_dirs) {
  const TJson *output_location_json = json.TryAddress({"cache_directory"});
  if(output_location_json) {
    ThrowIfWrongKind(TJson::String, *output_location_json);
    // TODO(cmaloney): string switch
    const std::string &str = output_location_json->GetString();
    if(str == "cache") {
      return core_dirs.User + "/.cache/bit";
    } else if(str == "in_project") {
      return core_dirs.Project + "/.bit/cache";
    } else if(str == "outside_project") {
      // TODO(cmaloney): Normalize the path
      return core_dirs.Project + "/../.bit/cache";
    } else if(str == "tmp") {
      // TODO(cmaloney): obey TMP / TMPDIR / etc. See python's mkstemp()
      return "/tmp/bit_cache";
    } else {
      NOT_IMPLEMENTED();
    }
  } else {
    return core_dirs.Project + "/.bit/cache";
  }
}

TMixinConfig TMixinConfig::Load(const std::string &name, const TCoreDirs &core_dirs) {
  // TODO(cmaloney): Find the mixin in the core dirs.
  NOT_IMPLEMENTED()
  string mixin_filename = name + ".mixin.json";

  TJson json = TJson::Read(mixin_filename);

  TMixinConfig config;
  config.Mixins = ExtractOptional<unordered_set>(json, {"mixins"});
  config.Targets = ExtractOptional<unordered_set>(json, {"targets"});
  config.JobConfig = ReadJobConfig(json);

  return config;
}

TConfig LoadMixins(TConfig &&config, const TCoreDirs &core_dirs) {
  // TODO(cmaloney): Replace std::unordered_set with a more sane datastructure.
  unordered_set<string> known_mixins;

  deque<string> work_list;

  auto add_mixins = [&work_list, &known_mixins](const unordered_set<string> &mixins) {
    // TODO(cmaloney): Should we discard mixin case? Probably...
    for(const string &mixin : mixins) {
      if(Contains(known_mixins, mixin)) {
        continue;
      }

      // Only the system can add 'bit.' reserved mixins.
      if (mixin.compare(0, 4, "bit.")) {
        THROWER(Config::TInvalidValue) << "Only bit can add mixins starting with 'bit.'.";
      }
      known_mixins.insert(mixin);
      work_list.push_back(mixin);
    }
  };

  // Add base mixins
  add_mixins(config.Mixins);

  // TODO(cmaloney): Based on the platform add bit reserved mixins to be loaded
  // EX: osx -> bit.os.osx

  // TODO(cmaloney): Make recursive to generate better error messages?
  // Load all mixins, don't re-load already loaded mixins.
  while(!work_list.empty()) {
    string mixin = work_list.front();
    work_list.pop_front();
    TMixinConfig mixin_conf = TMixinConfig::Load(mixin, core_dirs);
    add_mixins(mixin_conf.Mixins);

    // Add the targets
    config.Targets.insert(mixin_conf.Targets.begin(), mixin_conf.Targets.end());
    DeltaMergeJobs(std::move(config.JobConfig), std::move(mixin_conf.JobConfig));
  }

  return config;
}

TConfig LoadProjectConfig(const string &project_dir) {
  TConfig config;

  TJson json = TJson::Read(project_dir + "bit.json");

  // Load the individual configuration options.
  config.Mixins = ExtractOptional<unordered_set>(json, {"mixins"});
  config.Targets = ExtractOptional<unordered_set>(json, {"targets"});
  config.Jobs = ExtractOptional<unordered_set>(json, {"jobs"});
  config.JobConfig = ReadJobConfig(json);

  return config;
}

TConfig Bit::TConfig::Load(const TCoreDirs &core_dirs) {
  assert(LastNotSlash(core_dirs.Project));
  assert(LastNotSlash(core_dirs.User));
  assert(LastNotSlash(core_dirs.System));

  // Load the project base config.
  TConfig config = LoadProjectConfig(project_dir);

  // Add the user config.
  auto user_json = TJson::TryRead(core_dirs.User + "/.config/bit.json");
  config.CacheDirectory = ReadCacheDir(user_json, project_dir, user_dir, system_dir);

  // TODO(cmaloney): Provide a way for platforms to add / edit / set default flags.
  // TODO(cmaloney): Add distro / OS specific mixins for projects (ex. OSX -> bit.distro.osx mixin)

  // Load all the mixins, apply their config. Return the final config.
  return LoadMixins(config, project_dir, user_dir, system_dir)
}
