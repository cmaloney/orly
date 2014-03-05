/* <stig/client/test_server.cc>

   Implements <stig/client/test_server.h>.

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

#include <stig/client/test_server.h>

#include <sstream>

#include <syslog.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/timerfd.h>

#include <base/error_utils.h>
#include <base/zero.h>
#include <signal/masker.h>
#include <signal/set.h>

/* Standard namespaces. */
using namespace std;
using namespace chrono;
using namespace placeholders;

/* Library namespaces. */
using namespace Base;
using namespace Signal;
using namespace Socket;

/* Stig namespaces. */
using namespace Stig;
using namespace BinProto;
using namespace Client;

TTestServer::TCmd::TCmd(bool verbose_logging)
    : PortNumber(19380), ConnectionBacklog(8) {
  if (verbose_logging) {
    All = true;
    Echo = true;
  }
}

TTestServer::TCmd::TCmd(int argc, char *argv[])
    : TCmd() {
  Parse(argc, argv, TMeta());
}

TTestServer::TCmd::TMeta::TMeta()
    : TLog::TCmd::TMeta("The Stig test server.") {
  Param(
      &TCmd::PortNumber, "port_number", Optional, "port_number\0pn\0",
      "The port on which we listen clients."
  );
  Param(
      &TCmd::ConnectionBacklog, "connection_backlog", Optional, "connection_backlog\0cb\0",
      "The maximum number of client connection requests to backlog."
  );
}

TTestServer::TTestServer(const TCmd &cmd)
    : Log(cmd), SignalHandlerInstaller(SIGUSR1), Stopping(false),
      AcceptingAddress(TAddress::IPv4Any, cmd.PortNumber) {
  syslog(LOG_INFO, "starting server");
  try {
    /* Open the accepting socket, bind it to our address, and start listening. */
    AcceptingSocket = TFd(socket(AcceptingAddress.GetFamily(), SOCK_STREAM, 0));
    int flag = true;
    IfLt0(setsockopt(AcceptingSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)));
    Bind(AcceptingSocket, AcceptingAddress);
    IfLt0(listen(AcceptingSocket, cmd.ConnectionBacklog));
    /* Launch the acceptor thread. */
    string worker_name;
    /* extra */ {
      ostringstream strm;
      strm << "acceptor on " << AcceptingAddress;
      worker_name = strm.str();
    }
    LaunchWorker(move(worker_name), bind(&TTestServer::AcceptorMain, this, _1));
  } catch (const exception &ex) {
    syslog(LOG_ERR, "error while starting server; %s", ex.what());
    throw;
  }
  syslog(LOG_INFO, "server started");
}

TTestServer::~TTestServer() {
  assert(this);
  syslog(LOG_INFO, "stopping server");
  try {
    /* Setting this flag will prevent new workers from launching and new I/O requests from starting. */
    Stopping = true;
    /* Interrupt all the workers' pending I/O. */
    unique_lock<mutex> lock(WorkersMutex);
    for (const auto &worker: Workers) {
      pthread_kill(worker, SIGUSR1);
    }
    /* Wait for all the workers to exit. */
    auto deadline = system_clock::now() + seconds(2);
    while (!Workers.empty()) {
      if (FewerWorkers.wait_until(lock, deadline) == cv_status::timeout) {
        throw runtime_error("worker(s) failed to stop before deadline");
      }
    }
  } catch (const exception &ex) {
    syslog(LOG_EMERG, "fatal error while stopping server; %s", ex.what());
    abort();
  }
  syslog(LOG_INFO, "server stopped");
}

TTestServer::TConn::TConn(const TTestServer *server, TFd &&client)
    : Server(server), Client(move(client)), Deadline(timerfd_create(CLOCK_MONOTONIC, 0)), DeadlineIsArmed(false) {
  assert(server);
  assert(Client.IsOpen());
  /* Set up the poller with the client (as event 0, flags set later) and the deadline (as event 1, for read only). */
  Poller[0].fd     = Client;
  Poller[1].fd     = Deadline;
  Poller[1].events = POLLIN;
}

void TTestServer::TConn::ArmDeadline(int secs) {
  assert(this);
  itimerspec spec;
  Zero(spec);
  spec.it_value.tv_sec = secs;
  IfLt0(timerfd_settime(Deadline, 0, &spec, nullptr));
  Poller[1].revents = 0;
  DeadlineIsArmed = true;
}

void TTestServer::TConn::DisarmDeadline() {
  assert(this);
  Poller[1].revents = 0;
  DeadlineIsArmed = false;
}

