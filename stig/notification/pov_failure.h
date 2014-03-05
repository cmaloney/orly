/* <stig/notification/pov_failure.h>

   A notification pushed by the server to the client when a pov has failed.

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

    /* A notification pushed by the server to the client when a pov has failed. */
    class TPovFailure final
        : public TNotification {
      public:

      /* See base class. */
      virtual void Accept(const TVisitor &visitor) const override;

      /* The id of the pov which has failed. */
      const Base::TUuid &GetPovId() const {
        assert(this);
        return PovId;
      }

      /* True if this notification matches that one. */
      bool Matches(const TPovFailure &that) const;

      /* Stream out. */
      void Write(Io::TBinaryOutputStream &strm) const;

      /* Construct from scratch. */
      static TPovFailure *New(const Base::TUuid &pov_id) {
        return new TPovFailure(pov_id);
      }

      /* Construct from stream in. */
      static TPovFailure *New(Io::TBinaryInputStream &strm) {
        return new TPovFailure(strm);
      }

      private:

      /* Construct from scratch. */
      TPovFailure(const Base::TUuid &pov_id)
          : PovId(pov_id) {}

      /* Construct from stream in. */
      TPovFailure(Io::TBinaryInputStream &strm);

      /* See accessor. */
      Base::TUuid PovId;

    };  // TPovFailure

  }  // Notification

}  // Stig
