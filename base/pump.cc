/* <base/pump.cc>

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/pump.h>

#include <unistd.h>

#include <future>
#include <iostream>

// Link against the platform-specific TPumper implementation.
#ifdef __APPLE__
#include <base/pump_kqueue.h>
#else
#include <base/pump_epoll.h>
#endif

#include <util/error.h>
#include <util/io.h>

using namespace Base;
using namespace std;
using namespace Util;


class TPump::TPipe {
  NO_COPY(TPipe)

  public:
  // Construct a pipe which connects a cyclic buffer to an fd, with data flowing
  // in the direction specified by Direction.
  TPipe(TPump *pump, TPipeDirection direction, TFd &&act_fd, shared_ptr<TCyclicBuffer> &buffer)
      : Buffer(buffer), Direction(direction), Fd(move(act_fd)), Pump(pump) {
    assert(pump);
    assert(&buffer);
    assert(buffer);

    SetNonBlocking(Fd);

    assert(Fd);
  }

  ~TPipe() {
    if (Working) {
      Stop();
    }
    if (!Stopped) {
      Stopped = true;
      Finished.set_value();
    }
  }

  // Pipes data to / from the buffer as needed.
  // Returns true iff more processing will be needed. Returns false if no future
  // work to be done.
  bool Service() {
    assert(this);

    switch (Direction) {
      case TPipeDirection::WriteToBuffer: {
        ssize_t read_size = Buffer->WriteTo(Fd);
        if (read_size == -1) {  // Error.
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Do-nothing. We just didn't read data.
          } else {
            ThrowSystemError(errno);
          }
        } else if (read_size == 0) {  // EOF. Done reading.
          Stop();
          return false;
        } else {
          // Read data successfully, keep on reading.
        }

        return true;
      }
      case TPipeDirection::ReadFromBuffer: {
        ssize_t write_size = Buffer->ReadFrom(Fd);
        if (write_size == -1) {  // Error.
          if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            // Do-nothing. We just didn't write data.
            return true;
          } else {
            // Done writing since hit an unknown error.
            Stop();
            ThrowSystemError(errno);
          }
        }

        // If all bits have been written from the buffer, then be done.
        if (Buffer->IsEmpty()) {
          Stop();
          assert(!Stopped);
          Stopped = true;
          Finished.set_value();
          return false;
        } else {
          return true;
        }
      }
    }
  }

  future<void> GetFinishedFuture() { return Finished.get_future(); }

  private:
  void Start() {
    assert(this);
    assert(!Working);

    // Mark as working then make it so we can start working.
    Working = true;

    switch (Direction) {
      case TPipeDirection::WriteToBuffer:
        Pump->Pumper.Join(Fd, TPumper::Read, this);
        break;
      case TPipeDirection::ReadFromBuffer:
        Pump->Pumper.Join(Fd, TPumper::Write, this);
        break;
    }
  }

  void Stop() {
    assert(this);
    assert(Working);

    // Leave the pump making it so the pipe can't possibly be serviced then
    // mark as stopped

    switch (Direction) {
      case TPipeDirection::WriteToBuffer:
        // Read Fd can only be stopped once ever. Will be stopped on Pump
        // destruction or lack of data.
        assert(Fd.IsOpen());
        Pump->Pumper.Leave(Fd, TPumper::Read);
        Fd.Reset();

        assert(!Stopped);
        Stopped = true;
        Finished.set_value();
        break;
      case TPipeDirection::ReadFromBuffer:
        // Write Fd can be stopped as much as needed as the buffer behind the
        // writing fd fills.
        Pump->Pumper.Leave(Fd, TPumper::Write);
        break;
    }

    Working = false;
  }

  // Storage for the data.
  shared_ptr<TCyclicBuffer> Buffer;

  // Direction of the pump.
  const TPipeDirection Direction;

  // End of the pipe for this program to pump to / from in order to move the
  // data from the cyclic buffer to the target.
  TFd Fd;

  // Pointer to the pump to enable self registration / deregistration.
  // TODO(cmaloney): Make the pump own pipe registration with a callback
  // for explicitly leaving.
  TPump *Pump;

  // True when the pipe is in the event loop / can get events.
  bool Working = false, Stopped = false;

  friend class Base::TPump;
  promise<void> Finished;
};

future<void> TPump::AddPipe(TPipeDirection direction,
                            TFd &&fd,
                            shared_ptr<TCyclicBuffer> &buffer) {
  TPipe *pipe = new TPipe(this, direction, move(fd), buffer);
  auto future = pipe->GetFinishedFuture();

  /* Find an empty slot in the pipes container for the pipe and start the pipe running. */
  uint64_t start = uint64_t(pipe) % MaxPipes;
  TPipe *nullPipe = nullptr;
  for(uint64_t i = 0; i < MaxPipes; ++i) {
    if (i > MaxPipes - 10) {
      std::cout << "WARNING: Pump MaxPipes is being approached. Increase MaxPipes or things will crash\n";
    }
    if (Pipes[(start + i) % MaxPipes].compare_exchange_strong(nullPipe, pipe)) {
      break;
    }
  }
  ++PipeCount;
  pipe->Start();
  return future;
}

TFd TPump::NewReadFromBuffer(shared_ptr<TCyclicBuffer> &source) {
  // Make a pipe so the user can operate on one end then the pump will pump data
  // out of the buffer and into the write end.
  TFd caller_fd, pump_fd;
  tie(caller_fd, pump_fd) = TFd::Pipe();
  AddPipe(TPipeDirection::ReadFromBuffer, move(pump_fd), source);
  return caller_fd;
}

tuple<TFd, future<void>> TPump::NewWriteToBuffer(shared_ptr<TCyclicBuffer> &target) {
  // Make a pipe so the user can operate on one end then the pump will read data
  // from the other end and pump it into the bufer.
  TFd caller_fd, pump_fd;
  tie(pump_fd, caller_fd) = TFd::Pipe();
  auto future = AddPipe(TPipeDirection::WriteToBuffer, move(pump_fd), target);
  return make_tuple(move(caller_fd), move(future));
}

void TPump::WaitForIdle() const {
  assert(this);
  unique_lock<mutex> lock(PipeMutex);
  while (PipeCount > 0) {
    PipeDied.wait(lock);
  }
}

TPump::TPump() : PipeCount(0), Pumper(*this) {
  for (uint64_t i = 0; i < MaxPipes; ++i) {
    Pipes[i] = nullptr;
  }
}

TPump::~TPump() {
  Pumper.Shutdown();

  lock_guard<mutex> lock(PipeMutex);
  for (TPipe *pipe : Pipes) {
    delete pipe;
  }
  PipeDied.notify_all();
}

bool TPump::ServicePipe(TPipe *pipe) {
  assert(this);

  // If the pipe is still active, say we serviced it and be done.
  if (pipe->Service()) {
    return true;
  }

  // The pipe is dead, so remove it from the set of pipes then notify PipeDied of it's
  // death.
  // Find the pipe and remove it from the set of pipes.
  uint64_t start = uint64_t(pipe) % MaxPipes;
  for(uint64_t i = 0; i < MaxPipes; ++i) {
    if (i > MaxPipes - 10) {
      std::cout << "WARNING: Pump MaxPipes is being approached. Increase MaxPipes or things will crash\n";
    }
    if (Pipes[(start + i) % MaxPipes].compare_exchange_strong(pipe, nullptr)) {
      // Removed. Ping PipeDied
      --PipeCount;
      PipeDied.notify_all();
      delete pipe;
      return false;
    }
  }

  THROWER(std::logic_error) << "Internal consistency error: Couldn't find pipe which was running to remove. That's odd...";
}
