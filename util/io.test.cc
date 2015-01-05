/* <util/io.test.cc>

   Unit test for <util/io.h>.

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

#include <util/io.h>

#include <cerrno>
#include <cstring>
#include <string>
#include <system_error>

#include <signal.h>

#include <base/fd.h>
#include <base/zero.h>
#include <test/kit.h>

using namespace Base;
using namespace std;
using namespace std::literals;
using namespace Util;

static const char *ExpectedData = "Mofo the Psychic Gorilla";

static const size_t ExpectedSize = 24;

static const size_t MaxActualSize = 1024;
static char ActualData[MaxActualSize];

FIXTURE(ReadAtMost) {
  // Validate above constants
  assert(ExpectedSize == strlen(ExpectedData));

  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  WriteExactly(writeable, ExpectedData, ExpectedSize);
  Zero(ActualData);
  size_t actual_size = ReadAtMost(readable, ActualData, MaxActualSize);
  if(EXPECT_EQ(actual_size, ExpectedSize)) {
    EXPECT_FALSE(strcmp(ActualData, ExpectedData));
  }
}

FIXTURE(WriteAtMost) {
  struct sigaction action;
  Zero(action);
  action.sa_handler = [](int) {};
  sigaction(SIGPIPE, &action, 0);
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  readable.Reset();
  size_t actual_size = WriteAtMost(writeable, 0, 0);
  EXPECT_FALSE(actual_size);
#ifdef __linux__
  bool caught_broken_pipe = false;
  try {
    WriteAtMost(writeable, ExpectedData, ExpectedSize);
  } catch(const system_error &error) {
    /* TODO: change this to error.code() == errc::broken_pipe */
    caught_broken_pipe = (error.code().value() == EPIPE);
  }
  EXPECT_TRUE(caught_broken_pipe);
#endif
  writeable.Reset();
}

FIXTURE(TryReadExactlyNothing) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  writeable.Reset();
  EXPECT_FALSE(TryReadExactly(readable, ActualData, ExpectedSize));
}

FIXTURE(TryReadExactlyEverything) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  WriteExactly(writeable, ExpectedData, ExpectedSize);
  EXPECT_TRUE(TryReadExactly(readable, ActualData, ExpectedSize));
}

FIXTURE(TryReadExactlySomething) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  WriteExactly(writeable, ExpectedData, ExpectedSize / 2);
  writeable.Reset();
  bool caught_unexpected_end = false;
  try {
    TryReadExactly(readable, ActualData, ExpectedSize);
  } catch(const TUnexpectedEnd &) {
    caught_unexpected_end = true;
  }
  EXPECT_TRUE(caught_unexpected_end);
}

FIXTURE(ReadExactlyNothing) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  writeable.Reset();
  bool caught_could_not_start = false;
  try {
    ReadExactly(readable, ActualData, ExpectedSize);
  } catch(const TCouldNotStart &) {
    caught_could_not_start = true;
  }
  EXPECT_TRUE(caught_could_not_start);
}

FIXTURE(ReadExactlyEverything) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  WriteExactly(writeable, ExpectedData, ExpectedSize);
  Zero(ActualData);
  ReadExactly(readable, ActualData, ExpectedSize);
  EXPECT_FALSE(strcmp(ActualData, ExpectedData));
}

FIXTURE(ReadExactlySomething) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  WriteExactly(writeable, ExpectedData, ExpectedSize / 2);
  writeable.Reset();
  bool caught_unexpected_end = false;
  try {
    ReadExactly(readable, ActualData, ExpectedSize);
  } catch(const TUnexpectedEnd &) {
    caught_unexpected_end = true;
  }
  EXPECT_TRUE(caught_unexpected_end);
}
