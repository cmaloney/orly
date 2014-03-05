/* <stig/notification/system_shutdown.h>

   A notification pushed by the server to the client when the system is shutting down.

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

#include <cassert>
#include <chrono>

#include <stig/notification/notification.h>

namespace Stig {

  namespace Notification {

    /* A notification pushed by the server to the client when the system is shutting down. */
    class TSystemShutdown final
        : public TNotification {
      public:

      /* See base class. */
      virtual void Accept(const TVisitor &visitor) const override;

      /* The number of seconds before the server shuts down. */
      const std::chrono::seconds &GetTtl() const {
        assert(this);
        return Ttl;
      }

      /* True if this notification matches that one. */
      bool Matches(const TSystemShutdown &that) const;

      /* Stream out. */
      void Write(Io::TBinaryOutputStream &strm) const;

      /* Construct from scratch. */
      static TSystemShutdown *New(const std::chrono::seconds &ttl) {
        return new TSystemShutdown(ttl);
      }

      /* Construct from stream in. */
      static TSystemShutdown *New(Io::TBinaryInputStream &strm) {
        return new TSystemShutdown(strm);
      }

      private:

      /* Construct from scratch. */
      TSystemShutdown(const std::chrono::seconds &ttl)
          : Ttl(ttl) {}

      /* Construct from stream in. */
      TSystemShutdown(Io::TBinaryInputStream &strm);

      /* See accessor. */
      std::chrono::seconds Ttl;

    };  // TSystemShutdown

  }  // Notification

}  // Stig
