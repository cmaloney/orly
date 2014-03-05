/* <stig/notification/update_progress.h>

   A notification pushed by the server to the client when a pov responds to an update.

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

#include <base/uuid.h>
#include <stig/notification/notification.h>

namespace Stig {

  namespace Notification {

    /* A notification pushed by the server to the client when a pov responds to an update. */
    class TUpdateProgress final
        : public TNotification {
      public:

      /* TODO */
      enum TResponse {

        /* TODO */
        Accepted,

        /* TODO */
        Replicated,

        /* TODO */
        SemiDurable,

        /* TODO */
        Durable,

      };  // TResponse

      /* See base class. */
      virtual void Accept(const TVisitor &visitor) const override;

      /* The id of the pov which is responding to the update. */
      const Base::TUuid &GetPovId() const {
        assert(this);
        return PovId;
      }

      /* The response of the pov to the update. */
      TResponse GetResponse() const {
        assert(this);
        return Response;
      }

      /* The id of the update on whose progress we're reporting. */
      const Base::TUuid &GetUpdateId() const {
        assert(this);
        return UpdateId;
      }

      /* True if this notification matches that one. */
      bool Matches(const TUpdateProgress &that) const;

      /* Stream out. */
      void Write(Io::TBinaryOutputStream &strm) const;

      /* Construct from scratch. */
      static TUpdateProgress *New(const Base::TUuid &pov_id, const Base::TUuid &update_id, TResponse response) {
        return new TUpdateProgress(pov_id, update_id, response);
      }

      /* Construct from stream in. */
      static TUpdateProgress *New(Io::TBinaryInputStream &strm) {
        return new TUpdateProgress(strm);
      }

      private:

      /* Construct from scratch. */
      TUpdateProgress(const Base::TUuid &pov_id, const Base::TUuid &update_id, TResponse response)
          : PovId(pov_id), UpdateId(update_id), Response(response) {}

      /* Construct from stream in. */
      TUpdateProgress(Io::TBinaryInputStream &strm);

      /* See accessors. */
      Base::TUuid PovId, UpdateId;

      /* See accessor. */
      TResponse Response;

    };  // TUpdateProgress

  }  // Notification

}  // Stig
