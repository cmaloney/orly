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
  // TODO(cmaloney): Fixtures should be runnable as a group
  for(const TFixture *fixture : GetFixtures()) {
    // TODO(cmaloney): Check/test that we're only in one thread, as little other context
    // is around.

    // Group results by this fixture.
    TContext context(fixture->Name);

    // Run the fixture
    try {
      fixture->Func();

    } catch(const exception &ex) {
      TContext::Log(false) << "exception"
                           << "(" << Demangle(typeid(ex)).get() << ")"
                           << ": " << ex.what();
    } catch(...) {
      TContext::Log(false) << "unknown exception";
    }
  }
}

int Main(int argc, char *argv[]) {
  Cmd::TParser parser;
  parser.Attach(&GetArgs(), &GetOptionsNonConst());
  ParseWithStandard(parser, ExtractArgVector(GetArgs()), argc, argv);

  // Context to aggregate all the test results.
  TContext global_ctx(nullptr);

  RunFixtures();

  bool success = global_ctx.GetFailureCount();
  if(!success || GetOptions().Verbose) {
    std::cout << "passed " << global_ctx.GetPassCount() << ", failed " << global_ctx.GetFailureCount();
  }

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
