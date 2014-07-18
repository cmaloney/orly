/* <orly/csv_to_bin/translator_gen.cc>

   Command-line program to be fed an SQL-like source text and
   generates a CSV converter.

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

#include <iostream>

#include <base/as_str.h>
#include <base/fd.h>
#include <base/log.h>
#include <base/path.h>
#include <base/pump.h>
#include <base/source_root.h>
#include <base/subprocess.h>
#include <orly/csv_to_bin/synth.h>
#include <orly/csv_to_bin/write_cc.h>
#include <orly/csv_to_bin/write_orly.h>
#include <orly/code_gen/cpp_printer.h>
#include <util/error.h>
#include <util/io.h>
#include <util/path.h>

using namespace std;
using namespace Orly::CsvToBin;
using namespace Orly::CodeGen;

class TCmd final
    : public Base::TLog::TCmd {
  public:

  TCmd(int argc, char *argv[]) {
    Parse(argc, argv, TMeta());
    if (StarterScript.empty()) {
      StarterScript = Base::TPath(Schema).Name;
    }  // if
  }

  string Schema;
  string StarterScript;

  private:

  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    TMeta()
        : Base::TLog::TCmd::TMeta("CSV to Orly binary converter generator.") {
      Param(
          &TCmd::Schema, "schema", Required,
          "The path to the schema file to read from (SQL).");
      Param(
          &TCmd::StarterScript, "starter-script", Optional, "starter-script\0ss\0",
          "The name of the generated starter Orly script.");
    }

  };  // TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  string cc_outfile = Base::GetSrcRoot() + "orly/csv_to_bin/translate.cc";
  string orly_outfile = cmd.StarterScript + ".orly";
  try {
    ifstream instrm;
    Util::OpenFile(instrm, cmd.Schema);
    ofstream cc_outstrm;
    Util::OpenFile<ofstream>(cc_outstrm, cc_outfile);
    TCppPrinter orly_printer(orly_outfile, "Orly script");
    string sql(istreambuf_iterator<char>{instrm}, istreambuf_iterator<char>{});
    auto table = NewTable(cerr, sql.data());
    if (!table) {
      cerr << "exiting early; no useable table definition" << endl;
      exit(EXIT_FAILURE);
    }
    WriteCc(cc_outstrm, table.get());
    WriteOrly(orly_printer, table.get());
    std::cout << "wrote [" << orly_outfile << "]" << std::endl;
    Base::TPump pump;
    #ifndef NDEBUG
    const char *jhm_cmd = "jhm driver";
    #else
    const char *jhm_cmd = "jhm -c release driver";
    #endif
    std::cout << "running [" << jhm_cmd << "]" << std::endl;
    auto subprocess = Base::TSubprocess::New(pump, jhm_cmd);
    auto status = subprocess->Wait();
    if (status) {
      Base::EchoOutput(subprocess->TakeStdOutFromChild());
      Base::EchoOutput(subprocess->TakeStdErrFromChild());
    }  // if
    Util::Delete(cc_outfile.data());
    auto translator_name = Base::AsStr(cmd.StarterScript, "_translator");
    rename("driver", translator_name.data());
    std::cout << "moved [driver] to [" << translator_name << "]" << std::endl;
  } catch (const exception &ex) {
    cerr << "Error occured: " << ex.what() << endl;
    exit(EXIT_FAILURE);
  }  // try
}

