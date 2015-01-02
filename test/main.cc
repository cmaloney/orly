/* Runs one unit test. */

#include <cstdlib>

#include <cmd/main.h>
#include <cmd/parse.h>
#include <test/context.h>
#include <test/options.h>
#include <test/fixture.h>

using namespace std;
using namespace Base;
using namespace Test;


void RunFixtures() {
  for(const TFixture *fixture: GetFixtures()) {
    //TODO(cmaloney): Stack based logging singleton?
    // A context captures failures that occur with in it. A single failure causes all previous
    // log messages to be flushed out. And all future log messages at that context to be written
    // straight to the output.
    TContext context(fixture->Name);
    // On construction initializes the log with
    // TLog() << "begin " << Name;
    // On destruction finishes it off with:
    // TLog() << "end " << Name << "; " << FailureCount ? "fail" : "pass" << '\n';

    fixture->Func();
  }
}

int Main(int argc, char *argv[]) {
  TParser parser;
  parser.Attach(GetArgs(), GetOptionsNonConst());

  // Set the target to be std::cout when we're printing all the test.
  // This makes it so that we don't buffer the results at all.
  //
  // In regular operation results are buffered (Fixture begin/end), test results
  // until there is a failure at which point we print out all the context
  // which leads to the failure (what fixture it was in), as well as the failure
  // itself.
  if (PrintTests) {
    TLogger::SetTarget(std::cout);
  }

  // True == pass, False == fail.
  bool unit_test_result = true;

  // Hoisted out of loop for performance.
  chrono::time_point<chrono::system_clock> start, stop;
  ostringstream out;

  RunFixtures();

  const TContext &root_ctx = Context::GetRoot();

  bool success = root_ctx.FailCount;
  TLog(!success) << "passed" << root_ctx.PassCount << ", failed " << root_ctx.FailCount << "\n";
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
