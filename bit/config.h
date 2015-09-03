/* Loads config, stores aggregated config information

TODO(cmaloney): Prune all this down to match reality

~ == project root

base, project "meta" sets (including default) loaded from ~/bit.json. Contains list of mixins, and optionally flags inside itself directly.

Mixins are loaded from ~/{name}.mixin.bit.json or ~/mixin.bit/{name}.json

Mixins may ask for additional mixins.

Local config options are picked up from a "local.bit.json". If there is a "configure.bit" file will try to execute it (Will error if not executable).

Mixins are purely additive.

mixins.bit/{name}.json

Purely additive. Mixins just add flags. Each individual mixin can have order, but no order is guaranteed between mixins. Things that need "link" order to be seperate from "flag" order should do that themselves

exec(config.bit) -> local.mixins


project, project files, mixnis
  - job_options/flags interpreted by jobs (JSON, lazily processed)

project, project mixins only:
  - Enable / disable specific jobs (compile_cc, etc)
  - Targets to build
  - Enable / disable various auto-task finding / enabling builds (Ex: Tests, perf tests, etc)

user-only (UI Options)
  - Output file location

user, project, or system:
  - Define mixins
  - Define mixin sets


Mixins cannot fight with eachother in delta config specification.

Load all of them, their stuff. Apply all additions. Then apply all removals.
  NOTES for removals:
    - Removing from vector is individual elements, not sequences. No error if not set.
    - Removing from map is key removal.
    - To delete a key, do "key=" null?


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
    -- bit mixin config dir (Default none, only specifiable in files)
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
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <base/json.h>

#include <base/exception.h>

namespace Bit {

using TJobConfig = std::map<std::string, Base::TJson>;

struct TCoreDirs {
  std::string Project;
  std::string User;
  std::string System;
};


// TODO(cmaloney): Add an API so that when no mixins are specified by a user a
// project (and only a project) can provide a "default" mixin / set of mixins to
// use
struct TMixinConfig {
  TJobConfig JobConfig;
  std::unordered_set<std::string> Mixins;
  std::unordered_set<std::string> Targets;

  static TMixinConfig Load(const std::string &name, const TCoreDirs &core_dirs);
};

struct TConfig {
  std::unordered_set<std::string> Targets;
  std::unordered_set<std::string> Mixins;  // Needed for per-file config loading
  TJobConfig JobConfig;
  std::unordered_set<std::string> Jobs;
  std::string CacheDirectory;

  // If optional is true, then if no mixin with the given name exists, don't error.
  // Fast-exits if the same mixin is added twice.
  // If system is true, then the mixin being added comes from bit itself, and
  // is allowed to / must start with `bit.`
  void AddMixin(const std::string &name, const TCoreDirs &core_dirs, bool optional=false, bool system=false);

  // Loads config from user, system, and project, enforcing basic rules, expanding
  // and resolving mixins. Doesn't load per-file config.
  static TConfig Load(const TCoreDirs &core_dirs);
};

namespace Config {

EXCEPTION(TNoSuchMixin, std::runtime_error, nullptr)

} // Config

} // Bit
