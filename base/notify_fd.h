/* An fd which can be easily used to wakeup waiting threads */

#include <base/class_traits.h>
#include <base/fd.h>
#include <util/io.h>

namespace Base {

class TNotifyFd {
  NO_COPY(TNotifyFd);
  public:
  TNotifyFd() {
    TFd::Pipe(Read, Write);
  }

  const TFd &GetFd() const {
    return Read;
  }

  void Notify() {
    char data[8] = {};
    Util::WriteExactly(Write, data, 8);
  }

  private:
  TFd Read, Write;

};

}