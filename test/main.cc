/* Runs one unit test. */

#include <cstdlib>
#include <iostream>

#include <base/demangle.h>
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
  Cmd::TArgs<TOptions> args{
      Cmd::Optional({"verbose", "v"},
                    &TOptions::Verbose,
                    "Show the results of unit tests, regardless of whether they pass or fail"),
  };
  parser.Attach(&args, &GetOptionsNonConst());
  ParseWithStandard(parser, ExtractArgVector(args), argc, argv);

  // Context to aggregate all the test results.
  TContext global_ctx(nullptr);

  RunFixtures();

  bool success = global_ctx.GetFailureCount() == 0;
  if(!success || GetOptions().Verbose) {
    std::cout << "passed " << global_ctx.GetPassCount() << ", failed "
              << global_ctx.GetFailureCount() << '\n';
  }

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
