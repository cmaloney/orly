/* The epoll against which the background thread blocks. */
#pragma once

#include <thread>

#include <base/fd.h>
#include <base/notify_fd.h>

namespace Base {
namespace Pump {

class TPipe;

//TODO(cmaloney): Make one pumper with two impls in different CCs?
//TODO: This should use a more generic event wrapper / libevent
class TPumper {

  /* Max number of epoll events returned simultaneously */
  static const size_t MaxEventCount = 64;

  TPumper();
  void Join(int fd, TEvent event_type, TPipe *pipe);
  void Leave(int fd);

  // Returns after the background thread has shut down.
  void Shutdown();

  private:
  void BackgroundMain();

  /* Pushed in the destructor.  It causes the background thread to exit. */
  TNotifyFd ShutdownFd;
  TFd Epoll;
  std::thread Background;
};

} // Pump
} // Base
