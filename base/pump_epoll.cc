#include <base/pump_epoll.h>

#include <base/pump.h>

using namespace std;
using namespace Base;
using namespace Base::Pump;
using namespace Util;

TPumper::TPumper() : Epoll(Util::IfLt0(epoll_create1(EPOLL_CLOEXEC))) {
  // Add the shutting down fd to the epoll. It'll be the only one with a null pointer associated with it.
  Join(ShutdownFd.GetFd(), Read, nullptr);

  // Launch the background thread.
  Background = std::thread(&TPumper::BackgroundMain, this);
}

void TPumper::Join(int fd, TEvent event_type, TPipe *pipe) {
  assert(this);
  epoll_event event;
  Zero(event);
  switch(event_type) {
    case Read:
      event.events = EPOLLIN;
      break;
    case Write:
      event.events = EPOLLOUT;
      break;
  }
  event.data.ptr = pipe;
  Util::IfLt0(epoll_ctl(Epoll, EPOLL_CTL_ADD, fd, &event));
}

void TPumper::Leave(int fd) {
  assert(this);
  Util::IfLt0(epoll_ctl(Epoll, EPOLL_CTL_DEL, fd, nullptr));
}

// Returns after the background thread has shut down.
void TPumper::Shutdown() {
  ShutdownFd.Notify();
  Background.join();
}

void TPumper::BackgroundMain() {
  assert(this);

  // TODO: Block all signals

  epoll_event events[MaxEventCount];

  bool Stopping = false;

  /* Loop forever, servicing pipes until a stop is requested */
  while (!Stopping) {
    int event_count = epoll_wait(Epoll, events, MaxEventCount, -1);
    if (event_count < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        Util::ThrowSystemError(errno);
      }
    }
    std::unordered_set<TPipe *> dead_pipes;
    for (int i = 0; i < event_count; ++i) {
      TPipe *pipe = static_cast<TPipe *>(events[i].data.ptr);

      // If pipe is null, this is the exit signal.
      if (pipe) {
        // If we've already been deleted, skip.
        if (Contains(dead_pipes, pipe)) {
          continue;
        }

        if (!pipe->Service()) {
          // The pipe is dead. Remove it from our set of pipes, ping PipeDied.
          dead_pipes.insert(pipe);
          std::lock_guard<mutex> lock(PipeMutex);
          Pipes.erase(pipe);
          PipeDied.notify_all();
          delete pipe;
        }
      } else {
        Stopping = true;
      }
    }
  }
}

  /* Pushed in the destructor.  It causes the background thread to exit. */
  TNotifyFd ShutdownFd;
  TFd Epoll;
  std::thread Background;
};

} // Pump
} // Base
