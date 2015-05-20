bit - BuildIT
--------------

Software is built about the same everywhere. Projects are layed out more or less the same. We write massive amounts of glue code to try to build what is almost always the same, enable using common tools. Lets stop doing that.

# Getting the build system out of the way

1. If an interface is included / used, that means it should be linked against, whether as a library or statically.
1. If two files share the same base name, they are related. foo.test tests foo.cc which implements the interface of foo.h
1. Know how to setup external project dependencies, including local patching of dependencies, while still being able to use system ones if present.
1. Make it a simple compile option to apply any of the great modern tools to a given project
1. Don't pretend to know how to build software we don't. Just use the native build system.
1. Don't litter the source code checkout with intermediate build files

# How to get there

1. Project configured via `core.bit`
  - Describes project produced files
  - Base compile config (include directory vs. not)
  - Per-tool arguments to use (CFLAGS, CXXFLAGS, LDFLAGS, etc)
  - Projects may override / add their own by specifying the "flag sets" to use (add/remove) for each
    of the core sets.
  - Dependencies to fetch
  ? one repo -> many "independent" packages
    - libmesos, libprocess, stout
    - llvm, clang, clang-tools, clang-tools-extra, libcxx, etc
  - Magic "include" directories for publishing.
  - Feature flags and how to enable/disable them
    - "enable foobar -> add C++ define"
  -
2. Common "flag sets", tools ready to run
  - debug, release sitting there for you.
  - Can mix and match option sets (Address Sanitizer + Debug)
  - All build results stored ccache style?
  ? Blacklist known problematic / incompatible option sets?
3. Tools wrap/watch build, can add/modify build flags
  - Test running / result collection harness
  - ASan, TSan, MSan
  - Clang-Analyzer / Static Analysis
  - Valgrind, Cachegrind, Callgrind
  ? AFL-Fuzz
  - Code Coverage
  ? CCache
  - DistCC via Mesos
  - Per-distro native packaging?
  - GDB / LLDB
  - Outputting clang compilation database
  - Each language / bit of tooling in use should augment or explicitly state it doesn't support some particular kind of tooling.
4. We build "virtualenv" type environments per-project
  - All deps are "make installed" to that folder
  - Deps may specify "I'm just code, unpack"
  - Knows how to run an arbitrary script pkgpanda style to make the dep
    - Deps may have transitive deps, both "buil" deps and "runtime" deps.
    - Try to isolate end-user code from runtime deps
  - The virtualenv can be entirely isolated / self-hosted from pkgpanda packages
    so you have an "isolated" known-good dev environment to get anywhere.
  - Pkgpanda packages can be pre-built, distributed in binary form for specific
    platforms (OSX, Linux Isolated, Win64 primarily)
  - Special "guess" mechanism which knows how to detect basic build types (python, haskell, ruby, autotoos/configure, go, CMake, etc)
5. Users / packagers can override all settings via `~/.config/bit` and `{}.user.bit`
  - Add to section, remove from section, subtitute section of config
  - Specify how to unbundle / use deps on base system
  - Disable -Werror, etc
  ? Add patches to dependencies
6. Dependencies
  - Knows static, dynamic linking
  - Mapping 1+ include -> a library
  ? Reading symbols out of a lib to figure out when we get link errors what was probably missing
  - Can forcibly specify certain libs to satisfy certain deps (ex: c++ standard library -> libc++, libstdc++)
  - Platform / distro can describe how to get the "Standard" version of a lib for that platform
    Ex: "libc++ is standard, available at location /foo/bar/baz"
    - Can either be url (Magic deps, support for adding providers. Ex: github.com/moodycamel/concurrentqueue)
    - Or tarball to download / extract (libcurl)
    - Or git repo to clone/
    - And apply locally commited patch files
7. Extensible for project-specific needs
  - Adding in new types of files which can be compiled, analyzed, etc.
  - Adding in new ways of finding,
  - Use libclang directly instead of clang through command line
  - Add in custom tools for compiling new jobs (.td via tablegen)
  - Add in new methods of downloading dependencies (Torrent from internal hosts?)
  - Tight Editor integration (Autocomplete build options)
