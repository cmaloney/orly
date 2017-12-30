#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include <base/as_str.h>
#include <base/not_implemented.h>
#include <bit/config.h>
#include <bit/environment.h>
#include <bit/naming.h>
#include <bit/options.h>
#include <bit/produce.h>
#include <cmd/args.h>
#include <cmd/main.h>
#include <cmd/parse.h>
#include <util/error.h>
#include <util/path.h>

using namespace Base;
using namespace Bit;
using namespace std;
using namespace Util;

// TODO(cmaloney): Allow users to provide via a 'configure' type mechanism.
static const char *SystemBitDir = "/usr/lib/bit";

// TODO(cmaloney): This should be a library function.
TTree GetHomeDirectory() {
  char *home_dir = getenv("HOME");
  if (home_dir) {
    return TTree(home_dir);
  } else {
    // TODO(cmaloney): Implement a sane fallback if HOME isn't set.
    NOT_IMPLEMENTED();
  }
}

int Main(int argc, char *argv[]) {
  Cmd::TArgs<TOptions> args{
      Cmd::Optional({"mixin", "m"}, &TOptions::Mixins,
                    "Configuration snippets to add on top of the base project configuration"),
      // TODO(cmaloney): Worker count is really coarse grained for what people want.
      // Ninja's pool options are more what is wanted / needed here.
      Cmd::Optional("max-parallel", &TOptions::WorkerCount,
                    "Max number of commands/jobs to run at once"),
      Cmd::Required(&TOptions::Targets, "targets", "List of files to try to produce"),
      // TODO(cmaloney): print-cmd is really handy but children can run arbitrary code...
      Cmd::Optional("print-cmd", &TOptions::PrintCmd, "Print the command line of all Exec calls")};
  TOptions options = Cmd::Parse(args, argc, argv);

  if (options.PrintCmd) {
    Subprocess::SetEchoCommands(true);
  }


  // Stash the current directory since we change our working directory to always
  // be the root of the project, but we need to still resolve relative paths
  // from the invocation location.
  auto cwd = GetCwd();
  TTree cwd_tree(cwd);

  // Find the root of the project from the current directory.
  // TODO(cmaloney): Add config like GIT_DIR, GIT_WORK_TREE so that bit doesn't
  // have to be run only from folders in the project.
  // TODO(cmaloney): CURPOS. src is definitely coming out wrong.
  // TODO(cmaloney): Allow subprojects to configure themselves pseudo-independently
  // of base projects to allow a llvm type project structure.
  TTree src = TTree::Find(cwd, "bit.json");

  TCoreDirs core_dirs{src.Path, GetHomeDirectory().Path, TTree(SystemBitDir).Path};

  // Load the config
  auto config = TConfig::Load(core_dirs);

  // Load user-given mixins. If none given, try loading `default` mixin.
  if (!options.Mixins.empty()) {
    for (const auto &mixin : options.Mixins) {
      config.AddMixin(mixin, core_dirs);
    }
  } else {
    config.AddMixin("default", core_dirs, true);
  }

  // Construct the build environment.
  // Load all job factories with defaults loaded into them.
  TEnvironment environment(config, src.Path);

  // Use the default targets if no user-given ones were given.
  vector<string> *targets = nullptr;
  if (!options.Targets.empty()) {
    targets = &options.Targets;
  } else {
    targets = &config.Targets;
  }
  assert(targets);

  // Produce the requested targets.
  DoProduce(options.WorkerCount, environment, *targets, options.Targets.empty());

  return 0;
}
