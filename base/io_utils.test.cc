/* <base/io_utils.test.cc>

   Unit test for <base/io_utils.h>.

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

#include <base/io_utils.h>

#include <cerrno>
#include <cstring>
#include <string>
#include <system_error>

#include <signal.h>

#include <base/fd.h>
#include <base/zero.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

static const char *ExpectedData = "Mofo the Psychic Gorilla";
static const size_t ExpectedSize = strlen(ExpectedData);

static const size_t MaxActualSize = 1024;
static char ActualData[MaxActualSize];

FIXTURE(ReadAtMost) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  WriteExactly(writeable, ExpectedData, ExpectedSize);
  Zero(ActualData);
  size_t actual_size = ReadAtMost(readable, ActualData, MaxActualSize);
  if (EXPECT_EQ(actual_size, ExpectedSize)) {
    EXPECT_FALSE(strcmp(ActualData, ExpectedData));
  }
  bool timed_out = false;
  try {
    actual_size = ReadAtMost(readable, ActualData, MaxActualSize, 1000);
  } catch (const system_error &x) {
    if (x.code().value() == ETIMEDOUT) {
      timed_out = true;
    } else {
      EXPECT_TRUE(false);
    }
  }
  EXPECT_TRUE(timed_out);
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
  bool caught_broken_pipe = false;
  try {
    WriteAtMost(writeable, ExpectedData, ExpectedSize);
  } catch (const system_error &error) {
    /* TODO: change this to error.code() == errc::broken_pipe */
    caught_broken_pipe = (error.code().value() == EPIPE);
  }
  EXPECT_TRUE(caught_broken_pipe);
  writeable.Reset();
  TFd::Pipe(readable, writeable);
  bool timed_out = false;
  try {
    for (; ; ) {
      WriteAtMost(writeable, ExpectedData, ExpectedSize, 1000);
    }
  } catch (const system_error &x) {
    if (x.code().value() == ETIMEDOUT) {
      timed_out = true;
    } else {
      EXPECT_TRUE(false);
    }
  }
  EXPECT_TRUE(timed_out);
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

FIXTURE(TryReadExactlyTimeout) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  bool timed_out = false;
  try {
    TryReadExactly(readable, ActualData, ExpectedSize, 1000);
  } catch (const system_error &x) {
    if (x.code().value() == ETIMEDOUT) {
      timed_out = true;
    } else {
      EXPECT_TRUE(false);
    }
  }
  EXPECT_TRUE(timed_out);
}

FIXTURE(TryWriteExactlyTimeout) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  bool timed_out = false;
  try {
    for (; ; ) {
      TryWriteExactly(writeable, ExpectedData, ExpectedSize, 1000);
    }
  } catch (const system_error &x) {
    if (x.code().value() == ETIMEDOUT) {
      timed_out = true;
    } else {
      EXPECT_TRUE(false);
    }
  }
  EXPECT_TRUE(timed_out);
}

FIXTURE(TryReadExactlySomething) {
  TFd readable, writeable;
  TFd::Pipe(readable, writeable);
  WriteExactly(writeable, ExpectedData, ExpectedSize / 2);
  writeable.Reset();
  bool caught_unexpected_end = false;
  try {
    TryReadExactly(readable, ActualData, ExpectedSize);
  } catch (const TUnexpectedEnd &) {
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
  } catch (const TCouldNotStart &) {
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
  } catch (const TUnexpectedEnd &) {
    caught_unexpected_end = true;
  }
  EXPECT_TRUE(caught_unexpected_end);
}
