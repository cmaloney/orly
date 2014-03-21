/* <stig/client/client.cc>

   Implements <stig/client/client.h>.

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

#include <stig/client/client.h>

#include <iomanip>  // TODO
#include <memory>

#include <poll.h>
#include <sys/socket.h>

#include <base/debug_log.h>
#include <base/io_utils.h>
#include <base/no_default_case.h>
#include <io/binary_output_only_stream.h>
#include <io/device.h>
#include <io/recorder_and_player.h>
#include <stig/protocol.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Io;
using namespace Rpc;
using namespace Socket;
using namespace Stig;
using namespace Stig::Client;
using namespace Stig::Handshake;

TClient::TClient(const TAddress &server_address, const TOpt<TUuid> &session_id, const seconds &time_to_live)
    : Rpc::TContext(TProtocol::Protocol),
      ServerAddress(server_address), SessionId(session_id), TimeToLive(time_to_live) {
  TFd fd;
  TFd::SocketPair(fd, InternalSocket, AF_UNIX, SOCK_STREAM);
  Device = make_shared<TDevice>(move(fd));
  BinaryIoStream = make_shared<TBinaryIoStream>(Device);
  DispatchThread = thread(&TClient::DispatchMain, this);
  IoThread = thread(&TClient::IoMain, this);
}

TClient::~TClient() {
  assert(this);
  Destructing.Push();
  IoThread.join();
  DispatchThread.join();
}

shared_ptr<TFuture<string>> TClient::Echo(const string &msg) {
  assert(this);
  return Write<string>(1000, msg);
}

shared_ptr<TFuture<void>> TClient::InstallPackage(const vector<string> &package_name, uint64_t version) {
  assert(this);
  return Write<void>(ServerRpc::InstallPackage, package_name, version);
}

shared_ptr<TFuture<void>> TClient::UnInstallPackage(const vector<string> &package_name, uint64_t version) {
  assert(this);
  return Write<void>(ServerRpc::UninstallPackage, package_name, version);
}

shared_ptr<Rpc::TFuture<TUuid>> TClient::NewFastPrivatePov(const TOpt<TUuid> &parent_pov_id, const std::chrono::seconds &ttl) {
  assert(this);
  return Write<TUuid>(ServerRpc::NewFastPrivatePov, parent_pov_id, ttl);
}

shared_ptr<Rpc::TFuture<TUuid>> TClient::NewSafePrivatePov(const TOpt<TUuid> &parent_pov_id, const std::chrono::seconds &ttl) {
  assert(this);
  return Write<TUuid>(ServerRpc::NewSafePrivatePov, parent_pov_id, ttl);
}

shared_ptr<Rpc::TFuture<TUuid>> TClient::NewFastSharedPov(const TOpt<TUuid> &parent_pov_id, const std::chrono::seconds &ttl) {
  assert(this);
  return Write<TUuid>(ServerRpc::NewFastSharedPov, parent_pov_id, ttl);
}

shared_ptr<Rpc::TFuture<TUuid>> TClient::NewSafeSharedPov(const TOpt<TUuid> &parent_pov_id, const std::chrono::seconds &ttl) {
  assert(this);
  return Write<TUuid>(ServerRpc::NewSafeSharedPov, parent_pov_id, ttl);
}

shared_ptr<Rpc::TFuture<void>> TClient::PausePov(const TUuid &pov_id) {
  assert(this);
  return Write<void>(ServerRpc::PausePov, pov_id);
}

shared_ptr<Rpc::TFuture<void>> TClient::UnpausePov(const TUuid &pov_id) {
  assert(this);
  return Write<void>(ServerRpc::UnpausePov, pov_id);
}

shared_ptr<Rpc::TFuture<void>> TClient::SetUserId(const TUuid &user_id) {
  assert(this);
  return Write<void>(ServerRpc::SetUserId, user_id);
}

shared_ptr<Rpc::TFuture<void>> TClient::SetTimeToLive(const TUuid &durable_id, const chrono::seconds &time_to_live) {
  assert(this);
  return Write<void>(ServerRpc::SetTimeToLive, durable_id, time_to_live);
}

shared_ptr<Rpc::TFuture<TMethodResult>> TClient::Try(const TUuid &pov_id, const vector<string> &fq_name, const TClosure &closure) {
  assert(this);
  return Write<TMethodResult>(ServerRpc::Try, pov_id, fq_name, closure);
}

shared_ptr<Rpc::TFuture<void>> TClient::BeginImport() {
  assert(this);
  return Write<void>(ServerRpc::BeginImport);
}

shared_ptr<Rpc::TFuture<void>> TClient::EndImport() {
  assert(this);
  return Write<void>(ServerRpc::EndImport);
}

shared_ptr<Rpc::TFuture<void>> TClient::TailGlobalPov() {
  assert(this);
  return Write<void>(ServerRpc::TailGlobalPov);
}

shared_ptr<Rpc::TFuture<string>> TClient::ImportCoreVector(const string &file_pattern, int64_t num_load_threads, int64_t num_merge_threads, int64_t merge_simultaneous) {
  assert(this);
  return Write<string>(ServerRpc::ImportCoreVector, file_pattern, num_load_threads, num_merge_threads, merge_simultaneous);
}

void TClient::DispatchMain() {
  assert(this);
  try {
    /* Loop until we get the destruction semaphore, handling messages from the server. */
    pollfd polls[2];
    polls[0].events = POLLIN;
    polls[0].fd = Destructing.GetFd();
    polls[1].events = POLLIN;
    polls[1].fd = Device->GetFd();
    for (;;) {
      IfLt0(poll(polls, 2, -1));
      if (polls[0].revents) {
        /* We're destructing. */
        break;
      }
      if (polls[1].revents) {
        /* The internal pipe contains data from the server. */
        shared_ptr<const TAnyRequest> request;
        do {
          try { request = Read(); } catch (...) { syslog(LOG_INFO, "stig client; dispatch thread; catch at line %d", __LINE__); throw; }
          if (request) {
            /* The server sent us an RPC request, so do what it requested. */
            try {
              (*request)();
            } catch (const exception &ex) {
              syslog(LOG_ERR, "stig client; dispatch thread; exception event handler; %s", ex.what());
            }
          }
        } while (BinaryIoStream->HasBufferedData());
      }
    }
    /* Release our I/O resources. */
    BinaryIoStream.reset();
    Device.reset();
  } catch (const exception &ex) {
    syslog(LOG_EMERG, "stig client; dispatch thread; top-level exception; %s; aborting process", ex.what());
    abort();
  }
}

