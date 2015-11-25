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

// Link against the platform-specific TPumper implementation.
#pragma clang diagnostic ignored "-Wundef"
#if __APPLE__
#include <base/pump_kqueue.h>
#else
#include <base/pump_epoll.h>
#endif

#include <util/error.h>
#include <util/io.h>

// TMP
#include <iostream>
#include <thread>


using namespace Base;
using namespace std;
using namespace Util;

class TPump::TPipe {
  NO_COPY(TPipe)

  public:
  enum class TDirection { WriteToBuffer, ReadFromBuffer };

  // Construct a pipe which connects a cyclic buffer to an fd, with data flowing
  // in the direction specified by Direction.
  TPipe(TPump *pump, TDirection direction, TFd &act_fd, std::shared_ptr<TCyclicBuffer> &buffer)
      : Buffer(buffer), Pump(pump), Direction(direction) {
    assert(pump);
    assert(&act_fd);
    assert(act_fd);
    assert(&buffer);
    assert(buffer);

    switch (direction) {
      case TDirection::WriteToBuffer:
        // Pipe output from an fd into a cyclic buffer where the caller will
        // eventually read from.
        TFd::Pipe(Fd, act_fd);
        break;
      case TDirection::ReadFromBuffer:
        // Pipe input from the buffer where the user data is to the fd which the
        // caller will read from.
        TFd::Pipe(act_fd, Fd);
        break;
    }

    SetNonBlocking(Fd);

    assert(Fd);
  }

  ~TPipe() {
    if (Working) {
      Stop();
    }
  }

  // Pipes data to / from the buffer as needed.
  // Returns true iff more processing will be needed. Returns false if no future
  // work to be done.
  bool Service() {
    assert(this);

    switch (Direction) {
      case TDirection::WriteToBuffer: {
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
      case TDirection::ReadFromBuffer: {
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
          return false;
        } else {
          return true;
        }
      }
    }
  }

  private:
  void Start() {
    assert(this);
    assert(!Working);

    // Mark as working then make it so we can start working.
    Working = true;

    switch (Direction) {
      case TDirection::WriteToBuffer:
        Pump->Pumper.Join(Fd, TPumper::Read, this);
        break;
      case TDirection::ReadFromBuffer:
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
      case TDirection::WriteToBuffer:
        // Read Fd can only be stopped once ever. Will be stopped on Pump
        // destruction or lack of data.
        assert(Fd.IsOpen());
        Pump->Pumper.Leave(Fd, TPumper::Read);
        Fd.Reset();
        break;
      case TDirection::ReadFromBuffer:
        // Write Fd can be stopped as much as needed as the buffer behind the
        // writing fd fills.
        Pump->Pumper.Leave(Fd, TPumper::Write);
        break;
    }

    Working = false;
  }

  // Storage for the data.
  std::shared_ptr<TCyclicBuffer> Buffer;

  // Pointer to the pump to enable self registration / deregistration.
  // TODO(cmaloney): Make the pump own pipe registration with a callback
  // for explicitly leaving.
  TPump *Pump;

  // Direction of the pump.
  const TDirection Direction;

  // End of the pipe for this program to pump to / from in order to move the
  // data from the cyclic buffer to the target.
  TFd Fd;

  // True when the pipe is in the event loop / can get events.
  bool Working = false;

  friend class Base::TPump;
};

TFd TPump::NewReadFromBuffer(std::shared_ptr<TCyclicBuffer> &source) {
  TFd fd;
  std::lock_guard<mutex> lock(PipeMutex);
  TPipe *pipe = new TPipe(this, TPipe::TDirection::ReadFromBuffer, fd, source);
  Pipes.insert(pipe);
  pipe->Start();

  return fd;
}

TFd TPump::NewWriteToBuffer(std::shared_ptr<TCyclicBuffer> &source) {
  TFd fd;
  std::lock_guard<mutex> lock(PipeMutex);
  TPipe *pipe = new TPipe(this, TPipe::TDirection::WriteToBuffer, fd, source);
  Pipes.insert(pipe);
  pipe->Start();

  return fd;
}

void TPump::WaitForIdle() const {
  assert(this);
  std::unique_lock<std::mutex> lock(PipeMutex);
  while (!Pipes.empty()) {
    PipeDied.wait(lock);
  }
}

TPump::TPump() : Pumper(*this) {}

TPump::~TPump() {
  Pumper.Shutdown();

  std::lock_guard<mutex> lock(PipeMutex);
  for (TPipe *pipe : Pipes) {
    delete pipe;
  }
  PipeDied.notify_all();
}

bool TPump::ServicePipe(TPipe *pipe) {
  assert(this);

  if (!pipe->Service()) {
    // The pipe is dead. Remove it from our set of pipes, ping PipeDied.
    std::lock_guard<mutex> lock(PipeMutex);
    Pipes.erase(pipe);
    PipeDied.notify_all();
    delete pipe;
    return false;
  }
  return true;
}
