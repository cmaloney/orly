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
    THROWER(TInvalidValue)
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
      THROWER(TInvalidValue) << "Unable to merge addin into target. Can't merge into type "
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

bool IsLastSlash(const string &s) { return s.back() == '/'; }

std::string ReadCacheDir(const TJson &json, const TCoreDirs &core_dirs) {
  const TJson *output_location_json = json.TryAddress({"cache_directory"});
  if(output_location_json) {
    ThrowIfWrongKind(TJson::String, *output_location_json,
                     " trying to read cache_directory from user config");
    // TODO(cmaloney): string switch
    const std::string &str = output_location_json->GetString();
    if(str == "cache") {
      return core_dirs.User + ".cache/bit";
    } else if(str == "in_project") {
      return core_dirs.Project + ".bit/cache";
    } else if(str == "outside_project") {
      // TODO(cmaloney): Normalize the path
      return core_dirs.Project + "../.bit/cache";
    } else if(str == "tmp") {
      // TODO(cmaloney): obey TMP / TMPDIR / etc. See python's mkstemp()
      return "/tmp/bit_cache";
    } else {
      NOT_IMPLEMENTED();
    }
  } else {
    return core_dirs.Project + ".bit/cache";
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
    auto path = dir + "mixin.bit/" + name + ".json";
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

  THROWER(Config::TNoSuchMixin) << "Unable to load mixin: " << name;
}

// TODO(cmaloney): Mixins should be case insensitive (lower snake case).
void TConfig::AddMixin(const std::string &name, const TCoreDirs &core_dirs, bool is_optional, bool is_system) {
  // Only system can add `bit.` named mixins.
  if (name.compare(0, 4, "bit.")) {
    if (!is_system) {
      THROWER(TInvalidValue) << "Only bit can add mixins starting with 'bit.'.";
    }
  } else if (is_system) {
    THROWER(TInvalidValue) << "System-added mixins must begin with 'bit.'";
  }

  // Fast exit if already loaded.
  if (Contains(Mixins, name)) {
    return;
  }

  // Add the mixin to the loaded set.
  Mixins.insert(name);

  // Load the mixin, recursively load any mixins it depends upon.
  // NOTE: Mixins which are depended upon are always required because if it says
  // it needs it, it needs it. Optional only applies to finding the first one.
  try {
    TMixinConfig config = TMixinConfig::Load(name, core_dirs);

    // Merge the mixin into the config
    Targets.insert(config.Targets.begin(), config.Targets.end());
    DeltaMergeJobs(JobConfig, std::move(config.JobConfig));
  } catch (const TNoSuchMixin &) {
    // If it was optional, no need to worry that it wasn't found.
    // TODO(cmaloney): Exceptions in regular code flows is bad practice.
    if (is_optional) {
      return;
    }
    throw;
  }
}

TConfig LoadProjectConfig(const string &project_dir) {
  TConfig config;
  const string config_filename = project_dir + "bit.json";

  TJson json = TJson::Read(config_filename);

  // Load the individual configuration options.
  config.Mixins = ExtractOptional<unordered_set<string>>(json, {"mixins"});
  config.Targets = ExtractOptional<unordered_set<string>>(json, {"targets"});
  config.Jobs = ExtractOptional<unordered_set<string>>(json, {"jobs"});
  config.JobConfig = ReadJobConfig(json, config_filename);

  return config;
}

TConfig Bit::TConfig::Load(const TCoreDirs &core_dirs) {
  assert(IsLastSlash(core_dirs.Project));
  assert(IsLastSlash(core_dirs.User));
  assert(IsLastSlash(core_dirs.System));

  // Load the project base config.
  TConfig config = LoadProjectConfig(core_dirs.Project);

  // Add the user config.
  auto user_json = TJson::TryRead(core_dirs.User + ".config/bit.json");
  config.CacheDirectory = ReadCacheDir(user_json, core_dirs);

  // TODO(cmaloney): Provide a way for platforms to add / edit / set default flags.
  // TODO(cmaloney): Add distro / OS specific mixins for projects (ex. OSX -> bit.distro.osx mixin)

  // Load all the mixins, apply their config. Return the final config.
  // Copy config.Mixins since AddMixin mutates it.
  const unordered_set<string> base_mixins = config.Mixins;
  for(const auto &mixin: base_mixins) {
    config.AddMixin(mixin, core_dirs, false, false);
  }

  return config;
}
