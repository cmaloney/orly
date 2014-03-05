/* <stig/stig.cc>

   The controller for the stig compiler. Runs all the phases, exits on first phase failure.

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

#include <ostream>

#include <base/cmd.h>
#include <stig/compiler.h>
#include <stig/error.h>
#include <stig/honcho.h>
#include <stig/spa/service.h>
#include <stig/type.h>

using namespace std;
using namespace Stig;
using namespace Stig::Spa; // Sort of ug. Would be nice to remove. Not strictly necessary for running tests...

//For the starsha runner.
bool PrintCmds = false;


class TCompilerConfig : public Base::TCmd {
  public:
  TCompilerConfig(int argc, char **argv)
      : DebugOutput(false), InfoReport(false), MachineForm(false), SemanticOnly(false),
        SkipTests(false), VerboseTests(false) {

    Parse(argc, argv, TMeta());
  }

  private:

  class TMeta : public TCmd::TMeta {
    public:
    TMeta() : TCmd::TMeta("STIG Spa Compiler v0.5") {
      Param(&TCompilerConfig::DebugOutput, "debug_output", Optional, "debug\0d\0", "Compile the stig package in debug mode.");
      Param(&TCompilerConfig::MachineForm, "machine_form", Optional, "machine-form\0m\0", "Print out machine readable progress.");
      Param(&TCompilerConfig::OutputDir, "output_directory", Optional, "output\0o\0", "The directory to write output to.");
      Param(&TCompilerConfig::SemanticOnly, "semantic_only", Optional, "symantic-only\0", "Don't actually produce output, just syntactically and semantically validate the program.");
      Param(&TCompilerConfig::SkipTests, "skip_tests", Optional, "skip-tests\0", "Don't run tests after compiling.");
      Param(&TCompilerConfig::VerboseTests, "verbose_tests", Optional, "v\0",
          "Run tests in 'verbose' mode, printing out the result of every test, rather than just failing tests.");
      //TODO: It would be nice if we could make this "Required"...
      Param(&TCompilerConfig::Source, "source", Required, "The Stig source file to compile.");
    }

    private:
    virtual void WriteAfterDesc(std::ostream &strm) const {
      assert(this);
      assert(&strm);
      strm << "Build: Unknown" << endl // TODO: Version from SCM.
           << endl
           << "Copyright Stig LLC" << endl
           << "Licensed under the Apache License, Version 2.0" << endl;
    }

  };  // TMeta

  virtual TCmd::TMeta *NewMeta() const {
    assert(this);
    return new TMeta();
  }

  public:
  //NOTE: These should really be private w/ accessors but I'm going for quick right now, not pretty.
  bool DebugOutput;
  THoncho Honcho;
  bool InfoReport;
  bool MachineForm;
  std::string OutputDir;
  bool SemanticOnly;
  std::string Source;
  bool SkipTests;
  bool VerboseTests;
};

int CompileCode(const TCompilerConfig &cmd) {
  assert(&cmd);

  bool found_root = false;
  Jhm::TAbsBase root = Jhm::TAbsBase::Find("__stig__", found_root /* out param*/);

  int result = EXIT_FAILURE;
  try {
    const auto output = Compiler::Compile(
                            root.GetAbsPath(cmd.Source),
                            Jhm::TAbsBase(cmd.OutputDir),
                            found_root,
                            cmd.DebugOutput,
                            cmd.MachineForm); //TODO: SemanticOnly);
    if(!cmd.SkipTests) {
      if(cmd.MachineForm) {
        cout<<"MM_NOTICE: Running tests"<<endl;
      }
      TService service;

      /* TODO: This is a little ugly */
      service.SetPackageDir(cmd.OutputDir.size() > 0 ? cmd.OutputDir : "");
      service.GetPackageManager().Install({output});
      result = service.RunTestSuite(output.Name, cmd.VerboseTests) ? EXIT_SUCCESS : EXIT_FAILURE;

      if(cmd.MachineForm) {
        cout<<"MM_NOTICE: Tests done"<<endl;
      }
      service.GetPackageManager().Uninstall({output});
    } else {
      if(cmd.MachineForm) {
        cout<<"MM_NOTICE: Skipped tests"<<endl;
      }
      result = EXIT_SUCCESS;
    }
  } catch (const Compiler::TCompileFailure &ex) {
    /* Do nothing, we already printed out the error message. */
    if (cmd.DebugOutput) {
      cerr << "compile failure: " << ex.what() << endl;
    }
    //TODO: The do nothing is not the prettiest.
  } catch (const TSourceError &src_error) {
    //TODO: It would be nice to put the stig code location at the end, and only in debug mode.
    cerr << src_error.GetPosRange() << ' ' << src_error.what() << endl;
  } catch (const Base::TError &ex) {
    cerr << "general error: " << ex.what() << endl;
  } catch (const exception &ex) {
    cerr << "exception: " << ex.what() << endl;
  }

  return result;
}

/* TODO: Main should be injected */
int main(int argc, char **argv) {
  TCompilerConfig config(argc, argv);

  return CompileCode(config);
}