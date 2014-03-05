/* <base/bg_generator.test.cc>

   Unit test for <base/bg_generator.h>.

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

#include <base/bg_generator.h>

#include <string>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace placeholders;

using namespace Base;

class TMaxCountReached final
    : public runtime_error {
  public:

  TMaxCountReached()
      : runtime_error("max count reached") {}
};

class TExistenceChecker final {
  public:

  TExistenceChecker() {
    assert(!Flag);
    Flag = true;
  }

  ~TExistenceChecker() {
    assert(Flag);
    Flag = false;
  }

  static bool Exists() {
    return Flag;
  }

  private:

  static bool Flag;

};

bool TExistenceChecker::Flag = false;

static const vector<string> Strings = {
    "hello", "doctor", "name", "continue", "yesterday", "tomorrow"
};

static bool GenerateStrings(
    const function<bool (const string &)> &cb, size_t max_count) {
  assert(&cb);
  assert(cb);
  const TExistenceChecker existence_checker;
  size_t count = 0;
  for (const auto &str: Strings) {
    if (count >= max_count) {
      throw TMaxCountReached();
    }
    if (!cb(string(str))) {
      return false;
    }
    ++count;
  }
  return true;
}

FIXTURE(GetDefStackSize) {
  EXPECT_GT(TAnyBgGenerator::GetDefStackSize(), 0u);
}

FIXTURE(RunToCompletion) {
  auto expected = Strings.begin();
  for (TBgGenerator<string> actual(bind(GenerateStrings, _1, Strings.size()));
      actual; ++actual, ++expected) {
    EXPECT_TRUE(TExistenceChecker::Exists());
    if (EXPECT_FALSE(expected == Strings.end())) {
      EXPECT_EQ(*actual, *expected);
    }
  }
  EXPECT_FALSE(TExistenceChecker::Exists());
  EXPECT_TRUE(expected == Strings.end());
}

FIXTURE(StopEarlyInFg) {
  auto expected = Strings.begin();
  size_t count = 0;
  for (TBgGenerator<string> actual(bind(GenerateStrings, _1, Strings.size()));
      actual && count < Strings.size() / 2; ++actual, ++expected, ++count) {
    EXPECT_TRUE(TExistenceChecker::Exists());
    if (EXPECT_FALSE(expected == Strings.end())) {
      EXPECT_EQ(*actual, *expected);
    }
  }
  EXPECT_FALSE(TExistenceChecker::Exists());
  EXPECT_FALSE(expected == Strings.end());
}

FIXTURE(StopEarlyInBg) {
  auto expected = Strings.begin();
  bool caught = false;
  try {
    for (TBgGenerator<string> actual(bind(GenerateStrings, _1, Strings.size() / 2));
        actual; ++actual, ++expected) {
      EXPECT_TRUE(TExistenceChecker::Exists());
      if (EXPECT_FALSE(expected == Strings.end())) {
        EXPECT_EQ(*actual, *expected);
      }
    }
  } catch (const TMaxCountReached &) {
    caught = true;
  }
  EXPECT_FALSE(TExistenceChecker::Exists());
  EXPECT_FALSE(expected == Strings.end());
  EXPECT_TRUE(caught);
}

FIXTURE(DefaultCtor) {
  size_t count = 0;
  for (TBgGenerator<string> gen; gen; ++gen, ++count);
  EXPECT_EQ(count, 0u);
}

FIXTURE(Swap) {
  TBgGenerator<string>
      a(bind(GenerateStrings, _1, Strings.size())),
      b;
  a.Swap(b);
  size_t count = 0;
  for (; a; ++a, ++count);
  EXPECT_EQ(count, 0u);
  for (; b; ++b, ++count);
  EXPECT_EQ(count, Strings.size());
}

FIXTURE(MoveCtor) {
  TBgGenerator<string>
      a(bind(GenerateStrings, _1, Strings.size())),
      b(move(a));
  size_t count = 0;
  for (; a; ++a, ++count);
  EXPECT_EQ(count, 0u);
  for (; b; ++b, ++count);
  EXPECT_EQ(count, Strings.size());
}

FIXTURE(MoveAssign) {
  TBgGenerator<string>
      a(bind(GenerateStrings, _1, Strings.size())),
      b;
  b = move(a);
  size_t count = 0;
  for (; a; ++a, ++count);
  EXPECT_EQ(count, 0u);
  for (; b; ++b, ++count);
  EXPECT_EQ(count, Strings.size());
}

/* A factory to make a string generator and move it back to the caller. */
static TBgGenerator<string> StringGenerator() {
  return TBgGenerator<string>(bind(GenerateStrings, _1, Strings.size()));
}

FIXTURE(Factory) {
  auto expected = Strings.begin();
  for (auto actual = StringGenerator(); actual; ++actual, ++expected) {
    EXPECT_TRUE(TExistenceChecker::Exists());
    if (EXPECT_FALSE(expected == Strings.end())) {
      EXPECT_EQ(*actual, *expected);
    }
  }
  EXPECT_FALSE(TExistenceChecker::Exists());
  EXPECT_TRUE(expected == Strings.end());
}
