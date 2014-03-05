/* <starsha/starsha.cc>

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <cstdlib>
#include <cstring>
#include <exception>
#include <fnmatch.h>
#include <iostream>
#include <limits.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_set>
#include <vector>


#include <base/cmd.h>
#include <base/os_error.h>
#include <starsha/corpus.h>
#include <starsha/exe_file.h>
#include <starsha/runner.h>
#include <starsha/string_builder.h>

using namespace std;
using namespace Base;
using namespace Starsha;

static const char *TestExt = ".test";
static const size_t TestExtLen = strlen(TestExt);

//Used by runner.cc as an extern
bool PrintCmds = false;

void GetAbsPath(char *abs_path, const char *root, const char *arg) {
  assert(abs_path);
  assert(root);
  assert(arg);
  strcpy(abs_path, root);
  strcat(abs_path, "/");
  char *abs_path_end = abs_path + strlen(abs_path);
  const char
      *arg_csr = arg,
      *arg_end = arg + strlen(arg);
  while (arg_csr < arg_end) {
    if (strncmp(arg_csr, "./", 2) == 0) {
      arg_csr += 2;
      continue;
    }
    if (strncmp(arg_csr, "../", 3) == 0) {
      abs_path_end -= 2;
      for (;;) {
        if (abs_path_end < abs_path + 2) {
          THROW << '"' << arg << "\" navigates beyond the root of the file system; nice shootin', Tex";
        }
        if (*abs_path_end == '/') {
          ++abs_path_end;
          break;
        }
        --abs_path_end;
      }
      arg_csr += 3;
      continue;
    }
    break;
  }
  if (arg_csr >= arg_end || strcmp(arg_csr, ".") == 0 || strcmp(arg_csr, "..") == 0){
    THROW << '"' << arg << "\" seems to contain nothing but navigation";
  }
  strcpy(abs_path_end, arg_csr);
}

void GetTargets(const vector<string> &Targets, const TCorpus &corpus, vector<TCorpus::TFile *> &targets) {
  assert(&corpus);
  assert(&targets);
  char abs_path[PATH_MAX];
  for(auto t: Targets) {
    const char *rel_path;
    if (t[0] == '/') {
      rel_path = corpus.TryGetRelPath(t.c_str() + 1);
    } else {
      GetAbsPath(abs_path, corpus.GetCwd().c_str(), t.c_str());
      rel_path = corpus.TryGetRelPath(abs_path);
    }
    if (!rel_path) {
      THROW << '"' << t << "\" is not part of the corpus";
    }
    targets.push_back(corpus.GetFile(rel_path));
  }
}

void InspectFile(TCorpus::TFile *file) {
  cout << (file->GetIsSrc() ? "$SRC/" : "$OUT/") << file->GetRelPath();
  auto producer = file->TryGetProducer();
  if (producer) {
    cout << ", producer = " << producer->GetRelPath();
  }
  const TOpt<time_t> &mod_time = file->GetModTime();
  if (mod_time) {
    cout << ", mod_time = " << ctime(&*mod_time);
  } else {
    cout << ", (doesn't exist)" << endl;
  }
}

class TStarsha
    : public TCmd {
  public:

  TStarsha(int argc, char *argv[])
      : Config("debug"), Describe(false), Inspect(false),  KeepGoing(false), PrintCmds(false), RunTest(false),
        Verbose(false), WorkerCount(std::thread::hardware_concurrency()) {
    Parse(argc, argv, TMeta());
  }

  int Run() {
    ::PrintCmds = PrintCmds;
    int result;
    try {
      TCorpus corpus(Config.c_str(), ConfigMixin.size() ? ConfigMixin.c_str() : nullptr, WorkerCount);
      vector<TCorpus::TFile *> targets;
      if (All.size() > 0) {
        const char *pattern = All.c_str();
        const char *exclude_pattern = Exclude.size() > 0 ? Exclude.c_str() : 0;
        corpus.ForEachFile([&targets, pattern, exclude_pattern](TCorpus::TFile *file) {
          switch (fnmatch(pattern, file->GetRelPath().c_str(), 0)) {
            case 0: {
              if(exclude_pattern) {
                switch (fnmatch(exclude_pattern, file->GetRelPath().c_str(), 0)) {
                  case 0: {
                    break;
                  }
                  case FNM_NOMATCH: {
                    targets.push_back(file);
                    break;
                  }
                  default: {
                    throw TOsError(HERE);
                  }
                }
              } else {
                targets.push_back(file);
              }
              break;
            }
            case FNM_NOMATCH: {
              break;
            }
            default: {
              throw TOsError(HERE);
            }
          }
          return true;
        });
      }
      GetTargets(Targets, corpus, targets);
      bool is_ok = true;
      if (Describe) {
        for (auto target: targets) {
          cout << target->GetRelPath() << ':' << endl;
          size_t batch_number = 1;
          corpus.DescribeBuild(target, [&batch_number](const vector<TCorpus::TFile *> &files) {
            cout << "  batch #" << batch_number << ':' << endl;
            for (auto file: files) {
              cout << "    " << file->GetRelPath();
              auto producer = file->TryGetProducer();
              if (producer) {
                cout << " (" << file->TryGetProducer()->GetRelPath() << ')';
              }
              cout << endl;
            }
            ++batch_number;
            return true;
          });
        }
      } else if (Inspect) {
        for (auto target: targets) {
          InspectFile(target);
        }
      } else {
        corpus.Build(targets, [this, &is_ok](TCorpus::TFile *file, const vector<string> &msgs) {
          is_ok = false;
          cout << file->GetRelPath() << ':' << endl;
          for (const string &msg: msgs) {
            cout << msg << endl;
          }
          return static_cast<bool>(KeepGoing);
        });
        if (is_ok && RunTest) {
          for (auto target: targets) {
            if (target->GetKind() == &TExeFile::Kind) {
              const string &rel_path = target->GetRelPath();
              if (rel_path.compare(rel_path.size() - TestExtLen, TestExtLen, TestExt) == 0) {
                string cmd_line;
                TStringBuilder(cmd_line) << target->GetAbsPath() << (Verbose ? " -v" : "");
                TRunner runner(cmd_line);
                int status = runner.Wait();
                if (status || Verbose) {
                  cout << rel_path << ':' << endl;
                  runner.ForEachLine([](bool, const char *line) {
                    cout << line << endl;
                    return true;
                  });
                }
                is_ok = is_ok && (status == 0);
                if (!is_ok && !KeepGoing) {
                  break;
                }
              }  // if (target's rel_path ends with test ext...)
            }  // if (target is exe...)
          }  // for (targets...)
        }  // if (we're going to try to run tests...)
      }
      corpus.WriteNote();
      result = is_ok ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (const exception &ex) {
      cerr << "exception: " << ex.what() << endl;
      result = EXIT_FAILURE;
    }
    return result;
  }

  private:

  class TMeta : public TCmd::TMeta {
    public:
    TMeta() : TCmd::TMeta("Starsha") {
      Param(&TStarsha::All, "all", Optional, "all\0", "Build all items matching the pattern");
      Param(&TStarsha::Exclude, "exclude", Optional, "exclude\0", "Exclude items matching the pattern from 'all'");
      Param(&TStarsha::ConfigMixin, "config_mixin", Optional, "mixin\0m\0", "Add a configuration mixin to the base configuration");
      Param(&TStarsha::Config, "config", Optional, "config\0c\0", "Run starsha in a particular configuration");
      Param(&TStarsha::Describe, "describe", Optional, "describe\0desc\0", "Describe something");
      Param(&TStarsha::KeepGoing, "keep_going", Optional, "keep-going\0", "Go as long as possible, even if something fails");
      Param(&TStarsha::Inspect, "inspect", Optional, "inspect\0", "Inspect something");
      Param(&TStarsha::RunTest, "run_tests", Optional, "test\0t\0", "Run all found tests");
      Param(&TStarsha::Verbose, "verbose", Optional, "verbose\0v\0", "Run in verbose mode");
      Param(&TStarsha::WorkerCount, "worker_count", Optional, "worker-count\0", "Change the number of worker threads starsha users");
      Param(&TStarsha::PrintCmds, "print_cmds", Optional, "print-cmds\0", "print commands as they run");
      Param(&TStarsha::Targets, "targets", Optional, "Targets to try to build");
      /* TODO: Cleaner way to push this to global scope so runner.cc can get to it, currently assign in Run func.*/
    }

    virtual void WriteAfterDesc(std::ostream &strm) const {
      assert(this);
      assert(&strm);
      strm << "Copyright Stig LLC" << endl
           << "Licensed under the Apache License, Version 2.0" << endl;
    }
  };

  virtual TCmd::TMeta *NewMeta() const {
    assert(this);
    return new TMeta();
  }

  std::string All;
  std::string Config;
  std::string ConfigMixin;
  bool Describe;
  std::string Exclude;
  bool Inspect;
  bool KeepGoing;
  bool PrintCmds;
  bool RunTest;
  vector<std::string> Targets;
  bool Verbose;
  unsigned int WorkerCount;
};

int main(int argc, char *argv[]) {
  return TStarsha(argc, argv).Run();
}
