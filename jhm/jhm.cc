/* <jhm/jhm.cc>

   JHM build system.

   TODO:
    - Make it so that we minimize the amount of stuff which builds up in the foreground thread / we
   keep the worker
      queue busier all the time
    - Remove Stdout/Stderr printing from work_finder.h, so it's more portable
    - Make jobs and files const more of the time.
    - Eliminate the Ready queue. It just slows us down a little / makes unnecessary extra looping
   happen...

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <unistd.h>

#include <iostream>
#include <string>

#include <base/thrower.h>
#include <cmd/args.h>
#include <cmd/main.h>
#include <cmd/parse.h>
#include <jhm/options.h>
#include <jhm/env.h>
#include <jhm/status_line.h>
#include <jhm/test.h>
#include <jhm/work_finder.h>
#include <util/path.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace std::placeholders;
using namespace Util;

/* Converts relative file to absolute path if needed, then has the environment find/make the actual
 * file object. */
TFile *FindFile(const string &cwd, TEnv &env, TWorkFinder &work_finder, const string &name) {
  if(name.size() < 1) {
    THROWER(runtime_error) << "Invalid target name " << quoted(name);
  }

  // If filename starts with '/' then it's an absolute path rooted at the base of the tree
  // Otherwise it's relative to the position where jhm was invoked.
  TFile *file = nullptr;
  if(name[0] == '/') {
    // Starts with a '/', so relative to src / an absolute pathname
    file = env.GetFile(TRelPath(TPath(name)));
  } else {
    // Doesn't start with a '/' so relative to execution location name.
    TPath abs_path(cwd + '/' + name);
    const TTree *tree;
    if(env.GetSrc()->Contains(abs_path)) {
      tree = env.GetSrc();
    } else if(env.GetOut()->Contains(abs_path)) {
      tree = env.GetOut();
    } else {
      THROWER(runtime_error)
          << "Target " << quoted(name)
          << " not relative to 'jhm' execution in either `src` or `out` directory";
    }
    file = env.GetFile(tree->GetRelPath(move(abs_path)));
  }

  if(!file) {
    // TODO: Do we want to report the relative path to the file here rather than the provided name?
    THROWER(runtime_error) << "Error finding target" << quoted(name);
  }

  // If the file isn't buildable as is, try making it an executable (Add an empty extension to the
  // end)
  if(!work_finder.IsBuildable(file)) {
    file = env.GetFile(TRelPath(AddExtension(TPath(file->GetRelPath().Path), {""})));
  }

  return file;
}

// TODO(cmaloney): More of this seems like it should be library functionality rather than hardcoded
// into main

