Configuration
-----------------------------------

All options can come from the command line, user, system, or project. Command line always wins out.

Projects contain a base config `core.jhm` which lists what to build, the general shape of the project, and any special generally-needed project config.

Config files may never be generated then loaded by the build system. If such behavior seems like it is necessary, probably the tool should be augmenting the files it writes with data which tools down the line will process.

# Loading

Config is loaded from many different locations. Some locations are limited in their ability.

*add*: Can only specify non-conflicting config which is simply merged to the existing config
*modify*: Can add, remove, replace and specify conditionals.

Add-only config is specified in '.json' files. Modify config is specified in '.delta.json'. There is a special syntax / magic keys for the delta config.

## Referencing configuration
TODO: `@filename.json` followed by name?


# Layers

1. Tools listed in core.jhm (add)
1. Mixins listed in core.jhm (add)
1. core.jhm options (add, modify)
1. Per-file options (add, modify)
1. Command-line mixins (add)
1. Command-line tools (add)
1. User-provided "environment" options file (add, modify)

# Overriding behavior

Mixins, tools are both defined by simple json files. Mixins are defined by `bit`/builtin, the Distro/OS, the Project, and the User. Mixins can only grab other mixins / import from their same level or further down the stack (distro mixins can only import other distro mixins and OS mixins). Simply making a mixin with the same name further up the stack doesn't override / change behavior. This may seem annoying, but allowing these to be changed that earlier results in a _lot_ of false build system bugs, because people do custom things and forget. The user may at the command line specify mixin addons or deltas to apply to individual mixins. The user may also specify a file containing multiple mixin deltas or a folder of mixin deltas to load.

## Changing the set of mixins
Mixins can't be modified/overriden. But the set of mixins to enable/disable can be set by either the project or user. The project can specify 'use this set of jobs/mixins only', 'add this to the defaults', or 'Remove these, add those'

## Layers

1. Builtin (/usr/share/bit/)
1. System (/usr/share/bit/distro)
1. Project (bit/)
1. User (~/.config/bit)

Configuration Settings
----------------------------------------
# Targets
List of the different file names to produce (And eventually install) from the source code.

# Features
Allows projects to specify features which can be enabled / disabled in the software. Roughly analogous to "--enable-foo" at configure. Each feature can specify all the same things that can be specified in the top level project configuration.

# Options
Specify flags for specific tools being used. For instance, add flags to the c++ compiler.

# Dependencies
Set of dependencies of the project which must be grabbed. Typically these are things like external git repository + tag which is depended upon. The dependency will be fetched and made available to the project at the location specified.

# Mixins
Collections of common options passed to compilation. For example, enabling C++11, enabling debug mode, choosing a compiler.

Project mixins override user, system ones by the same name by default. User may manually change this behavior. Mixins are purely additive in behavior, they cannot remove flags at this point. If a good case for removing (Or specify if x + y then z) comes up, can work on making the system more extensible.

# Tools
Change what tools are used for different tasks. For example, change between C++ compiler implementations (clang++, g++, icc, msvc).

## Tests
The tests tool automatically finds tests and causes them to be built. Adds options to BIT to enable running tests with builds if wanted.

## Install
Equivalent of `make install`. Choose how things are installed, wildcard patterns to build+install, prefix to use, among others.

# Jobs
Specify the set of transformations BIT knows how to use to build the software. Each job may specify options to add for itself in the case of certain mixins being present. For example, with the C++ job, and `debug` mixin activated, `-g` should be specified on the command line.


Mixin, Job, Tool configuration
------------------------------------------------

`.mixin.json`, `.tool.json`, `.job.json`


Special Config locations
--------------------------------

User Interaction options `~/.config/bit/user.json`
  - Where to put .bit output (.bit inside project, out at same level as project, in fixed directory)

System includes -> link arguments:

pthread.h -> `-pthread`
timers to -> `-rt`

```
/usr/lib/bit/distro/
  dep_map.json
  dep_map/
    {foo.json}
```

NOTE: If two files in dep_map have the same header /path specified then error.

# TODO / Rework

Config layout thoughts:
```
/usr/share/bit

/jobs/
  compile_c_family.so

/mixins/
  c++11.json
  debug.json
  release.json
  /jobs/
    {name}.json
    {name}/
      mixins/
        debug.json
        release.json
  /tools/
    {name}.json
    {name}/
      tool.json
      mixins/
        debug.json
        release

```
