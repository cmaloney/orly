/* <stig/protocol.h>

   The client/server protocol used by Stig.

   The client and server communicate over a TCP/IP socket.  The client connects, then sends the handshake, requesting either a new
   session or to reconnect to an existing session.  The server replies (or, on error, hangs up), and the handshake is complete.  (The
   classes in Stig::Handshake, below, describe this process in more detail.)

   After the handshake, the protocol becomes full-duplex, meaning that the client and server can request services from each other at
   any time.  Stig::ServerRpc and Stig::ClientRpc describe the individual entry points provided by the server and client, respectively.
   (See <rpc/rpc.h> for more information about the formats of RPC messages in general.)

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

#include <chrono>
#include <cstdint>
#include <stdexcept>

#include <netinet/in.h>

#include <base/layout.h>
#include <base/thrower.h>
#include <base/uuid.h>
#include <rpc/rpc.h>

namespace Stig {

  /* The port number on which the Stig server listens by default. */
  const in_port_t DefaultPortNumber = 19380;

  /* The port number on which the Stig server listens for a slave connection by default. */
  const in_port_t DefaultSlavePortNumber = 19381;

  namespace Handshake {

    /* The first part of the handshake sent by the client at connection time. */
    class PACKED THeader {
      public:

      /* Thrown by GetRequestKind() when the header is malformed. */
      DEFINE_ERROR(TBadHeader, std::runtime_error, "Stig handshake started with a bad header");

      /* A code for requesting a new or old session. */
      typedef char TRequestKind;

      /* Default-constructs a bad header.  This is meant to be used as a read buffer and overwritten with a valid header. */
      THeader();

      /* Constructs a valid header with the given session request code. */
      THeader(TRequestKind request_kind, const std::chrono::seconds &time_to_live);

      /* Either TNewSession::RequestKind or TOldSession::RequestKind. */
      TRequestKind GetRequestKind() const;

      /* The time-to-live to use when creating or re-opening the session. */
      std::chrono::seconds GetTimeToLive() const;

      private:

      /* A magic number used to identify the protocol. */
      typedef uint32_t TIntro;

      /* A protocol version number. */
      typedef uint16_t TVersion;

      /* The magic number with which we always start. */
      static const TIntro MagicIntro = 0x53544947;

      /* The current version of the protocol. */
      static const TVersion CurrentVersion = 0x0100;

      /* Must equal MagicIntro. */
      TIntro Intro;

      /* Must equal CurrentVersion. */
      TVersion Version;

      /* The session's time-to-live, in seconds. */
      uint32_t TimeToLive;

      /* See accessor. */
      TRequestKind RequestKind;

    };  // Stig::Handshake::THeader

    /* Sanity check. */
    static_assert(sizeof(THeader) == 11, "handshake header is insane");

    /* The general format of the handshake sent by the client at connection time.
       If the client wants to start a new session, it will send THandshake<TNewSession>.
       If the client wants to reconnect to an old session, it will send THandshake<TOldSession>. */
    template <typename TRequest>
    class PACKED THandshake {
      public:

      /* Pass the construction through to the request. */
      template <typename... TArgs>
      THandshake(const std::chrono::seconds &time_to_live, TArgs &&... args)
          : Header(TRequest::RequestKind, time_to_live), Request(std::forward<TArgs>(args)...) {}

      /* The request (for a new session or an old session) being sent. */
      const TRequest &GetRequest() const {
        assert(this);
        return Request;
      }

      private:

      /* See THeader. */
      THeader Header;

      /* See TNewSession and TOldSession. */
      TRequest Request;

    };  // Stig::Handshake::THandshake<TRequest>

    /* A request for a health check. */
    class PACKED THealthCheck {
      public:

      /* The code used in the header. */
      static const THeader::TRequestKind RequestKind = 'H';

      /* The reply sent by the server upon success. */
      class PACKED TReply {
        public:

        /* The result of the request. */
        enum class TResult : char {

          /* The machine is a master or solo and is ready to receive traffic.*/
          Ready = 'R',

          /* The machine is a synchronized slave (or a newly started machine trying to synchronize to become a slave)
             and so is not ready to receive traffic. */
          Unready = 'U',

          /* The reply object is uninitialzed. */
          Uninitialized = 'X'

        };  // TResult

        /* Default-constructs a bad reply.  This is meant to be used as a read buffer and overwritten with a valid reply. */
        TReply();

        /* Constructs a reply, giving the state (Master or Slave) of the current machine. */
        TReply(TResult result);

        /* See TResult. */
        TResult GetResult() const;

        private:

        /* See TResult. */
        TResult Result;

      };  // Stig::Handshake::THealthCheck::TReply

      /* Do-nothing constructor, as we currently have no members.  This may change in the future, if we start supporting
         multiple kinds of healthchecks. */
      THealthCheck() {}

    };  // Stig::Handshake::THealthCheck

    /* Sanity check. */
    static_assert(sizeof(THandshake<THealthCheck>) == 12, "health-check handshake is insane");

    /* A request for a new session. */
    class PACKED TNewSession {
      public:

      /* The code used in the header. */
      static const THeader::TRequestKind RequestKind = 'N';

      /* The reply sent by the server upon success. */
      class PACKED TReply {
        public:

        /* Default-constructs a bad reply.  This is meant to be used as a read buffer and overwritten with a valid reply. */
        TReply();

        /* Constructs a reply, granting the new session with the given id. */
        TReply(const Base::TUuid &session_id);

        /* The id of the newly created session. */
        Base::TUuid GetSessionId() const;

        private:

        /* See accessor. */
        uuid_t SessionId;

      };  // Stig::Handshake::TOldSession::TReply

      /* Do-nothing constructor, as we currently have no members.  This may change in the future, if we start supporting
         multiple kinds of sessions. */
      TNewSession() {}

    };  // Stig::Handshake::TNewSession

    /* Sanity check. */
    static_assert(sizeof(THandshake<TNewSession>) == 12, "new session handshake is insane");

    /* A request for an old session. */
    class PACKED TOldSession {
      public:

      /* The code used in the header. */
      static const THeader::TRequestKind RequestKind = 'O';

      /* The reply sent by the server upon success. */
      class PACKED TReply {
        public:

        /* The result of the request. */
        enum class TResult : char {

          /* The client has successfully reconnected to the old session.*/
          Success = 'S',

          /* The session id was bad.  It may have timed out or it may never have existed. */
          BadId = 'B',

          /* The session is already connected to another client. */
          AlreadyConnected = 'A',

          /* The reply object is uninitialzed. */
          Uninitialized = 'X'

        };  // TResult

        /* Default-constructs a bad reply.  This is meant to be used as a read buffer and overwritten with a valid reply. */
        TReply();

        /* Constructs a reply with the given result. */
        TReply(TResult result);

        /* See TResult. */
        TResult GetResult() const;

        private:

        /* See TResult. */
        TResult Result;

      };  // Stig::Handshake::TOldSession::TReply

      /* Default-constructs a bad request.  This is meant to be used as a read buffer and overwritten with a valid request. */
      TOldSession();

      /* Constructs a valid request to open the session with the given id. */
      TOldSession(const Base::TUuid &session_id);

      /* The id of the old session the client wants. */
      Base::TUuid GetSessionId() const;

      private:

      /* See accessor. */
      uuid_t SessionId;

    };  // Stig::Handshake::TOldSession

    /* Sanity check. */
    static_assert(sizeof(THandshake<TOldSession>) == 27, "old session handshake is insane");

  }  // Stig::Handshake

  namespace ServerRpc {

    /* Entries in the server's RPC context. */
    const Rpc::TEntryId

      /* SetUserId(Base::TUuid user_id) -> void;
         Assign a user id to the current session.  This is only valid if the session doesn't yet have a user id.
         NOTE: In the future, the server will call the client back, requesting a digital signature to authenticate
         the user.  We don't currently handle any of that, so the server just takes your word for it. */
      SetUserId = 1001,

      /* SetTimeToLive(Base::TUuid durable_id, std::chrono::seconds time_to_live) -> void;
         Change the time-to-live of a pov or session.  This will take effect when the session closes. */
      SetTimeToLive = 1002,

      /* InstallPackage(std::vector<std::string> fq_name, int64_t version) -> void;
         Install the given package, if it is not already installed. */
      InstallPackage = 1003,

      /* UninstallPackage(std::vector<std::string> fq_name, int64_t version) -> void;
         Uninstall the given package, if it is installed. */
      UninstallPackage  = 1004,

      /* NewFastPrivatePov(Base::TOpt<Base::TUuid> parent_pov_id, std::chrono::seconds time_to_live) -> Base::TUuid;
         Create a new, fast private pov.  A fast pov does not aggressively write through to durable storage and
         so responds more quickly than a safe pov; however, a fast pov may not survive a datacenter catastrophe.  If no
         parent is given, the global pov will be the parent. */
      NewFastPrivatePov = 1005,

      /* NewSafePrivatePov(Base::TOpt<Base::TUuid> parent_pov_id, std::chrono::seconds time_to_live) -> Base::TUuid;
         Create a new, safe private pov.  A safe pov aggressively writes through to durable storage and so responds
         more slowly than a fast pov; however, a safe pov will survive a datacenter catastrophe.  If no parent is given,
         the global pov will be the parent.*/
      NewSafePrivatePov = 1006,

      /* NewFastSharedPov(Base::TOpt<Base::TUuid> parent_pov_id, std::chrono::seconds time_to_live) -> Base::TUuid
         Create a new, fast shared pov.  Otherwise similar to NewFastPrivatePov(). */
      NewFastSharedPov = 1007,

      /* NewSafeSharedPov(Base::TOpt<Base::TUuid> parent_pov_id, std::chrono::seconds time_to_live) -> Base::TUuid
         Create a new, safe shared pov.  Otherwise similar to NewSafePrivatePov(). */
      NewSafeSharedPov = 1008,

      /* PausePov(Base::TUuid pov_id) -> void;
         Pause a private or shared pov, preventing its updates from promoting to its parent pov. */
      PausePov = 1009,

      /* UnpausePov(Base::TUuid pov_id) -> void;
         Undo the effect of PausePov(), permitting the pov's updates to promote to its parent pov once again. */
      UnpausePov = 1010,

      /* Try(Base::TUuid pov_id, std::vector<std::string> fq_name, TClosure closure) -> TMethodResult;
         Try to execute a method.  If the method has side-effects, the return will include a tracking id.  If the method has no side-effects,
         the return will not include a tracking id. */
      Try = 1011,

      /* TryTracked(Base::TUuid pov_id, std::vector<std::string> fq_name, TClosure closure) -> TMethodResult;
         Try to execute a method.  The return will include a tracking id, even if the method has no side-effects.  Use this when you want to
         provide the user with a temporally stable view, such as when producing paginated results. */
      TryTracked = 1012,

      /* DoInPast(Base::TUuid pov_id, std::vector<std::string> fq_name, TClosure closure, Base::TUuid tracking_id) -> TMethodResult;
         Execute a method in at a point in the past specified by a tracking id returned by Try() or TryTracked().  The method must not have
         side-effects. */
      DoInPast = 1013,

      /* BeginImport() -> void
         Put the server into a mode wherein it can accept Import() calls. */
      BeginImport = 1014,

      /* EndImport() -> void
         Exit import mode entered by calling BeginImport(). */
      EndImport = 1015,

    /* ImportCoreVector(std::string file, int64_t num_load_threads, int64_t num_merge_threads, int64_t merge_simultaneous) -> string
         Import the files from the given string pattern. */
      ImportCoreVector = 1017,

    /* TailGlobalPov() -> void
         Tail the global pov. */
      TailGlobalPov = 1018;

  }  // Stig::ServerRpc

  namespace ClientRpc {

    /* Entries in the client's RPC context. */
    const Rpc::TEntryId

      /* PovFailed(Base::TUuid pov_id) -> void;
         Notifes the session that a pov has failed.  The pov contains one or more of the session's updates, which is why the session
         was notified.  Sessions not involved in a pov are not notified if the pov fails. */
      PovFailed = 2001,

      /* UpdateAccepted(Base::TUuid pov_id, Base::TUuid tracking_id) -> void;
         Notifies the session that one of its updates has been accepted by a pov, meaning that the update is now visible by more users.
         This notification is not sent for the private pov in which the update begins, but is sent for each pov after that, including
         the global pov.  */
      UpdateAccepted = 2002,

      /* UpdateReplicated(Base::TUuid pov_id, Base::TUuid tracking_id) -> void;
         Notifies the session that one of its updates has been replicated, meaning it will survive a single machine failure.  This
         notification is sent every pov, including the private pov in which the update begins and the global pov. */
      UpdateReplicated = 2003,

      /* UpdateDurable(Base::TUuid pov_id, Base::TUuid tracking_id) -> void;
         Notifies the session that one of its updates has been written to durable storage on the master and on the slave, meaning it will
         survive a double machine failure.  This notification is sent every pov, including the private pov in which the update begins
         and the global pov. */
      UpdateDurable = 2004,

      /* UpdateSemiDurable(Base::TUuid pov_id, Base::TUuid tracking_id) -> void;
         Notifies the session that one of its updates has been written to durable storage on the master and that there is no slave.  This
         notification is sent every pov, including the private pov in which the update begins, but only when the server has no slave;
         if there is a slave, the server sends UpdateDurable() instead. */
      UpdateSemiDurable = 2005;

  }  // Stig::ClientRpc

}  // Stig
