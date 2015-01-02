/* Execution context for tests.

Singleton context + helper functions to make the test harness simpler.

Only a single unit test may be running at any given point in time. That test
will have all fixtures and expectations logged into it's context through
these mechanisms. */

#include <base/class_traits.h>
#include <base/code_location.h>
#include <test/options.h>

namespace Test {

  // Stack based logging aggregators. Top of the stack aggregates all
  // log results.
  class TContext {
    NO_COPY(TContext);
    NO_MOVE(TContext);
    public:

    static void LogResult(bool pass);

    TContext() : Parent(GetCurrentContext()) {
      PushContext(this);
    }
    ~TContext() {
      TContext *ctx = PopContext();
      assert(this == ctx);
      assert(GetContext() == Parent);
      if (FailCount) {
        Parent->LogResult(false);
      }
    }


    std::ostream &GetWriteTarget() {
      if (WriteDirect) {
        return std::cout;
      } else {
        return BuffWriter;
      }
    }

    //TODO(cmaloney): Make thread safe.
    void LogResult(bool pass) {
      if (pass) {
        ++PassCount;
      } else {
        ++FailCount
        WriteDirect = true;
        // On the first failure
        if (FailCount == 1) {
          GetWriteTarget() << BuffWriter.str();
        }
      }
    }

    private:
    bool WriteDirect = false;
    std::ostringstream BuffWriter;
    TContext *Parent=nullptr;
    uint64_t FailCount=0, PassCount=0;

    // Maintainging the current context stack
    static void Push(TContext *ctx);
    static TContext *Pop();
    static TContext *Current();
  };

namespace Context {

  //TODO(cmaloney): Printing results
  //if (Print) {
  //  MessageBuilder << "; " << (Passed ? "pass" : "fail") << std::endl;
  //}
  std::vector<TResult> SwapResults();
  void ResetResults();

  // TODO(cmaloney): Logging to XML type reports
  /* Builds a stack of ostringstream loggers. All TLog() statements
     attach to the nearest logger's console. For things like structured
     result reports a different mechanism will need to be made. */
  class TLog {
    public:

    // NOTE(cmaloney): Semantically we could support a copy, but in regular
    // usage I don't think it is necessary.
    MOVE_ONLY(TLogger);


    // TODO(cmaloney): Move to cc.
    TLogger(const Base::TCodeLocation &loc, const std::string &&Test, bool passed)
          : Passed(passed), Print(!passed || GetOptions().Verbose) {

      Context::AddResult(passed);
    }

    ~TLogger() {
      AddResult(loc, text, expression, passed);
      AddRseult(Location, Test, MessageBuilder.str(), passed);
    }

    template <typename TVal>
    void Write(const TVal &val) {
      MessageBuilder << val;
    }

    private:
    ostringstream MessageBuilder;
    bool Passed, Print;
  };

  template <typename TVal>
  operator<<(TLogger &&logger, const TVal &val) {
    logger.Write(val);
  }

  TLogger LogResult(const Base::TCodeLocation &loc, const char *text, bool passed) {
    return TLogger(loc, text, passed);
  }
}
}