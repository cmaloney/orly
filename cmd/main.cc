#include <cmd/main.h>

#include <iostream>

#include <base/assert_true.h>
#include <base/backtrace.h>

using namespace Base;

int main(int argc, char *argv[]) {
  try {
    TBacktraceCatcher backtrace;
    return Main(argc, argv);
  } catch (const std::exception &ex) {
    std::cout << "EXCEPTION: " << ex.what() << std::endl;
    return -1;
  } catch (...) {
    std::cout << "Unknown exception" << std::endl;
    return -1;
  }
}