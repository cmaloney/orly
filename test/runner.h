/* <test/runner.h>

   Runs a fixture in unit test program.

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

#include <base/assert_true.h>
#include <base/no_copy_semantics.h>
#include <test/app.h>

namespace Test {

  /* TODO */
  class TFixture;

  /* TODO */
  class TRunner : public TApp::TRunner {
    NO_COPY_SEMANTICS(TRunner);
    public:

    /* TODO */
    class TExpect {
      NO_COPY_SEMANTICS(TExpect);
      public:

      /* TODO */
      virtual ~TExpect();

      /* TODO */
      virtual operator bool() const = 0;

      protected:

      /* TODO */
      TExpect() {}

      /* TODO */
      void PreDtor();
    };

    /* TODO */
    TRunner(TApp *app, const TFixture *fixture);

    /* TODO */
    virtual ~TRunner();

    /* TODO */
    virtual operator bool() const;

    /* TODO */
    void Run();

    /* TODO */
    static TRunner *GetRunner() {
      return Base::AssertTrue(Runner);
    }

    private:

    /* TODO */
    void OnExpectDtor(const TExpect *expect);

    /* TODO */
    const TFixture *Fixture;

    /* TODO */
    bool Pass;

    /* TODO */
    static TRunner *Runner;
  };

}
