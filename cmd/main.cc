#include <cmd/main.h>

#include <iostream>

#include <cmd/util.h>
#include <base/assert_true.h>
#include <base/backtrace.h>

using namespace Base;

int main(int argc, char *argv[]) {
  try {
    TBacktraceCatcher backtrace;
    return Main(argc, argv);
  } catch(const Cmd::TCleanExit &) {
    // Clean exit via exception stack unwinding.
    return 0;
  } catch(const Cmd::TErrorExit &ex) {
    // Error code exit via exception stack unwinding.
    std::cerr << "ERROR: " << ex.Message << std::endl;
    return ex.ExitCode;
  } catch(const std::exception &ex) {
    std::cout << "EXCEPTION: " << ex.what() << std::endl;
    return -1;
  } catch(...) {
    std::cout << "Unknown exception" << std::endl;
    return -1;
  }
}
