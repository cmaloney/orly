/* kqueue reader/writer. */
#pragma once

#include <thread>

#include <base/fd.h>
#include <base/notify_fd.h>
#include <base/pump_util.h>

namespace Base {
  class TPump;
namespace Pump {

class TPipe;

//TODO(cmaloney): Make one pumper with two impls in different CCs?
//TODO: This should use a more generic event wrapper / libevent
class TPumper {
  public:
  /* Max number of epoll events returned simultaneously */
  static const size_t MaxEventCount = 64;

  TPumper(TPump &pump);

  void Join(int fd, TEvent event_type, TPipe *pipe);

  void Leave(int fd, TEvent event_type);

  // Returns after the background thread has shut down.
  void Shutdown();

  private:
  void BackgroundMain();

  /* Pushed in the destructor.  It causes the background thread to exit. */
  // TODO(cmaloney): pump really shouldn't be needed.
  TPump &Pump;
  TNotifyFd ShutdownFd;
  TFd Kqueue;
  std::thread Background;
};

} // Pump
} // Base
