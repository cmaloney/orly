/* <stig/indy/failover.cc>

   Implements <stig/indy/failover.h>.

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/indy/failover.h>

#include <base/debug_log.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Stig::Indy;
using namespace Rpc;

TConnectionFailed::TConnectionFailed(const string &error_msg)
    : runtime_error(error_msg) {}

TCommonContext::~TCommonContext() {}

void TCommonContext::Shutdown() {
  IfLt0(shutdown(Device->GetFd(), SHUT_WR));
}

TCommonContext::TCommonContext(const Rpc::TProtocol &protocol,
                               const TFd &fd)
    : TContext(protocol),
      Fd(fd),
      ReaderState(Connected),
      WorkerState(Running) {
  Device = std::make_shared<TDevice>(Fd);
  BinaryIoStream = std::make_shared<TBinaryIoStream>(Device);
}

bool TCommonContext::Queue() {
  assert(this);
  switch (ReaderState) {
    case Connected : {
      switch (WorkerState) {
        case Running : {
          std::shared_ptr<const TAnyRequest> request;
          try {
            //std::cout << "Read() request" << std::endl;
            request = Read();
            //std::cout << "DONE Read() request" << std::endl;
          } catch(const std::system_error &err) {
            if (WasInterrupted(err)) {
              DEBUG_LOG("Server shutting down.");
              FailAllFutures("Server shutting down.");
              throw;
            } else {
              syslog(LOG_INFO, "TCommonContext::Queue() : Connection Failed. system error code [%d] [%s]", err.code().value(), err.what());
              FailAllFutures("TCommonContext::Queue() : Connection Failed.");
              ReaderState = NotConnected;
            }
          } catch (const std::exception &ex) {
            syslog(LOG_INFO, "TCommonContext::Queue() : Connection Failed. [%s]", ex.what());
            FailAllFutures("TCommonContext::Queue() : Connection Failed.");
            ReaderState = NotConnected;
          }
          if (request) {
            std::lock_guard<std::mutex> lock(Mutex);
            RequestQueue.push(request);
            //std::cout << "Sem Push()" << std::endl;
            Sem.Push();
            //std::cout << "FINISH Sem Pushed()" << std::endl;
          }
          break;
        }
        case Exited : {
          ReaderState = NotConnected;
          break;
        }
        case ExitedOnWriteFailure : {
          ReaderState = NotConnected;
          break;
        }
      }
      break;
    }
    case NotConnected : {
      break;
    }
  }
  return ReaderState == Connected;
}

bool TCommonContext::Work() {
  std::shared_ptr<const Rpc::TAnyRequest> request;

  /*acquire Mutex */ {
    shared_ptr<unique_lock<mutex>> lock_ptr(make_shared<unique_lock<mutex>>(Mutex));
    for (;;) {
      if (ReaderState == Connected && RequestQueue.empty()) {
        lock_ptr.reset();
        Sem.Pop();
        lock_ptr = make_shared<unique_lock<mutex>>(Mutex);
      } else {
        break;
      }
    }
    switch (ReaderState) {
      case Connected : {
        assert(!RequestQueue.empty());
        request = RequestQueue.front();
        RequestQueue.pop();
        break;
      }
      case NotConnected : {
        break;
      }
    }
  } // release Mutex
  bool ret = ReaderState == Connected;
  if (request) {
    try {
      (*request)();
    } catch(const std::system_error &err) {
      if (WasInterrupted(err)) {
        DEBUG_LOG("Server shutting down.");
        FailAllFutures("Server shutting down.");
        throw;
      } else {
        syslog(LOG_INFO, "TCommonContext::Work() : Connection Failed. system error code [%d] [%s]", err.code().value(), err.what());
        lock_guard<mutex> lock(WorkerMutex);
        WorkerState = ExitedOnWriteFailure;
        WorkerStateChanged.Push();
      }
    } catch (...) {
      syslog(LOG_INFO, "TCommonContext::Work() : Connection Failed.");
      FailAllFutures("TCommonContext::Work() : Connection Failed.");
      lock_guard<mutex> lock(WorkerMutex);
      WorkerState = ExitedOnWriteFailure;
      WorkerStateChanged.Push();
    }
  }
  if (!ret) {
    lock_guard<mutex> lock(WorkerMutex);
    WorkerState = Exited;
    WorkerStateChanged.Push();
  }
  return ret;
}

void TCommonContext::Join() {
  DEBUG_LOG("ENTER TCommonContext::Join()");
  try {
    switch (ReaderState) {
      case Connected : {
        assert(false);
        throw;
      }
      case NotConnected : {
        shared_ptr<unique_lock<mutex>> lock_ptr(make_shared<unique_lock<mutex>>(WorkerMutex));
        switch (WorkerState) {
          case Running : {
            Sem.Push();
            //std::cout << "Waiting to Pop()" << std::endl;
            lock_ptr.reset();
            WorkerStateChanged.Pop();
            //std::cout << "Finished Pop()" << std::endl;
            break;
          }
          case Exited : {
            break;
          }
          case ExitedOnWriteFailure : {
            break;
          }
        }
        break;
      }
    }
  } catch (const std::exception &ex) {
    syslog(LOG_ERR, "TCommonContext::Join() caught error [%s]", ex.what());
    throw;
  }
  DEBUG_LOG("EXIT TCommonContext::Join()");
}

TMasterContext::TMasterContext(const TFd &fd)
    : TCommonContext(TMasterContext::TProtocol::Protocol, fd) {}

TMasterContext::~TMasterContext() {}

TSlaveContext::TSlaveContext(const TFd &fd)
    : TCommonContext(TSlaveContext::TProtocol::Protocol, fd) {}

TSlaveContext::~TSlaveContext() {}