size_t TTestServer::TConn::RecvAtMost(void *buf, size_t size) {
  assert(this);
  assert(buf || !size);
  /* Wait for the client to be read-ready, then try to read some bytes.
     If we read none, or we get an error, throw appropriately. */
  WaitForClient(POLLIN);
  return HandleIoResult(recv(Client, buf, size, 0));
}

void TTestServer::TConn::RecvExactly(void *buf, size_t size) {
  assert(this);
  char *csr = static_cast<char *>(buf);
  while (size) {
    size_t actl = RecvAtMost(csr, size);
    csr  += actl;
    size -= actl;
  }
}

size_t TTestServer::TConn::SendAtMost(const void *buf, size_t size) {
  assert(this);
  assert(buf || !size);
  /* Wait for the client to be write-ready, then try to write some bytes.
     If we write none, or we get an error, throw appropriately. */
  WaitForClient(POLLOUT);
  return HandleIoResult(send(Client, buf, size, MSG_NOSIGNAL));
}

void TTestServer::TConn::SendExactly(const void *buf, size_t size) {
  assert(this);
  const char *csr = static_cast<const char *>(buf);
  while (size) {
    size_t actl = SendAtMost(csr, size);
    csr  += actl;
    size -= actl;
  }
}

void TTestServer::TConn::WaitForClient(int flags) {
  assert(this);
  /* If the server is stopping, don't even start this I/O. */
  if (Server->Stopping) {
    throw TInterrupted();
  }
  /* Set up the poller loop to wait for the client.
     NOTE: The poller may already have a deadline notification in it. */
  Poller[0].events  = flags;
  Poller[0].revents = 0;
  do {
    /* If the deadline has passed, throw. */
    if (Poller[1].revents) {
      throw TTimedOut();
    }
    /* Wait for the client to be ready or, if the deadline is armed, for the deadline to pass.
       If we encounter an error, throw appropriately. */
    if (poll(Poller, DeadlineIsArmed ? 2 : 1, -1) < 0) {
      if (errno == EINTR) {
        throw TInterrupted();
      }
      ThrowSystemError(errno);
    }
  } while (!Poller[0].revents);
}

size_t TTestServer::TConn::HandleIoResult(ssize_t actl) {
  /* If we transferred no bytes, we can assume the client has disconnected. */
  if (actl == 0) {
    throw TDisconnected();
  }
  /* If we got an error, throw something meaningful. */
  if (actl < 0) {
    switch (errno) {
      case EINTR: {
        throw TInterrupted();
      }
      case EPIPE: {
        throw TDisconnected();
      }
      default: {
        ThrowSystemError(errno);
      }
    }
  }
  /* Return the (non-zero) number of bytes transferred. */
  return actl;
}

void TTestServer::AcceptorMain(const string &worker_name) {
  assert(this);
  assert(&worker_name);
  /* Loop here, accepting connections and launching sessions, until the server begins stopping. */
  while (!Stopping) {
    syslog(LOG_INFO, "%s; waiting for connection", worker_name.c_str());
    TAddress client_address;
    socklen_t len = TAddress::MaxLen;
    int client_fd = accept(AcceptingSocket, client_address, &len);
    if (client_fd >= 0) {
      /* We have a connection.  Put the client's fd in a safe place and verify that we got a good IP address. */
      TFd client(client_fd);
      client_address.Verify();
      /* Launch a thread to talk to the client. */
      string conn_name;
      /* extra */ {
        ostringstream strm;
        strm << "connection to " << client_address;
        conn_name = strm.str();
      }
      syslog(LOG_INFO, "%s; launching %s", worker_name.c_str(), conn_name.c_str());
      LaunchWorker(move(conn_name), bind(&TTestServer::ConnectionMain, this, _1, move(client)));
    } else if (errno != EINTR) {
      /* We have an error other than an interruption by a signal. */
      ThrowSystemError(errno);
    }
  }
}

