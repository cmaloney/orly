/* <starsha/runner.cc>

   Implements <starsha/runner.h>.

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

#include <starsha/runner.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>

#include <base/class_traits.h>
#include <base/subprocess.h>
#include <strm/bin/in.h>
#include <strm/fd.h>

using namespace std;
using namespace Base;
using namespace Starsha;
using namespace Strm;

//TODO: Move to <strm/text/>
class TBgLineReader : public In::TCons {
  NO_COPY(TBgLineReader);

  public:
  TBgLineReader(In::TProd *prod, vector<string> &out)
      : In::TCons(prod), Out(out), BgReader(&TBgLineReader::ReadLinesToEof, this) {}

  ~TBgLineReader() {
    BgReader.join();
  }

  private:

  void ReadLinesToEof() {
    array<char, 4096> buf;
    using TIt = array<char, 4096>::const_iterator;

    //Loop will naturally exit when TryRead returns no data (This is one extra call to it, but good enough.)
    while(size_t act = TryRead(buf.data(), 4096)) {
      // Find each '\n', split at it, and move the line before that point to Out.
      TIt start = buf.cbegin();
      const TIt end = buf.cbegin() + act;
      TIt it;
      while((it = find(start, end, '\n')) != end) {
        CurLine.append(start, it);
        Out.emplace_back(move(CurLine));
        start = it + 1;
      }

      if (start != end) {
        CurLine.append(start, end);
      }
    }

    // Catch anything leftover
    if(!CurLine.empty()) {
      Out.emplace_back(move(CurLine));
    }
  }

  vector<string> &Out;
  string CurLine;

  thread BgReader;
};

void Starsha::Run(const string &cmd, vector<string> &lines, bool print_cmd) {
  assert(&cmd);
  assert(&lines);

  if(print_cmd) {
    cout << cmd << '\n';
  }

  // TOOD: should really memory cap the pump...
  // TODO: Jumping through <base/pump> here and <strm> gives us several unneded / extra copies.
  TPump pump;
  auto subproc = TSubprocess::New(pump, cmd.c_str());

  // TODO: We won't be providing any input. Tell the child process that.
  close(subproc->TakeStdInToChild());

  // Read stdout into lines
  TFdDefault stdout_fd(subproc->TakeStdOutFromChild());
  TBgLineReader stdout(&stdout_fd, lines);

  // Wait for process to exit
  int status = subproc->Wait();

  TFdDefault stderr_fd(subproc->TakeStdErrFromChild());
  Bin::TIn stderr_reader(&stderr_fd);

  if(status || stderr_reader) {
    // TODO: This reporting method sucks / lots of needless copying of potentially large strings...
    // Move all the data into a runtime_error
    ostringstream strm;
    if(stderr_reader) {
      uint8_t buf[4096];

      // Copy everything to the error message
      while(size_t read = stderr_reader.TryRead(buf, 4096)) {
        strm.write(reinterpret_cast<char*>(buf), read);
      }

    }
    if(status) {
      strm << "\n\nexit_code = " << status;
    }
    //TODO: Embed the data better / make a custom error object
    throw TRunError(status, strm.str());
  }
}
