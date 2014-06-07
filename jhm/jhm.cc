/* <jhm/jhm.cc>

   JHM build system.

   TODO:
    - Make it so that we minimize the amount of stuff which builds up in the foreground thread / we keep the worker
      queue busier all the time
    - Remove Stdout/Stderr printing from work_finder.h, so it's more portable
    - Make jobs and files const more of the time.
    - Eliminate the Ready queue. It just slows us down a little / makes unnecessary extra looping happen...

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

#include <base/cmd.h>
#include <base/path_utils.h>
#include <base/thrower.h>
#include <jhm/env.h>
#include <jhm/work_finder.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace std::placeholders;

/* Converts relative file to absolute path if needed, then has the environment find/make the actual file object. */
TFile *FindFile(const string &cwd, TEnv &env, TWorkFinder &work_finder, const string &name) {
  if (name.size() < 1) {
    THROW_ERROR(runtime_error) << "Invalid target name " << quoted(name);
  }

  // If filename starts with '/' then it's an absolute path rooted at the base of the tree
  // Otherwise it's relative to the position where jhm was invoked.
  TFile *file = nullptr;
  if (name[0] == '/') {
    // Starts with a '/', so relative to src / an absolute pathname
    file = env.GetFile(TRelPath(name.substr(1)));
  } else {
    // Doesn't start with a '/' so relative to execution location name.
    string abs_path = cwd + '/' + name;
    string rel_path;
    if(env.GetSrc().Contains(abs_path)) {
      // +2 to remove the forward and trailing '/'
      rel_path = abs_path.substr(env.GetSrc().Get().size()+2);
    } else if (env.GetOut().Contains(abs_path)) {
      rel_path = abs_path.substr(env.GetOut().Get().size()+2);
    } else {
      THROW_ERROR(runtime_error) << "Target " << quoted(name)
                                 << " not relative to 'jhm' execution in either `src` or `out` directory";
    }
    file = env.GetFile(TRelPath(rel_path));
  }


  if (!file) {
    // TODO: Do we want to report the relative path to the file here rather than the provided name?
    THROW_ERROR(runtime_error) << "Error finding target" << quoted(name);
  }

  // If the file isn't buildable as is, try making it an executable (Add an empty extension to the end)
  if (!work_finder.IsBuildable(file)) {
    file = env.GetFile(file->GetPath().GetRelPath().AddExtension({""}));
  }

  return file;
}

class TJhm : public TCmd {
  public:
  TJhm(int argc, char *argv[]) : WorkerCount(thread::hardware_concurrency()) {
    Parse(argc, argv, TMeta());
  }

  int Run() {
    auto cwd = GetCwd();
    // Build up the environment. Find the root, grab the project, user, and system configuration
    bool found_root = false;
    TAbsBase root = TAbsBase::Find(".jhm", found_root);
    if (!found_root) {
      THROW_ERROR(runtime_error) << "Unable to find '.jhm' directory indicating root of workspace";
    }

    // TODO: proj_name should be the folder immediately inside the root that executed inside, so long as it doesn't
    // begin with "out".
    TEnv env(root, "src", Config, ConfigMixin);

    // chdir to the src folder so we can always use relative paths. for commands
    // NOTE: has to come after the 'TAbsBase::Find' as that calls GetCwd, which this by it's nature upsets.
    /* extra */ {

      auto abs_root = "/" + env.GetSrc().Get();
      if (!ExistsPath(abs_root.c_str())) {
        THROW_ERROR(runtime_error) << "Source directory '/" << env.GetSrc().Get() << "' does not exist";
      }
      IfLt0(chdir(abs_root.c_str()));
    }

    // Grab targets. If none on command line, then use default set from config. Mark all as needed.
    // TODO: Uncomment once we have config
    if (Targets.size() == 0) {
      Targets = env.GetConfig().Read<vector<string>>("targets");
    }

    // Get the files for the targets
    TWorkFinder work_finder(WorkerCount, PrintCmd, bind(&TEnv::GetJobsProducingFile, &env, _1));

    // Break the cyclic dependency by registering these back.
    // TODO: Find a cleaner way to do this (Or remove it altogether)
    env.SetFuncs(bind(&TWorkFinder::IsBuildable, &work_finder, _1), bind(&TWorkFinder::IsFileDone, &work_finder, _1));

    // TODO: Gather exceptions here, rather than letting first one fly.
    for (const auto &target : Targets) {
      // Walks down the tree, adding the mappings for all the events we need to handle.
      work_finder.AddNeededFile(FindFile(cwd, env, work_finder, target));
    }

    // TODO: Add a single-line status message
    return work_finder.FinishAll() ? 0 : -1;

    // TODO Verify all target files are built? (Extra safety check)
  }

  private:
  class TMeta : public TCmd::TMeta {
    public:
    TMeta() : TCmd::TMeta("JHM") {
      Param(&TJhm::Config, "config", Optional, "config\0c\0", "Run JHM in a particular configuration");
      Param(&TJhm::ConfigMixin,
            "config_mixin",
            Optional,
            "mixin\0m\0",
            "Add a configuration mixin to the base configuration");
      Param(&TJhm::PrintCmd, "print_cmd", Optional, "print-cmd\0p\0", "Print out the commands JHM runs to do work");
      Param(&TJhm::WorkerCount,
            "worker_count",
            Optional,
            "worker-count\0",
            "Change the number of worker threads JHM uses to run jobs simultaneously");
      Param(&TJhm::Targets, "targets", Optional, "Targets to try to build");
    }

    virtual void WriteAfterDesc(ostream &strm) const {
      assert(this);
      assert(&strm);
      strm << "Copyright OrlyAtomics, Inc.\n"
           << "Licensed under the Apache License, Version 2.0" << endl;
    }
  };

  bool PrintCmd = false;
  string Config = "debug";
  string ConfigMixin;
  uint32_t WorkerCount = 0;
  vector<string> Targets;
};

int main(int argc, char *argv[]) {
  try {
    return TJhm(argc, argv).Run();
  }
  catch (const std::exception &ex) {
    cerr << "EXCEPTION: " << ex.what() << endl;
    return -1;
  }
}