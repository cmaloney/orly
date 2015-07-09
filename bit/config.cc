#include <bit/config.h>

#include <cassert>
#include <deque>
#include <fstream>
#include <iterator>

#include <base/json_util.h>
#include <base/not_implemented.h>
#include <base/thrower.h>
#include <util/path.h>
#include <util/stl.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Config;
using namespace std;
using namespace Util;

void AppendMerge(TJson &target, TJson &&addin) {
  // TODO(cmaloney): Use the stack to provide a good error message of what we were trying to merge.
  if(target.GetKind() != addin.GetKind()) {
    THROWER(Config::TInvalidValue)
        << "Unable to merge addin into target. Can't merge non-identical types " << target.GetKind()
        << " and " << addin.GetKind();
  }

  // Merge arrays, dictionaries. Ignore everything else.
  switch(target.GetKind()) {
    case TJson::Object: {
        auto &t_o = target.GetObject();
        auto &a_o = addin.GetObject();
        for(auto &item: a_o) {
          auto iter = t_o.find(item.first);

          if (iter == t_o.end()) {
            t_o.emplace(std::move(item.first), std::move(item.second));
          } else {
            AppendMerge(iter->second, move(item.second));
          }
        }
        break;
      }
    case TJson::Array: {
      auto &target_array = target.GetArray();
      auto &addin_array = addin.GetArray();
      target_array.reserve(target_array.size() + addin_array.size());
      std::move(addin_array.begin(), addin_array.end(), back_inserter(target_array));
      break;
    }
    case TJson::Null:
    case TJson::Bool:
    case TJson::Number:
    case TJson::String:
      THROWER(Config::TInvalidValue) << "Unable to merge addin into target. Can't merge into type "
                                      << target.GetKind();
  }
}

void DeltaMergeJobs(TJobConfig &target, TJobConfig &&addin) {
  for(auto &item : addin) {
    auto iter = target.find(item.first);

    if(iter == target.end()) {
      // Not in lhs yet, just copy across
      target.emplace(std::move(item.first), std::move(item.second));
    } else {
      // Merge the lhs with the rhs.
      AppendMerge(iter->second, move(item.second));
    }
  }
}

// TODO(cmaloney): Create a wrapper which can be used for these to introduce
// a context of 'while reading this particular config file'.

bool LastNotSlash(const string &s) { return s.back() != '/'; }

std::string ReadCacheDir(const TJson &json, const TCoreDirs &core_dirs) {
  const TJson *output_location_json = json.TryAddress({"cache_directory"});
  if(output_location_json) {
    ThrowIfWrongKind(TJson::String, *output_location_json,
                     " trying to read cache_directory from user config");
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

TJobConfig ReadJobConfig(const TJson &json, const std::string &path) {
  TJobConfig result;
  const TJson *job_config = json.TryAddress({"job_config"});

  if(!job_config) {
    return result;
  }

  ThrowIfWrongKind(TJson::Object, *job_config, (" trying to read job_config from " + path).c_str());

  // TODO(cmaloney): This is very unnecessarily copy happy.
  result = job_config->GetObject();

  return result;
}

TMixinConfig TMixinConfig::Load(const std::string &name, const TCoreDirs &core_dirs) {
  TMixinConfig conf;

  auto load_config = [&](const std::string &dir) {
    auto path = dir + "/mixin.bit/" + name + ".json";
    if(!ExistsPath(path.c_str())) {
      return false;
    }

    TJson json = TJson::Read(path);

    TMixinConfig config;
    config.Mixins = ExtractOptional<unordered_set<string>>(json, {"mixins"});
    config.Targets = ExtractOptional<unordered_set<string>>(json, {"targets"});
    config.JobConfig = ReadJobConfig(json, path);

    return true;
  };

  // Use short circuiting so as long as we load one (and we load them in
  // priority order), we return success, otherwise we throw an exception.
  if(load_config(core_dirs.Project) || load_config(core_dirs.User) ||
     load_config(core_dirs.System)) {
    return conf;
  }

  THROWER(Config::TInvalidValue) << "Unable to load mixin: " << name;
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
      if(mixin.compare(0, 4, "bit.")) {
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
    DeltaMergeJobs(config.JobConfig, std::move(mixin_conf.JobConfig));
  }

  return config;
}

TConfig LoadProjectConfig(const string &project_dir) {
  TConfig config;
  const string config_filename = project_dir + "/bit.json";

  TJson json = TJson::Read(config_filename);

  // Load the individual configuration options.
  config.Mixins = ExtractOptional<unordered_set<string>>(json, {"mixins"});
  config.Targets = ExtractOptional<unordered_set<string>>(json, {"targets"});
  config.Jobs = ExtractOptional<unordered_set<string>>(json, {"jobs"});
  config.JobConfig = ReadJobConfig(json, config_filename);

  return config;
}

TConfig Bit::TConfig::Load(const TCoreDirs &core_dirs) {
  assert(LastNotSlash(core_dirs.Project));
  assert(LastNotSlash(core_dirs.User));
  assert(LastNotSlash(core_dirs.System));

  // Load the project base config.
  TConfig config = LoadProjectConfig(core_dirs.Project);

  // Add the user config.
  auto user_json = TJson::TryRead(core_dirs.User + "/.config/bit.json");
  config.CacheDirectory = ReadCacheDir(user_json, core_dirs);

  // TODO(cmaloney): Provide a way for platforms to add / edit / set default flags.
  // TODO(cmaloney): Add distro / OS specific mixins for projects (ex. OSX -> bit.distro.osx mixin)

  // Load all the mixins, apply their config. Return the final config.
  return LoadMixins(move(config), core_dirs);
}
