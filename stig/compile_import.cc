/* <stig/compile_import.cc>

   Compile a Stig import file.

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

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <fcntl.h>
#include <syslog.h>

#include <base/log.h>
#include <io/device.h>
#include <io/binary_output_only_stream.h>
#include <stig/atom/transport_arena2.h>
#include <stig/client/program/parse_image.h>
#include <stig/native/all.h>
#include <stig/sabot/all.h>
#include <stig/type/type_czar.h>
#include <stig/var/sabot_to_var.h>

using namespace std;
using namespace Stig;

static Type::TTypeCzar TypeCzar;

/* Command-line arguments. */
class TCmd final
    : public Base::TLog::TCmd {
  public:

  /* Construct with defaults. */
  TCmd() {}

  /* Construct from argc/argv. */
  TCmd(int argc, char *argv[]) {
    Parse(argc, argv, TMeta());
  }

  /* The files to compile. */
  vector<string> ImportFiles;

  private:

  /* Our meta-type. */
  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    /* Registers our fields. */
    TMeta()
        : Base::TLog::TCmd::TMeta("The Stig import file compiler.") {
      Param(&TCmd::ImportFiles, "import_file", Required, "A an import file to compile.");
    }

  };  // TCmd::TMeta

};  // TCmd

/* Count the transactions in an import file. */
static size_t Count(const string &input_file) {
  syslog(LOG_INFO, "counting transactions in \"%s\"", input_file.c_str());
  size_t result = 0;
  Stig::Client::Program::ParseImageFile(
      input_file.c_str(),
      [&result](const Stig::Client::Program::TXact *) {
        ++result;
        return true;
      }
  );
  return result;
}

/* Compile an import file. */
static void Compile(const string &input_file) {
  size_t xact_count = Count(input_file);
  string output_file = input_file + ".arenas";
  syslog(LOG_INFO, "compiling %ld transactions(s) from \"%s\" to \"%s\"", xact_count, input_file.c_str(), output_file.c_str());
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open(output_file.c_str(), O_WRONLY | O_CREAT, 0777)));
  strm << xact_count;
  Stig::Client::Program::ParseImageFile(
      input_file.c_str(),
      [&strm](const Stig::Client::Program::TXact *xact) {
        std::vector<Var::TVar> kv;
        TranslateXact(
            xact,
            [&kv](const Sabot::State::TAny *lhs_state, const Sabot::State::TAny *rhs_state) {
              kv.push_back(Var::ToVar(*lhs_state));
              kv.push_back(Var::ToVar(*rhs_state));
              return true;
            }
        );
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        Atom::TSuprena arena;
        Atom::TCore core(kv, &arena, state_alloc);
        Atom::TTransportArena::Write(strm, &arena, core);
        return true;
      }
  );
}

/* Main. */
int main(int argc, char *argv[]) {
  TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  for (const auto &import_file: cmd.ImportFiles) {
    Compile(import_file);
  }
  return EXIT_SUCCESS;
}
