#include <base/pump.h>

#include <sys/event.h>

#include <util/error.h>

using namespace Base;
using namespace std;
using namespace Util;

TPump::TPumper::TPumper(TPump &pump) : Pump(pump), Fd(IfLt0(kqueue())) {
  // Add the shutting down fd to the epoll. It'll be the only one with a null pointer associated
  // with it.
  Join(ShutdownFd.GetFd(), Read, nullptr);

  // Launch the background thread.
  Background = thread(&TPumper::BackgroundMain, this);
}

int16_t EventToFilter(TPump::TPumper::TEvent event_type) {
  return int16_t(event_type == TPump::TPumper::Read ? EVFILT_READ : EVFILT_WRITE);
}

void TPump::TPumper::Join(int fd, TEvent event_type, TPipe *pipe) {
  assert(this);
  struct kevent event = {};
  event.ident = unsigned(fd);
  event.flags = EV_ADD;
  event.filter = EventToFilter(event_type);
  event.udata = pipe;

  IfLt0(kevent(Fd, &event, 1, nullptr, 0, nullptr));
}

void TPump::TPumper::Leave(int fd, TEvent event_type) {
  assert(this);
  struct kevent event = {};
  event.ident = unsigned(fd);
  event.filter = EventToFilter(event_type);
  event.flags = EV_DELETE;

  IfLt0(kevent(Fd, &event, 1, nullptr, 0, nullptr));
}

// Returns after the background thread has shut down.
void TPump::TPumper::Shutdown() {
  ShutdownFd.Notify();
  Background.join();
}

void TPump::TPumper::BackgroundMain() {
  assert(this);

  // TODO: Block all signals

  struct kevent events[MaxEventCount] = {};

  bool Stopping = false;

  /* Loop forever, servicing pipes until a stop is requested */
  while(!Stopping) {
    int event_count = kevent(Fd, nullptr, 0, events, MaxEventCount, nullptr);

    if(event_count < 0) {
      if(errno == EINTR) {
        continue;
      } else {
        ThrowSystemError(errno);
      }
    }
    std::unordered_set<TPipe *> dead_pipes;
    for(int i = 0; i < event_count; ++i) {
      TPipe *pipe = static_cast<TPipe *>(events[i].udata);

      // If pipe is null, this is the exit signal.
      if(pipe) {
        // If we've already been deleted, skip.
        if(Contains(dead_pipes, pipe)) {
          continue;
        }

        if(!Pump.ServicePipe(pipe)) {
          dead_pipes.insert(pipe);
        }
      } else {
        Stopping = true;
      }
    }
  }
}
