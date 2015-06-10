/* Loads config, stores aggregated config information

project, project files, mixnis
  - job_options/flags interpreted by jobs (JSON, lazily processed)

project, project mixins only:
  - Enable / disable specific jobs (compile_cc, etc)
  - Targets to build

user-only (UI Options)
  - Output file location

user, project, or system:
  - Define mixins
  - Define mixin sets



1) Load mixins. Start with project mixin file (bit.config.json), move to sytem
(/usr/share/bit/mixins), then finally user-specific if others not found (~/.bit/mixins)

User must explicitly override to change behavior of existing --user-mixin asdfasdfasdf

Mixins are purely additive


2) Load project config file
  - core.bit.json
    -- Specifieds default mixins (c++11, etc)
    -- Specifies list of tools to enable / disable default tools
    -- Defines project-specific mixins
    -- Adds flags per tool / command
    -- jhm mixin config dir (Default none, only specifiable in files)
    EVENTUALLY:
      -- Enable/disable features / config toggles / flags (And can do it in seperate file)
      -- Defines basic configuration toggles (enable/disable)
      -- Defines dependencies, how to fetch them if needed

3) Load user config file for bit
  -- Where to put output directory (.bit/out, folder at same level as project, in ~/.cache/out)
  -- User mixin groups (NOTE: these never override project ones, must be done via --user-mixin or be
non-conflicting)

Eventual features
  - Ability to reference external files, more than one config file.
  - Per-file override config files
*/

#include <string>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace bit {

using TJobConfig = std::unordered_map<std::string, nlohmann::json>;

// TODO(cmaloney): enabled_jobs, job_config shouldn't be in the interface,
// rather just pass back the set of jobs.
struct TConfig {
  std::vector<std::string> DefaultTargets;
  std::vector<std::string> EnabledMixins;  // Needed for per-file config loading
  TJobConfig JobConfig;
  std::vector<std::string> EnabledJobs;
  std::string CacheDirectory;

  // Loads config from user, system, and project, enforcing basic rules, expanding
  // and resolving mixins. Doesn't load per-file config.
  static TConfig Load(std::string project_dir, std::string user_dir, std::string system_dir);
};

struct TFileConfig {
  TJobConfig JobConfig;

  // Loads a per-file config and emits the resulting job config.
  static TFileConfig Load(std::vector<std::string> &EnabledMixins, const std::string &filename);
};


namespace Config {

// Merge two json config files according to the merge rules.
// 'name=' means replace
// 'name+' means append / add
// 'name-' means remove
// 'name' as a dict key just means add / merge subkeys.
nlohmann::json DeltaMerge(nlohmann::json &&base, nlohmann::json &&delta_json);

// Merge two json config files, updating the base
nlohmann::json Append(nlohmann::json &&lhs, nlohmann::json &&to_add);

}

}
