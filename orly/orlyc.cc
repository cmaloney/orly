/* <orly/orly.cc>

   The controller for the orly compiler. Runs all the phases, exits on first phase failure.

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

#include <ostream>

#include <base/cmd.h>
#include <orly/compiler.h>
#include <orly/error.h>
#include <orly/spa/honcho.h>
#include <orly/spa/service.h>
#include <orly/type.h>
#include <util/path.h>

using namespace std;
using namespace Orly;
using namespace Orly::Spa; // Sort of ug. Would be nice to remove. Not strictly necessary for running tests...

class TCompilerConfig : public Base::TCmd {
  public:
  TCompilerConfig(int argc, char **argv)
      : DebugOutput(false),
        InfoReport(false),
        MachineForm(false),
        OutputDir(Util::GetCwd()),
        SemanticOnly(false),
        SkipTests(false),
        VerboseTests(false) {

    Parse(argc, argv, TMeta());
  }

  private:

  class TMeta : public TCmd::TMeta {
    public:
    TMeta() : TCmd::TMeta("Orly Compiler v0.5") {
      Param(&TCompilerConfig::DebugOutput, "debug_output", Optional, "debug\0d\0", "Compile the orly package in debug mode.");
      Param(&TCompilerConfig::MachineForm, "machine_form", Optional, "machine-form\0m\0", "Print out machine readable progress.");
      Param(&TCompilerConfig::OutputDir, "output_directory", Optional, "output\0o\0", "The directory to write output to.");
      Param(&TCompilerConfig::SemanticOnly, "semantic_only", Optional, "semantic-only\0", "Don't actually produce output, just syntactically and semantically validate the program.");
      Param(&TCompilerConfig::SkipTests, "skip_tests", Optional, "skip-tests\0", "Don't run tests after compiling.");
      Param(&TCompilerConfig::VerboseTests, "verbose_tests", Optional, "v\0",
          "Run tests in 'verbose' mode, printing out the result of every test, rather than just failing tests.");
      Param(&TCompilerConfig::Source, "source", Required, "The Orly source file to compile.");
    }

    private:
    virtual void WriteAfterDesc(std::ostream &strm) const {
      assert(this);
      assert(&strm);
      strm << "Build: Unknown" << endl // TODO: Version from SCM.
           << endl
           << "Copyright OrlyAtomics, Inc." << endl
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

  //TODO: This 'make absolute' is something we do fairly commonly. (At least here and <jhm/jhm>). Should canonicalize.
  string src = cmd.Source;
  if(src.front() != '/') {
    src = Util::GetCwd() + '/' + src;
  }

  //TODO: Practically speaking for each src file given, esp. if we have more than one
  // We need to find the root of it's repository (__orly__) file. And use that
  int result = EXIT_FAILURE;
  try {
    const auto output = Compiler::Compile(
                            Base::TPath(src),
                            cmd.OutputDir,
                            cmd.DebugOutput,
                            cmd.MachineForm,
                            cmd.SemanticOnly);
    if(!cmd.SkipTests && !cmd.SemanticOnly) {
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
    //TODO: It would be nice to put the orly code location at the end, and only in debug mode.
    cerr << src_error.GetPosRange() << ' ' << src_error.what() << endl;
  } catch (const exception &ex) {
    cerr << "error: " << ex.what() << endl;
  }

  return result;
}

/* TODO: Main should be injected */
int main(int argc, char **argv) {
  TCompilerConfig config(argc, argv);

  return CompileCode(config);
}
