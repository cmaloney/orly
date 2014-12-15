/* Include this file to get a main function injected into your program

Uses well-defined globals to build an exception-catching entrypoint to
a program which will parse arguments.


Expects one global function:
unique_ptr<TRunner> GetRunner();

TRunner bundles any argument processing along with the C++ entrypoint
which takes configuration parameters as an object. */

#include <memory>

int Main(int argc, char *argv[]);
