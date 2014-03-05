/* <test/fixture.h>

   A fixture in a unit test program.

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

#pragma once

#include <cassert>

#include <base/code_location.h>
#include <base/no_copy_semantics.h>

#define FIXTURE(name) \
  static void name##_(); \
  static const ::Test::TFixture name##Fixture(HERE, #name, name##_); \
  static void name##_()

namespace Test {

  /* TODO */
  class TFixture {
    NO_COPY_SEMANTICS(TFixture);
    public:

    /* TODO */
    typedef void (*TFunc)();

    /* TODO */
    TFixture(
        const Base::TCodeLocation &code_location, const char *name, TFunc func);

    /* TODO */
    TFunc GetFunc() const {
      assert(this);
      return Func;
    }

    /* TODO */
    const char *GetName() const {
      assert(this);
      return Name;
    }

    /* TODO */
    const TFixture *GetNextFixture() const {
      assert(this);
      return NextFixture;
    }

    /* TODO */
    static const TFixture *GetFirstFixture() {
      return FirstFixture;
    }

    private:

    /* TODO */
    Base::TCodeLocation CodeLocation;

    /* TODO */
    const char *Name;

    /* TODO */
    TFunc Func;

    /* TODO */
    mutable const TFixture *NextFixture;

    /* TODO */
    static const TFixture *FirstFixture, *LastFixture;
  };

}
