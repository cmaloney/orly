/* <orly/server/ws.h>

   The websockets server object used by the main server.

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <cstdint>
#include <string>
#include <vector>

#include <netinet/in.h>

#include <base/class_traits.h>
#include <base/opt.h>
#include <base/uuid.h>
#include <orly/method_request.h>
#include <orly/method_result.h>

namespace Orly {

  namespace Server {

     /* An interface to a websocket server.  We use an interface-only definition
        here to avoid bloating up the build with a lot of websocket-specific
        headers. */
      class TWs {
        NO_COPY(TWs);
        public:

        /* Inherit and finalize this class to handle per-session operations.
           The websockets subsystem will destroy this interface object when the connection
           goes away, so, in the Orly sense, it's not the session (which is durable), but
           a pin in the session to indicate it is in use. */
        class TSessionPin {
          NO_COPY(TSessionPin);
          public:

          /* Do-little. */
          virtual ~TSessionPin() {}

          /* Override to perform the request (syncrhonously. */
          virtual void BeginImport() const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void EndImport() const = 0;

          /* The id used to resume the session later. */
          virtual const Base::TUuid &GetId() const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void Import(const std::string &path, uint64_t count) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void InstallPackage(const std::vector<std::string> &name, uint64_t version) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual Base::TUuid NewPov(bool is_safe, bool is_shared, const Base::TOpt<Base::TUuid> &parent_id) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void PausePov(const Base::TUuid &pov_id) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void SetTtl(const Base::TUuid &durable_id, const std::chrono::seconds &ttl) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void SetUserId(const Base::TUuid &user_id) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void Tail() const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual TMethodResult Try(const TMethodRequest &method_request) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void UninstallPackage(const std::vector<std::string> &name, uint64_t version) const = 0;

          /* Override to perform the request (syncrhonously. */
          virtual void UnpausePov(const Base::TUuid &pov_id) const = 0;

          protected:

          /* Do-little. */
          TSessionPin() {}

        };  // TWs::TSessionPin

        /* Inherit and finalize this class to handle per-connection.
           To the websockets subsystem, this interface represents the rest of the server. */
        class TSessionManager {
          NO_COPY(TSessionManager);
          public:

          /* For the convenience of those who inherit from us. */
          using TSessionPin = TWs::TSessionPin;

          /* Called when the connection wishes to create a new session. */
          virtual TSessionPin *NewSession() = 0;

          /* Called when the connection wishes to resume an old session. */
          virtual TSessionPin *ResumeSession(const Base::TUuid &id) = 0;

          protected:

          /* Do-little. */
          TSessionManager() {}

          /* Do-little. */
          virtual ~TSessionManager() {}

        };  // TWs::TSessionManager

        /* Shut down the server. */
        virtual ~TWs() {}

        /* Use this factory to construct an instance of this class.
           The server will be open for business by the time this function
           returns.  To shut down the server, destroy the object. */
        static TWs *New(TSessionManager *session_mngr, in_port_t port_number = 8080);

        protected:

        /* Start up the server. */
        TWs() {}

    };  // TWs

  }  // Server

}  // Orly