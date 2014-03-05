/* <stig/client/test_server.h>

   A server against which to test your clients.

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

#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include <poll.h>
#include <netinet/in.h>

#include <base/fd.h>
#include <base/layout.h>
#include <base/log.h>
#include <base/no_copy_semantics.h>
#include <io/endian.h>
#include <signal/handler_installer.h>
#include <socket/address.h>

namespace Stig {

  namespace BinProto {

    class PACKED TMsgHdr final {
      public:

      enum class TKind : uint8_t { Request = '?', NormalReply = '.', ErrorReply = '!' };

      enum class TByteOrder : uint8_t { Network = 'N', Host = 'H' };

      TMsgHdr() = delete;

      TMsgHdr(const TMsgHdr &) = default;

      TMsgHdr &operator=(const TMsgHdr &) = default;

      TKind GetKind() const {
        assert(this);
        return Kind;
      }

      uint32_t GetRequestId() const {
        assert(this);
        return RequestId;
      }

      uint32_t GetSize() const {
        assert(this);
        return Size;
      }

      static void Encode(char *&buf, TByteOrder byte_order, TKind kind, uint32_t request_id, uint32_t size) {
        /* Cast to point at a workspace, then fill in the latter fields. */
        auto *msg_hdr = Cast(buf);
        msg_hdr->Kind      = kind;
        msg_hdr->RequestId = request_id;
        msg_hdr->Size      = size;
        /* Handle byte-ordering. */
        if (byte_order != TByteOrder::Host) {
          msg_hdr->SwapEnds();
          msg_hdr->ByteOrder = TByteOrder::Network;
        } else {
          msg_hdr->ByteOrder = TByteOrder::Host;
        }
      }

      static TMsgHdr *TryDecode(char *&buf, TByteOrder &byte_order) {
        assert(&byte_order);
        /* Cast to point at a potential message header. */
        auto *msg_hdr = Cast(buf);
        /* Is the byte order good? */
        bool ok;
        switch (msg_hdr->ByteOrder) {
          /* The byte order is fine as-is. */
          case TByteOrder::Host: {
            byte_order = TByteOrder::Host;
            ok = true;
            break;
          }
          /* We need to swap ends, but it's ok. */
          case TByteOrder::Network: {
            msg_hdr->SwapEnds();
            msg_hdr->ByteOrder = TByteOrder::Host;
            byte_order = TByteOrder::Network;
            ok = true;
            break;
          }
          /* The byte order is bad. */
          default: {
            ok = false;
          }
        }
        return ok ? msg_hdr : nullptr;
      }

      private:

      void SwapEnds() {
        RequestId = Io::SwapEnds(RequestId);
        Size      = Io::SwapEnds(Size);
      }

      static TMsgHdr *Cast(char *&buf) {
        assert(&buf);
        assert(buf);
        auto *msg_hdr = reinterpret_cast<TMsgHdr *>(buf);
        buf += sizeof(TMsgHdr);
        return msg_hdr;
      }

      TByteOrder ByteOrder;

      TKind Kind;

      uint32_t RequestId;

      uint32_t Size;

    };  // TMsgHdr

    static_assert(sizeof(TMsgHdr) == 10, "BinProto::TMsgHdr must be exactly 10 bytes.");

  }  // BinProto

  namespace Client {

    /* A server against which to test your clients. */
    class TTestServer final {
      NO_COPY_SEMANTICS(TTestServer);
      public:

      /* Command-line parser. */
      class TCmd final
          : public Base::TLog::TCmd {
        public:

        /* Construct with defaults. */
        TCmd(bool verbose_logging = false);

        /* Construct from argc/argv. */
        TCmd(int argc, char *argv[]);

        /* The port on we listen for clients. */
        in_port_t PortNumber;

        /* The maximum number of connection requests to backlog. */
        int ConnectionBacklog;

        private:

        /* Our meta-type. */
        class TMeta final
            : public Base::TLog::TCmd::TMeta {
          public:

          /* Registers our fields. */
          TMeta();

        };  // TTestServer::TCmd::TMeta

      };  // TTestServer::TCmd

      /* Does not return until the server has started. */
      TTestServer(const TCmd &cmd);

      /* Does not return until the server has stopped. */
      ~TTestServer();

      /* The address on which the server accepts client connections. */
      const Socket::TAddress &GetAcceptingAddress() const {
        assert(this);
        return AcceptingAddress;
      }

      private:

      /* A trivial object, thrown by the TConn input functions when the client is quiet for too long. */
      class TDisconnected {};

      /* A trivial object, thrown by LaunchWorker() and the TConn I/O functions when Stopping, below, is true. */
      class TInterrupted {};

      /* A trivial object, thrown by the TConn input functions when the client is quiet for too long. */
      class TTimedOut {};

      /* Handles connunications between the server and a connected client.
         In particular, when a deadline is armed, the client must complete all I/O before the deadline or be hung up on. */
      class TConn final {
        NO_COPY_SEMANTICS(TConn);
        public:

        /* Handle communications between the given server and the given client socket.
           We take ownership of the client socket and will close it when we're destroyed.
           The deadline is initially disarmed. */
        TConn(const TTestServer *server, Base::TFd &&client);

        /* Set the I/O deadline to the given number of seconds from now and arm it. */
        void ArmDeadline(int secs = 2);

        /* Disarm the deadline. */
        void DisarmDeadline();

        /* Read at most the given number of bytes from the client and return the actual number of bytes read.
           Never returns zero.
           If the deadline arrives while we're doing so, throw TTimedOut.
           If the client disconnects while we're doing so, throw TDisconnected.
           If the server starts stopping while we're doing so, throw TInterrupted. */
        size_t RecvAtMost(void *buf, size_t size);

        /* Read exactly the given number of bytes from the client.
           If the deadline arrives while we're doing so, throw TTimedOut.
           If the client disconnects while we're doing so, throw TDisconnected.
           If the server starts stopping while we're doing so, throw TInterrupted. */
        void RecvExactly(void *buf, size_t size);

        /* Write at most the given number of bytes to the client and return the actual number of bytes written.
           Never returns zero.
           If the deadline arrives while we're doing so, throw TTimedOut.
           If the client disconnects while we're doing so, throw TDisconnected.
           If the server starts stopping while we're doing so, throw TInterrupted. */
        size_t SendAtMost(const void *buf, size_t size);

        /* Write exactly the given number of bytes to the client.
           If the deadline arrives while we're doing so, throw TTimedOut.
           If the client disconnects while we're doing so, throw TDisconnected.
           If the server starts stopping while we're doing so, throw TInterrupted. */
        void SendExactly(const void *buf, size_t size);

        private:

        /* Wait for the client to become read- or write-ready, depending on the flag, which must be POLLIN or POLLOUT.
           If the deadline arrives, the client disconnects, or some other error occurs, throw appropriately. */
        void WaitForClient(int flags);

        /* Hande the result of a send() or recv() operation.  If the actual number of bytes transferred is non-zero, then
           the function will simply return that number; otherwise, the function will throw appropriately.  This function
           never returns zero. */
        static size_t HandleIoResult(ssize_t actl);

        /* The server to which this connection belongs. */
        const TTestServer *const Server;

        /* The socket connected to the client. */
        Base::TFd Client;

        /* The an fd-based timer set to our next I/O deadline.
           This is armed only when DeadlineIsArmed, below, is true. */
        Base::TFd Deadline;

        /* True iff. Deadline, above, is armed.
           This is set by EnableDeadline() and reset by DisableDeadline(). */
        bool DeadlineIsArmed;

        /* We use this poller when performing I/O with the client.
           Event-0 is always the client (for read or write, as appropriate)
           and event-1 is always the deadline (for read). */
        pollfd Poller[2];

      };  // TTestServer::TConn

      /* Entry point of the thread which accepts connections. */
      void AcceptorMain(const std::string &worker_name);

      /* Entry point of a thread which handles a single connection. */
      void ConnectionMain(const std::string &worker_name, Base::TFd &client);

      /* Launch a worker thread with the given name at the given entry point.
         If Stopping is true, don't launch a worker and throw TInterrupted instead. */
      void LaunchWorker(std::string &&worker_name, std::function<void (const std::string &)> &&entry_point);

      /* Used by LaunchWorker() to wrap a worker entry point,
         allowing work to be done immediately before and after the entry point. */
      void WorkerWrapper(const std::string &worker_name, const std::function<void (const std::string &)> &entry_point);

      /* An RAII object to hold the log open. */
      Base::TLog Log;

      /* Installs a do-nothing handler for SIGUSR1. */
      Signal::THandlerInstaller SignalHandlerInstaller;

      /* False at construction, then set true in the destructor.
         When true, LaunchWorker() and the TConn I/O functions will throw TInterrupted. */
      std::atomic_bool Stopping;

      /* See accessor. */
      Socket::TAddress AcceptingAddress;

      /* The socket on which we wait for connections.
         This socket is opened, bound to AcceptingAddress, above, and set to listen during construction.
         It is closed by AcceptorMain() after Stopping becomes true. */
      Base::TFd AcceptingSocket;

      /* Covers Workers, below. */
      std::mutex WorkersMutex;

      /* The set of worker threads currently running.
         When a worker thread starts (at WorkerWrapper), it adds itself to this set and pings MoreWorkers.
         When it is about to stop, it removes itself from this set and pings FewerWorkers*/
      std::set<pthread_t> Workers;

      /* Notifies one observer when Workers increases in size. */
      std::condition_variable MoreWorkers;

      /* Notifies one observer when Workers decreases in size. */
      std::condition_variable FewerWorkers;

    };  // TTestServer

  }  // Client

}  // Stig
