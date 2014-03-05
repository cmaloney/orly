/* <stig/closure.test.cc>

   Unit test for <stig/closure.h>.

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

#include <stig/closure.h>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <test/kit.h>

using namespace std;
using namespace Io;
using namespace Stig;

template <typename TVal>
void CheckArg(const TClosure &closure, const char *name, const TVal &expected) {
  TVal actual;
  EXPECT_EQ(closure.GetArg(name, actual), expected);
}

void CheckClosure(const TClosure &closure) {
  EXPECT_EQ(closure.GetMethodName(), string("add"));
  EXPECT_EQ(closure.GetArgCount(), 2u);
  CheckArg(closure, "x", 10);
  CheckArg(closure, "y", 20);
}

FIXTURE(Typical) {
  TClosure closure("add", string("x"), 10, string("y"), 20);
  CheckClosure(closure);
  auto recorder = make_shared<TRecorder>();
  /* write */ {
    TBinaryOutputOnlyStream strm(recorder);
    strm << closure;
  }
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
  TClosure copyof_closure;
  strm >> copyof_closure;
  CheckClosure(copyof_closure);
}
