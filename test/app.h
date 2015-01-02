/* <test/app.h>

   The application framework for unit test executables.

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

#pragma once

#include <cassert>
#include <cstddef>
#include <iostream>

#include <base/assert_true.h>
#include <base/class_traits.h>
#include <cmd/args.h>
namespace Test {

  /* TODO */
  class TFixture;


  class TApp final {
    NO_COPY(TApp);
    public:

    class TLogger {
      NO_COPY(TLogger);
      public:

      TLogger(bool is_critical = false) {
        Enabled = is_critical || TApp::IsVerbose();
      }

      ~TLogger() {
        assert(this);
        if (Enabled) {
          std::cout << std::endl;
        }
      }

      template <typename TVal>
      const TLogger &Write(const TVal &val) const {
        assert(this);
        if (Enabled) {
          std::cout << val;
        }
        return *this;
      }

      private:
      bool Enabled;
    };

    class TRunner {
      NO_COPY(TRunner);
      public:

      virtual ~TRunner();

      virtual operator bool() const = 0;

      static void Run(TApp *app, const TFixture *fixture);

      protected:
      TRunner(TApp *app) : App(app) {}

      void PreDtor();

      TApp *App;
    };

    TApp(const TOptions &cmd)
        : Options(options), PassCount(0), FailCount(0) {
      assert(!App);
      App = this;
    }

    ~TApp() {
      assert(this);
      assert(App == this);
      App = nullptr;
    }

    const TOptions &GetOptions() const {
      assert(this);
      return Options;
    }

    void OnRunnerDtor(const TRunner *runner);

    int Run();

    static const TApp *GetApp() {
      assert(App);
      return App;
    }

    private:


    const TOptions &Options;

    /* The number of fixtures which passed and failed. */
    size_t PassCount, FailCount;

    /* See accessor. */
    static TApp *App;

  };

  /* A stream inserter for Test::TApp::TLogger targets. */
  template <typename TVal>
  const Test::TApp::TLogger &operator<<(
      const Test::TApp::TLogger &logger, const TVal &val) {
    assert(&logger);
    return logger.Write(val);
  }
}