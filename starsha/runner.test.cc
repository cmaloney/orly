/* <starsha/runner.test.cc>

   Implements <starsha/runner.h>.

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

#include <starsha/runner.h>

#include <memory>
#include <string>

#include <limits.h>

#include <starsha/string_builder.h>
#include <test/kit.h>

using namespace std;
using namespace Starsha;

bool PrintCmds = false;

static shared_ptr<TRunner> NewRunner(const char *name) {
  char path[PATH_MAX];
  strcpy(path, Test::TApp::GetApp()->GetCmd().GetDir());
  if (*path) {
    strcat(path, "/");
  }
  strcat(path, name);
  return make_shared<TRunner>(path);
}

struct TDatum {
  bool IsErr;
  const char *Line;
};

static shared_ptr<TRunner> NewDummyRunner() {
  return NewRunner("dummy");
}

static void TestData(const TDatum data[]) {
  auto runner = NewDummyRunner();
  const TDatum
    *out_csr = 0,
    *err_csr = 0;
  for (const TDatum *datum = data; datum->Line; ++datum) {
    if (datum->IsErr) {
      if (!err_csr) {
        err_csr = datum;
      }
    } else {
      if (!out_csr) {
        out_csr = datum;
      }
    }
    string temp;
    TStringBuilder(temp) << (datum->IsErr ? "err:" : "out:") << datum->Line << '\n';
    runner->Send(temp);
  }
  runner->Send("end\n");
  auto success = runner->ForEachLine([&out_csr, &err_csr](bool is_err, const char *line) {
    const TDatum *&csr = is_err ? err_csr : out_csr;
    if (!EXPECT_TRUE(csr)) {
      return false;
    }
    EXPECT_EQ(string(line), string(csr->Line));
    for (;;) {
      ++csr;
      if (!csr->Line) {
        csr = 0;
        break;
      }
      if (csr->IsErr == is_err) {
        break;
      }
    }
    return true;
  });
  EXPECT_TRUE(success);
  EXPECT_FALSE(out_csr);
  EXPECT_FALSE(err_csr);
  runner->Wait();
}

static void TestExit(const char *data, int expected) {
  auto runner = NewDummyRunner();
  runner->Send(data);
  int actual = runner->Wait();
  EXPECT_EQ(actual, expected);
}

FIXTURE(SuccessExit) {
  TestExit("end\n", EXIT_SUCCESS);
}

FIXTURE(FailureExit) {
  TestExit("abend\n", EXIT_FAILURE);
}

FIXTURE(MeaningfulExit) {
  TestExit("55\n", 55);
}

FIXTURE(OutOnly) {
  static const TDatum data[] = {
    { false, "hello" },
    { false, "doctor" },
    { false, "name" },
    { false, 0 }
  };
  TestData(data);
}

FIXTURE(ErrOnly) {
  static const TDatum data[] = {
    { true, "continue" },
    { true, "yesterday" },
    { true, "tomorrow" },
    { false, 0 }
  };
  TestData(data);
}

FIXTURE(OutAndErrMixed) {
  static const TDatum data[] = {
    { false, "hello" },
    { true,  "continue" },
    { true,  "yesterday" },
    { false, "doctor" },
    { false, "name" },
    { true,  "tomorrow" },
    { false, 0 }
  };
  TestData(data);
}

FIXTURE(NoLF) {
  auto runner = NewDummyRunner();
  runner->Send("nolf\n");
  int cnt = 0;
  runner->ForEachLine([&cnt](bool is_err, const char *line) {
    ++cnt;
    EXPECT_FALSE(is_err);
    EXPECT_EQ(string(line), string("nolf"));
    return true;
  });
  EXPECT_EQ(cnt, 1);
  EXPECT_EQ(runner->Wait(), 0);
}