void TClient::IoMain() {
  assert(this);
  try {
    /* Loop while the internal pipe is open, relaying data between the pipe and the server. */
    pollfd polls[2];
    for (size_t i = 0; i < 2; ++i) {
      polls[i].events = POLLIN;
    }
    polls[0].fd = InternalSocket;
    TFd server_socket;
    char buf[4096];
    /* Loop here until this client object is being destroyed. */
    for (;;) {
      /* Decide which events will wake us. */
      int fd_count = 1;
      if (server_socket.IsOpen()) {
        /* We're connected to the server, so we'll wake up if it talks to us. */
        polls[1].fd = server_socket;
        fd_count = 2;
      } else {
        /* We'll wake up only for the internal pipe, so get rid of any lingering bits in the second poller. */
        polls[1].revents = 0;
        fd_count = 1;
      }
      /* Wait for something to happen... */
      IfLt0(poll(polls, fd_count, -1));
      /* ... and now it has. */
      if (polls[0].revents) {
        /* The internal pipe has awakened us. */
        size_t size;
        try {
          size = ReadAtMost(InternalSocket, buf, sizeof(buf));
        } catch (...) {
          size = 0;
        }
        if (!size) {
          /* The internal pipe has closed, so it's time to leave. */
          InternalSocket.Reset();
          break;
        }
        /* The internal pipe has data we need to relay to the server. */
        string err_msg;
        if (!server_socket.IsOpen()) {
          /* We're not connected to the server, so try to connect now.
             If we fail, set 'err_msg' appropriately. */
          try {
            TFd new_server_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
            Connect(new_server_socket, ServerAddress);
            if (SessionId) {
              THandshake<TOldSession> handshake(TimeToLive, *SessionId);
              WriteExactly(new_server_socket, &handshake, sizeof(handshake));
              TOldSession::TReply reply;
              ReadExactly(new_server_socket, &reply, sizeof(reply));
              switch (reply.GetResult()) {
                case TOldSession::TReply::TResult::Success: {
                  server_socket = move(new_server_socket);
                  break;
                }
                case TOldSession::TReply::TResult::BadId: {
                  err_msg = "bad session id";
                  break;
                }
                case TOldSession::TReply::TResult::AlreadyConnected: {
                  err_msg = "session already connected";
                  break;
                }
                NO_DEFAULT_CASE;
              }
            } else {
              THandshake<TNewSession> handshake(TimeToLive);
              WriteExactly(new_server_socket, &handshake, sizeof(handshake));
              TNewSession::TReply reply;
              ReadExactly(new_server_socket, &reply, sizeof(reply));
              SessionId = reply.GetSessionId();
              server_socket = move(new_server_socket);
            }
          } catch (const exception &ex) {
            err_msg = ex.what();
          }
        }
        if (server_socket.IsOpen()) {
          /* Relay the data from the internal pipe to the server. */
          try {
            WriteExactly(server_socket, buf, size);
          } catch (const exception &ex) {
            /* We couldn't write.  Close the socket and report the error. */
            server_socket.Reset();
            err_msg = ex.what();
          }
        } else if (buf[0] == RequestIntroducer) {
          /* We're not connected to the server and the client is trying to send a request.
             Synthesize an error for him instead. */
          const char intro[] = "could not connect to server; ";
          const size_t intro_size = sizeof(intro) - 1;
          size_t
              msg_size = min(err_msg.size(), sizeof(buf) - (1 + sizeof(TRequestId) + sizeof(size_t) + intro_size)),
              nbo_size = SwapEnds(intro_size + msg_size);
          buf[0] = ErrorResultIntroducer;
          memcpy(buf + 1 + sizeof(TRequestId), &nbo_size, sizeof(size_t));
          memcpy(buf + 1 + sizeof(TRequestId) + sizeof(size_t), intro, intro_size);
          memcpy(buf + 1 + sizeof(TRequestId) + sizeof(size_t) + intro_size, err_msg.data(), msg_size);
          try {
            WriteExactly(InternalSocket, buf, 1 + sizeof(TRequestId) + sizeof(size_t) + intro_size + msg_size);
          } catch (...) {
            /* The internal pipe has closed, so it's time to leave. */
            InternalSocket.Reset();
            break;
          }
        } else {
          /* We're not connected to the server and the client is trying to send a reply.
             Just drop it. */
          syslog(LOG_ERR, "stig client; i/o thread; could not connect to server; %s; dropping reply", err_msg.c_str());
        }
      }
      if (polls[1].revents) {
        /* The server has awakened us. */
        size_t size;
        try {
          size = ReadAtMost(server_socket, buf, sizeof(buf));
        } catch (...) {
          size = 0;
        }
        if (size) {
          /* Relay the data from the server to the internal pipe. */
          try {
            WriteExactly(InternalSocket, buf, size);
          } catch (...) {
            /* The internal pipe has closed, so it's time to leave. */
            InternalSocket.Reset();
            break;
          }
        } else {
          /* The server has closed the socket. */
          server_socket.Reset();
        }
      }
    }  // for (;;)
  } catch (const exception &ex) {
    syslog(LOG_EMERG, "stig client; i/o thread; top-level exception; %s; aborting process", ex.what());
    abort();
  }
}

TClient::TProtocol::TProtocol() {
  Register<TClient, void, TUuid>       (ClientRpc::PovFailed,         &TClient::OnPovFailed);
  Register<TClient, void, TUuid, TUuid>(ClientRpc::UpdateAccepted,    &TClient::OnUpdateAccepted);
  Register<TClient, void, TUuid, TUuid>(ClientRpc::UpdateReplicated,  &TClient::OnUpdateReplicated);
  Register<TClient, void, TUuid, TUuid>(ClientRpc::UpdateDurable,     &TClient::OnUpdateDurable);
  Register<TClient, void, TUuid, TUuid>(ClientRpc::UpdateSemiDurable, &TClient::OnUpdateSemiDurable);
}

const TClient::TProtocol TClient::TProtocol::Protocol;
