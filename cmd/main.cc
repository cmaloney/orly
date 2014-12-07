#include <cmd/main.h>

#include <base/backtrace.h>

int main(int argc, char *argv[]) {
  try {
    TBacktraceCatcher backtrace;
    return GetRunner().Run(argc, argv);
  } catch (const std::exception &ex) {
    cout << "EXCEPTION: " << ex.what() << endl;
    return -1;
  } catch (...) {
    cout << "Unknown exception" << std::endl;
    return -1;
  }
}