void TTestServer::ConnectionMain(const string &worker_name, TFd &client) {
  assert(this);
  assert(&worker_name);
  /* Make an object to manage the I/O over this connection,
     then give the client a few tries to establish a session. */
  TConn conn(this, move(client));
  char buf[8192];
  while (!Stopping) {
    /* Read a message header from the client. */
    conn.ArmDeadline();
    syslog(LOG_INFO, "%s; waiting for client", worker_name.c_str());
    size_t size = conn.RecvAtMost(buf, sizeof(buf));
    syslog(LOG_INFO, "%s; received %ld byte(s)", worker_name.c_str(), size);
    if (size < sizeof(TMsgHdr)) {
      syslog(LOG_INFO, "%s; too short to be a message header; hanging up", worker_name.c_str());
      return;
    }
    char *start = buf;
    TMsgHdr::TByteOrder byte_order;
    TMsgHdr *msg_hdr = TMsgHdr::TryDecode(start, byte_order);
    if (!msg_hdr) {
      syslog(LOG_INFO, "%s; bad message header byte order; hanging up", worker_name.c_str());
      return;
    }
    /* Act on the message header. */
    syslog(
        LOG_INFO, "%s; message header; byte_order = '%c', kind = '%c', request id = %d, size = %d",
        worker_name.c_str(), byte_order, msg_hdr->GetKind(), msg_hdr->GetRequestId(), msg_hdr->GetSize());
    switch (msg_hdr->GetKind()) {
      case TMsgHdr::TKind::Request: {
        auto request_id = msg_hdr->GetRequestId();
        auto msg_size   = msg_hdr->GetSize();
        start = buf;
        TMsgHdr::Encode(start, byte_order, TMsgHdr::TKind::NormalReply, request_id, msg_size);
        conn.ArmDeadline();
        conn.SendExactly(buf, size);
        size_t sent_size = size - sizeof(TMsgHdr);
        while (sent_size < msg_size) {
          conn.ArmDeadline();
          size = conn.RecvAtMost(buf, min(msg_size - sent_size, sizeof(buf)));
          conn.ArmDeadline();
          conn.SendExactly(buf, size);
          sent_size += size;
        }
        break;
      }
      case TMsgHdr::TKind::NormalReply:
      case TMsgHdr::TKind::ErrorReply: {
        break;
      }
      default: {
        syslog(LOG_EMERG, "%s; bad message header kind; hanging up", worker_name.c_str());
        return;
      }
    }
  }
}

void TTestServer::LaunchWorker(string &&worker_name, function<void (const string &)> &&entry_point) {
  assert(this);
  assert(&worker_name);
  assert(&entry_point);
  /* If we're stopping, throw. */
  if (Stopping) {
    throw TInterrupted();
  }
  try {
    /* Mask out all signals except for SIGUSR1.  The thread we are about to launch will
       inherit this as its mask, making sure its I/O operations can be interrupted when
       the server tries to stop. */
    TMasker masker(*TSet(TSet::Exclude, { SIGUSR1 }));
    /* Launch a new worker and wait for it to start. */
    unique_lock<mutex> lock(WorkersMutex);
    auto old_size = Workers.size();
    thread worker(&TTestServer::WorkerWrapper, this, move(worker_name), move(entry_point));
    while (Workers.size() <= old_size) {
      MoreWorkers.wait(lock);
    }
    /* Set the worker free. */
    worker.detach();
  } catch (const exception &ex) {
    syslog(LOG_EMERG, "fatal error launching %s; %s", worker_name.c_str(), ex.what());
    abort();
  }
}

void TTestServer::WorkerWrapper(const string &worker_name, const function<void (const string &)> &entry_point) {
  assert(this);
  assert(&worker_name);
  assert(&entry_point);
  assert(entry_point);
  /* Cache the handle to the thread we're in. */
  auto self = pthread_self();
  try {
    /* Insert ourself in the set of workers. */
    unique_lock<mutex> lock(WorkersMutex);
    if (!Workers.insert(self).second) {
      throw runtime_error("worker already in set");
    }
    /* Notify our observer that we have started. */
    MoreWorkers.notify_one();
  } catch (const exception &ex) {
    syslog(LOG_EMERG, "%s; fatal error while starting; %s", worker_name.c_str(), ex.what());
    abort();
  }
  syslog(LOG_INFO, "%s; started", worker_name.c_str());
  try {
    /* Call the entry point. */
    entry_point(worker_name);
  } catch (const TDisconnected &) {
    syslog(LOG_INFO, "%s; disconnected", worker_name.c_str());
  } catch (const TInterrupted &) {
    syslog(LOG_INFO, "%s; interrupted", worker_name.c_str());
  } catch (const TTimedOut &) {
    syslog(LOG_INFO, "%s; timed out", worker_name.c_str());
  } catch (const exception &ex) {
    syslog(LOG_ERR, "%s; error; %s", worker_name.c_str(), ex.what());
  }
  syslog(LOG_INFO, "%s; stopping", worker_name.c_str());
  try {
    /* Remove ourself from the set of workers. */
    unique_lock<mutex> lock(WorkersMutex);
    if (!Workers.erase(self)) {
      throw runtime_error("worker not in set");
    }
    /* Notify our observer that we're gone.
       NOTE: Our observer might be the destructor of TTestServer; therefore,
       as soon as we release our lock, we must consider 'this' to be a bad pointer. */
    FewerWorkers.notify_one();
  } catch (const exception &ex) {
    syslog(LOG_EMERG, "%s; fatal error while stopping; %s", worker_name.c_str(), ex.what());
    abort();
  }
}
