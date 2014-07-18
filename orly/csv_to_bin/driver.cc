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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>

#include <fcntl.h>

#include <base/fd.h>
#include <base/glob.h>
#include <base/log.h>
#include <orly/csv_to_bin/level1.h>
#include <orly/csv_to_bin/level2.h>
#include <orly/csv_to_bin/level3.h>
#include <orly/csv_to_bin/translate.h>
#include <strm/fd.h>

using namespace std;
using namespace Orly::CsvToBin;

class TCmd final
    : public Base::TLog::TCmd {
  public:

  TCmd(int argc, char *argv[])
      : MaxKvPerFile(250000),
        UnixEol(TLevel1::DefaultOptions.UnixEol),
        UseEsc(TLevel1::DefaultOptions.UseEsc),
        UseQuoteQuote(TLevel1::DefaultOptions.UseQuoteQuote),
        TrueKwd(TLevel3::DefaultOptions.TrueKwd),
        FalseKwd(TLevel3::DefaultOptions.FalseKwd) {
    Delim += TLevel1::DefaultOptions.Delim;
    Quote += TLevel1::DefaultOptions.Quote;
    Esc   += TLevel1::DefaultOptions.Esc;
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
    if (UseEsc && Quote.size() != 1u && !cb("escape must be a single byte")) {
      return false;
    }
    if (TrueKwd.empty() && !cb("true_kwd must not be an empty string")) {
      return false;
    }
    if (FalseKwd.empty() && !cb("false_kwd must not be an empty string")) {
      return false;
    }
    ToLower(TrueKwd);
    ToLower(FalseKwd);
    return true;
  }

  string Delim, Quote, Esc, InPattern, OutPrefix;
  size_t MaxKvPerFile;
  bool UnixEol, UseEsc, UseQuoteQuote;
  string TrueKwd, FalseKwd;

  static void ToLower(string &str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
  }

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
          &TCmd::Esc, "esc", Optional, "esc\0e\0",
          "The charater to expect as an escape inside quoted fields.");
      Param(
          &TCmd::UnixEol, "unix_eol", Optional, "unix_eol\0u\0",
          "Expect Unix-style EOL (LF) instead of CSV standard (CRLF).");
      Param(
          &TCmd::UseEsc, "use_esc", Optional, "use_esc\0eq\0",
          "Expect escape sequences inside of quoted fields.");
      Param(
          &TCmd::UseEsc, "use_quote_quote", Optional, "use_quote_quote\0qq\0",
          "Treat quote-quote sequences inside of quotes as a single quote.");
      Param(
          &TCmd::TrueKwd, "true_kwd", Optional, "true_kwd\0t\0",
          "The keyword to accept as bool true.");
      Param(
          &TCmd::FalseKwd, "false_kwd", Optional, "false_kwd\0f\0",
          "The keyword to accept as bool false.");
      Param(&TCmd::MaxKvPerFile,
            "max_kv_per_file",
            Optional,
            "max_kv_per_file\0m\0",
            "The maximum number of key-value pairs per Orly binary file.");
      Param(
          &TCmd::InPattern, "in_pattern", Required, "in_pattern\0i\0",
          "The pattern of input file to read from (CSV).");
      Param(
          &TCmd::OutPrefix, "out_prefix", Required, "out_prefix\0o\0",
          "The prefix of the output files to write to (Orly binary).");
    }

  };  // TCmd::TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  TTranslate translate(cmd.OutPrefix, cmd.MaxKvPerFile);
  Base::Glob(
      cmd.InPattern.data(),
      [&](const char *name) {
        // Open input/output file.
        Base::TFd fd;
        try {
          fd = Base::TFd(open(name, O_RDONLY));
        } catch (const exception &ex) {
          cerr << "error opening \"" << name << "\": " << ex.what() << endl;
          exit(EXIT_FAILURE);
        }  // try
        Strm::TFd<> file(move(fd));
        // Parse CSV.
        TLevel1 level1(
            &file,
            { static_cast<uint8_t>(cmd.Delim[0]),
              static_cast<uint8_t>(cmd.Quote[0]),
              cmd.UnixEol,
              cmd.UseEsc,
              static_cast<uint8_t>(cmd.Esc[0]),
              cmd.UseQuoteQuote
            });
        TLevel2 level2(level1);
        TLevel3 level3(level2, { cmd.TrueKwd, cmd.FalseKwd });
        translate(level3);
        return true;
      });
  return EXIT_SUCCESS;
}
