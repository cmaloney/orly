/* <orly/csv_to_bin/generator.cc>

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

#include <base/fd.h>
#include <base/log.h>
#include <base/pump.h>
#include <base/source_root.h>
#include <base/subprocess.h>
#include <orly/csv_to_bin/synth.h>
#include <orly/csv_to_bin/write_cc.h>
#include <util/path.h>

using namespace std;
using namespace Orly::CsvToBin;

class TCmd final
    : public Base::TLog::TCmd {
  public:

  TCmd(int argc, char *argv[]) {
    Parse(argc, argv, TMeta());
  }

  string Schema;

  private:

  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    TMeta()
        : Base::TLog::TCmd::TMeta("CSV to Orly binary converter generator.") {
      Param(
          &TCmd::Schema, "schema", Required, "schema\0s\0",
          "The path to the schema file to read from (SQL).");
    }

  };  // TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  string outfile = Base::GetSrcRoot() + "orly/csv_to_bin/translate.cc";
  try {
    ifstream instrm(cmd.Schema);
    ofstream outstrm(outfile);
    instrm.exceptions(ifstream::failbit);
    outstrm.exceptions(ofstream::failbit);
    string sql(istreambuf_iterator<char>{instrm}, istreambuf_iterator<char>{});
    auto table = NewTable(cerr, sql.data());
    WriteCc(outstrm, table.get());
    Base::TPump pump;
    #ifndef NDEBUG
    const char *jhm_cmd = "jhm driver";
    #else
    const char *jhm_cmd = "jhm -c release driver";
    #endif
    printf("running [%s]\n", jhm_cmd);
    auto subprocess = Base::TSubprocess::New(pump, jhm_cmd);
    auto status = subprocess->Wait();
    if (status) {
      Base::EchoOutput(subprocess->TakeStdOutFromChild());
      Base::EchoOutput(subprocess->TakeStdErrFromChild());
    }  // if
    Util::Delete(outfile.data());
  } catch (const exception &ex) {
    cerr << "Error occured: " << ex.what() << endl;
    exit(EXIT_FAILURE);
  }  // try
}

