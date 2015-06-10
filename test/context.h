/* Execution context for tests.

A context captures failures that occur with in it. A single failure causes all
previous log messages to be flushed out. And all future log messages at that
context to be written straight to the output.

Singleton context + helper functions to make the test harness simpler.

Only a single unit test may be running at any given point in time. That test
will have all fixtures and expectations logged into it's context through
these mechanisms.

TODO(cmaloney): Make contexts thread-safe. Probably best method is to make
  context thread-local then gather up.

TODO(cmaloney): Think about mapping all of this to "future" test results then
all we are doing is just evaluating all independently and collecting the results.
*/
#pragma once

#include <sstream>

#include <base/class_traits.h>
#include <base/code_location.h>

namespace Test {

// Stack based logging aggregators. Top of the stack aggregates all
// log results.
class TContext {
  NO_COPY(TContext)
  NO_MOVE(TContext)

  public:
  class TLog;

  static inline TLog Log(const Base::TCodeLocation &loc, const char *text, bool passed) {
    return TLog(loc, text, passed);
  }

  // Log an exception, something without a position / unevaluated expression.
  static inline TLog Log(bool passed) { return TLog(passed); }

  // Log a fixture to the parent context
  static inline void Log(const char * /* name */, bool passed) { Current()->LogResult(passed); }

  TContext(const char *name);
  ~TContext();

  inline uint64_t GetFailureCount() const { return FailureCount; }
  inline uint64_t GetPassCount() const { return PassCount; }

  // TODO(cmaloney): Logging to XML type reports
  /* Builds a stack of ostringstream loggers. All TLog() statements
     attach to the nearest logger's console. For things like structured
     result reports a different mechanism will need to be made. */
  class TLog {
    public:
    // NOTE(cmaloney): Semantically we could support a copy, but in regular
    // usage I don't think it is necessary.
    MOVE_ONLY(TLog)

    ~TLog();

    template <typename TVal>
    TLog &operator<<(const TVal &val) {
      if(Print) {
        Current()->GetWriteTarget() << val;
      }
      return *this;
    }

    private:
    TLog(const Base::TCodeLocation &loc, const std::string &&Test, bool passed);

    TLog(bool passed);

    bool Passed, Print;
    friend class TContext;
  };

  // Maintainging the current context stack
  static void Push(TContext *ctx);
  static TContext *Pop();
  static TContext *Current();

  private:
  std::ostream &GetWriteTarget();

  void LogResult(bool pass);

  const char *Name;
  std::ostringstream BuffWriter;
  bool WriteDirect = false;
  TContext *Parent = nullptr;
  uint64_t FailureCount = 0, PassCount = 0;
};

} // Test
