/* A fixture in a unit test.

Fixtures are small groups of test expectations which are executed as a group
within the same context / with one setup/teardown.

Sample:
FIXTURE(Name) {
  // Test things
}


Internal details:
Fixtures are global statics.

There is a vector of fixtures which is built up on fixture construction.
Fixtures are never removed from the vector. The ownership of fixture objects
all have static lifetimes.

The vector is only used during main() where the fixtures are all alive. The
vector takes now ownership of any of the fixtures. */

#pragma once

#include <cassert>
#include <vector>

#include <base/class_traits.h>
#include <base/code_location.h>

#define FIXTURE(name) \
  static void name##_(); \
  static const ::Test::TFixture name##Fixture (HERE, #name, name##_); \
  static void name##_()

namespace Test {

  /* A fixture is a group of unit tests which use the same execution
     environment to exercise some particular aspect of a test. */
  struct TFixture {
    NO_COPY(TFixture);

    typedef void (*TFunc)();

    TFixture(Base::TCodeLocation code_location, const char *name, const TFunc func);

    const Base::TCodeLocation CodeLocation;
    const char *Name;
    const TFunc Func;
  };

  const std::vector<const TFixture*> &GetFixtures();
  void AddFixture(const TFixture *fixture);

} // namespace Test
