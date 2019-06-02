#include <base/subprocess_coro.h>

#include <sys/epoll.h>
#include <unistd.h>

#include <array>
#include <tuple>

#include <base/zero.h>
#include <util/error.h>

using namespace Base;
using namespace Base::Subprocess;
using namespace std;
using namespace Util;

constexpr int MaxEpollEventCount = 128;
constexpr int ReadBlockSize = 4096;


void Base::Subprocess::Exec(const vector<string> &cmd) {
  assert(&cmd);

  // TODO(cmaloney): de-dup this between here and dep_clang.cc::GetDepsUsingClang.
  // NOTE: This is definitely less efficient than strictly necessary, but we
  // don't actually copy the strings, just pointers, so not too bad.
  unique_ptr<const char *[]> argv(new const char *[cmd.size() + 1]);
  argv[cmd.size()] = nullptr;
  for (uint64_t i = 0; i < cmd.size(); ++i) {
    argv[i] = cmd[i].c_str();
  }
  // NOTE: const_cast is unsafe. In this case though, we're going out of existence, so who cares.
  IfLt0(execvp(cmd[0].c_str(), const_cast<char **>(argv.get())));
  throw;
}

TProcessHandle::TProcessHandle(const vector<string> &cmd) {
  TFd  parent_write_to_child, child_read_input, child_write_output, child_write_error;
  tie(child_read_input, parent_write_to_child) = TFd::Pipe();
  tie(Output, child_write_output) = TFd::Pipe();
  tie(Error, child_write_error) = TFd::Pipe();
 
  IfLt0(ChildId = fork());
  if (!ChildId) {
    // Soon to be child process. Duplicate the pipes into their normal fd numbers
    // And close the originals so we don't leave the child with FDs it doesn't expect.
    IfLt0(dup2(child_read_input, 0));
    IfLt0(dup2(child_write_output, 1));
    IfLt0(dup2(child_write_error, 2));
    child_read_input.Reset();
    child_write_output.Reset();
    child_write_error.Reset();
    Exec(cmd);
  }
}

TProcessHandle::~TProcessHandle() {}

// TODO(cmaloney): Move to io_uring / liburing
TTaggedBlockGenerator TProcessHandle::Communicate() const {
  assert(this);
  // Create a new epoll which has stdout, stderr in it. Loop until both stdout and stderr are
  // closed. co_yield a block every time there is an epoll event.
  TFd epoll_fd(Util::IfLt0(epoll_create1(EPOLL_CLOEXEC)));

  // Add Output channel
  epoll_event event;
  Zero(event);
  event.events = EPOLLIN;
  event.data.ptr = const_cast<TFd*>(&Output);
  IfLt0(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, Output, &event));
  // Add Error channel
  event.data.ptr = const_cast<TFd*>(&Error);
  IfLt0(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, Error, &event));

  // TOOD(cmaloney): Some way to trigger a clean exit / subprocess kill?
  epoll_event events[MaxEpollEventCount];
  array<uint8_t, ReadBlockSize> read_buffer;
  while(true) {
    int event_count = epoll_wait(epoll_fd, events, MaxEpollEventCount, -1);
    if(event_count < 0) {
      if(errno == EINTR) {
        continue;
      } else {
        ThrowSystemError(errno);
      }
    }
    int closed_count = 0;
    // read from the particular fd
    for(int i = 0; i < event_count; ++i) {
      ssize_t read_size = read(events[i].data.fd, read_buffer.data(), ReadBlockSize);
      // Successful read, yield a block
      if (read_size > 0) {
        co_yield TTaggedBlock {
          .Kind=events[i].data.ptr == &Error ? TBlockKind::Error: TBlockKind::Output,
          .Bytes=basic_string_view<uint8_t>(read_buffer.data(), static_cast<unsigned long>(read_size))
        };
        continue;
      }
      
      // EOF. Add to closed count, if all watched fds are EOF/closed, then exit.
      if (read_size == 0) {
        closed_count += 1;
        if (closed_count == 2) {
          break;
        }
        // Remove the FD from the epoll
        Util::IfLt0(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr));
      }

      // Error occured. read_size should be -1, but make sure we don't fail hard if it isn't.
      assert(read_size < 0);
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Do-nothing, didn't read any data, but also no hard error.
        continue;
      }
      // Hard error, report to caller.
      ThrowSystemError(errno);

    }
  }
}
