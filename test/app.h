/* <test/app.h>

   The application framework for unit test executables.

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
#include <cstddef>
#include <iostream>

#include <base/assert_true.h>
#include <base/cmd.h>
#include <base/no_construction.h>
#include <base/no_copy_semantics.h>

namespace Test {

  /* TODO */
  class TFixture;

  class TApp final {
    NO_COPY_SEMANTICS(TApp);
    public:

    class TCmd : public Base::TCmd {
      NO_COPY_SEMANTICS(TCmd);
      public:

      TCmd(int argc, char **argv) : PrintTiming(false), VerboseMember(false) {
        Parse(argc, argv, TMeta());
      }

      private:
      class TMeta : public Base::TCmd::TMeta {
        public:
        TMeta() : Base::TCmd::TMeta("STIG Unit Test") {
          Param(&TCmd::VerboseMember, "verbose", Optional, "verbose\0v\0", "Show the results of unit tests, regardless of whether they pass or fail");
          Param(&TCmd::PrintTiming, "print_timing", Optional, "timing\0t\0", "Print execution time for each fixture");
        }
      };

      //TODO: These should be private with accessor functions, but this is faster to implement.
      public:
      bool PrintTiming;
      bool VerboseMember;
    };

    /* TODO */
    class TLogger {
      NO_COPY_SEMANTICS(TLogger);
      public:

      /* TODO */
      TLogger(bool is_critical = false) {
        Enabled = is_critical || TApp::IsVerbose();
      }

      /* TODO */
      ~TLogger() {
        assert(this);
        if (Enabled) {
          std::cout << std::endl;
        }
      }

      /* TODO */
      template <typename TVal>
      const TLogger &Write(const TVal &val) const {
        assert(this);
        if (Enabled) {
          std::cout << val;
        }
        return *this;
      }

      private:

      /* TODO */
      bool Enabled;
    };

    /* TODO */
    class TRunner {
      NO_COPY_SEMANTICS(TRunner);
      public:

      /* TODO */
      virtual ~TRunner();

      /* TODO */
      virtual operator bool() const = 0;

      /* TODO */
      static void Run(TApp *app, const TFixture *fixture);

      protected:

      /* TODO */
      TRunner(TApp *app) : App(app) {}

      /* TODO */
      void PreDtor();

      TApp *App;
    };

    TApp(const TCmd &cmd)
        : Cmd(cmd), PassCount(0), FailCount(0) {
      assert(!App);
      App = this;
    }

    ~TApp() {
      assert(this);
      assert(App == this);
      App = nullptr;
    }

    const TCmd &GetCmd() const {
      assert(this);
      return Cmd;
    }

    void OnRunnerDtor(const TRunner *runner);

    int Run();

    static const TApp *GetApp() {
      assert(App);
      return App;
    }

    static bool IsVerbose() {
      return Verbose;
    }

    private:


    //TODO: This should be one variable, not two, but adding that support to <base/cmd.h> is a bigger task than I have time for right now.
    static bool Verbose;
    const TCmd &Cmd;

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