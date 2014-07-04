/* <orly/csv_to_bin/driver.cc>

   Command-line driver to be compiled with all generated CSV converters.

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

#include <cassert>
#include <iostream>
#include <string>
#include <utility>

#include <fcntl.h>

#include <base/fd.h>
#include <base/log.h>
#include <orly/csv_to_bin/in.h>
#include <strm/fd.h>

using namespace std;
using namespace Orly::CsvToBin;

class TCmd final
    : public Base::TLog::TCmd {
  public:

  TCmd(int argc, char *argv[]) {
    Delim += TIn::DefaultOptions.Delim;
    Quote += TIn::DefaultOptions.Quote;
    UnixEol = TIn::DefaultOptions.UnixEol;
    Parse(argc, argv, TMeta());
  }

  virtual bool CheckArgs(const TMeta::TMessageConsumer &cb) override {
    assert(this);
    if (Delim.size() != 1u && !cb("delim must be a single byte")) {
      return false;
    }
    if (Quote.size() != 1u && !cb("quote must be a single byte")) {
      return false;
    }
    return true;
  }

  string Delim, Quote, InPath, OutPath;
  bool UnixEol;

  private:

  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    TMeta()
        : Base::TLog::TCmd::TMeta("CSV to Orly binary converter.") {
      Param(
          &TCmd::Delim, "delim", Optional, "delim\0d\0",
          "The charater to expect as a field delimiter.");
      Param(
          &TCmd::Quote, "quote", Optional, "quote\0q\0",
          "The charater to expect as a quote mark.");
      Param(
          &TCmd::UnixEol, "unix_eol", Optional, "unix_eol\0u\0",
          "Expect Unix-style EOL (LF) instead of CSV standard (CRLF).");
      Param(
          &TCmd::InPath, "in_path", Required,
          "The path to the file to read from (CSV).");
      Param(
          &TCmd::OutPath, "out_path", Required,
          "The path to the file to write to (Orly binary).");
    }

  };  // TCmd::TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  Base::TFd fd;
  try {
    fd = Base::TFd(open(cmd.InPath.c_str(), O_RDONLY));
  } catch (const exception &ex) {
    cerr << "error opening \"" << cmd.InPath << "\": " << ex.what() << endl;
    exit(EXIT_FAILURE);
  }
  Strm::TFd<> file(move(fd));
  TIn strm(
      &file,
      { static_cast<uint8_t>(cmd.Delim[0]),
        static_cast<uint8_t>(cmd.Quote[0]),
        cmd.UnixEol
      });
  return EXIT_SUCCESS;
}