int Main(int argc, char *argv[]) {
  Cmd::TArgs<TOptions> args{
      Cmd::Optional("print-cmd", &TOptions::PrintCmd, "Print commands when they are run"),
      Cmd::Optional(
          {"config", "c"}, &TOptions::Config, "Build the software in the given configuration"),
      Cmd::Optional({"mixin", "m"}, &TOptions::ConfigMixin, "Configuration mixin"),
      Cmd::Optional({"run-tests", "t"}, &TOptions::RunTests, "Run the unit tests"),
      Cmd::Optional("verbose-tests", &TOptions::VerboseTests, "Run the tests in verbose mode"),
      Cmd::Optional(
          "worker-count", &TOptions::WorkerCount, "Max number of commands to run at once"),
      Cmd::Required(&TOptions::Targets, "targets", "List of files to try to produce")};
  TOptions options = Cmd::Parse(args, argc, argv);
  auto cwd = GetCwd();

  // Build up the environment. Find the root, grab the project, user, and system configuration
  TTree cwd_tree(cwd);
  TTree src = TTree::Find(cwd, "core.jhm");
  TEnv env(src, options.Config, options.ConfigMixin);

  // chdir to the src folder so we can always use relative paths. for commands
  /* abs_root */ {
    auto abs_root = AsStr(*env.GetSrc());
    if(!ExistsPath(abs_root.c_str())) {
      THROWER(runtime_error) << "Source directory " << quoted(abs_root) << " does not exist";
    }
    IfLt0(chdir(abs_root.c_str()));
  }

  // Get the files for the targets
  TWorkFinder work_finder(
      // NOTE: Env is guaranteed to always have a timestamp, so derefrencing it here is safe.
      *env.GetConfig().GetTimestamp(),
      bind(&TEnv::GetJobsProducingFile, &env, _1),
      bind(&TEnv::GetFile, &env, _1),
      bind(&TEnv::TryGetFileFromPath, &env, _1));

  // Break the cyclic dependency by registering these back.
  // TODO: Find a cleaner way to do this (Or remove it altogether)
  env.SetFuncs(bind(&TWorkFinder::IsBuildable, &work_finder, _1),
               bind(&TWorkFinder::IsFileDone, &work_finder, _1));

  // TODO: Gather exceptions here, rather than letting first one fly.
  TSet<TFile *> target_files;

  // Either build the explicitly specified targets, or the default targets
  if(!options.Targets.empty()) {
    for(const auto &target : options.Targets) {
      target_files.emplace(FindFile(cwd, env, work_finder, target));
    }
  } else {
    // Add the default targets
    for(const auto &target : env.GetConfig().Read<vector<string>>({"targets"})) {
      target_files.emplace(FindFile(cwd, env, work_finder, '/' + target));
    }

    // Add the tests if we're supposed to by default
    bool build_tests = false;
    env.GetConfig().TryRead({"test", "build_with_default_targets"}, build_tests);
    if(build_tests) {
      auto tests = FindTests(env);

      // If the file is not buildable, skip the test. This prevents us from trying to test things
      // that are
      // untestable.
      // NOTE: If this excludes something that should be tested, we should be able to catch that in
      // test reports.
      // TODO: This should be a std::remove_if...
      /* filter */ {
        TSet<TFile *> filtered_tests;
        for(TFile *test : tests) {
          if(work_finder.IsBuildable(test)) {
            InsertOrFail(filtered_tests, test);
          }
        }
        tests = move(filtered_tests);
      }
      if(build_tests) {
        target_files.insert(tests.begin(), tests.end());
      }
    }
  }

  // Add all target files as needed
  for(TFile *f : target_files) {
    work_finder.AddNeededFile(f);
  }

  TJobRunner runner(options.WorkerCount, options.PrintCmd);
  if(!work_finder.FinishAll(runner)) {
    return 1;
  }

  // Run all the tests if requested
  if(!options.RunTests) {
    return 0;
  }

  // Run every test which should have been built
  // TODO: We really should make it a job to produce a test report, and let the job runner run them
  // in parallel. But
  //      that requires teaching the job runner about resource needs of various kinds of jobs (Ex.
  //      Run only one, 512MB
  //      of ram, etc).
  TPump pump;
  auto RunTest = [&options, &pump](TFile *test) {
    vector<string> cmd{test->GetPath()};
    // NOTE: This is a seperate line because otherwise it breaks in release builds
    if(options.VerboseTests) {
      cmd.push_back("-v");
      cout << "TEST: " << test;
    } else {
      TStatusLine() << "TEST: " << test;
    }
    auto subprocess = TSubprocess::New(pump, cmd);
    auto status = subprocess->Wait();

    if(options.VerboseTests || status) {
      TStatusLine::Cleanup();  // Make sure the TEST: line stays at the top.
      EchoOutput(subprocess->TakeStdOutFromChild());
      EchoOutput(subprocess->TakeStdErrFromChild());
    }

    if(status) {
      cout << "EXITCODE: " << status << '\n';
      return false;
    }
    return true;
  };

  for(TFile *f : target_files) {
    if(f->GetRelPath().Path.EndsWith({"test", ""})) {
      if(!RunTest(f)) {
        return 2;
      }
    }
  }
  TStatusLine::Cleanup();
  return 0;
}
