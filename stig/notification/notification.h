/* <stig/notification/notification.h>

   A notification pushed by the server to the client.

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

#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>

namespace Stig {

  namespace Notification {

    /* The base class for a notification pushed by the server to the client.
       We're using the visitor pattern, so this is the base of a closed hierarchy. */
    class TNotification {
      NO_COPY_SEMANTICS(TNotification);
      public:

      /* Sum of final types. */
      class TVisitor;

      /* Do-little. */
      virtual ~TNotification();

      /* Override to accept the visitor. */
      virtual void Accept(const TVisitor &visitor) const = 0;

      protected:

      /* Construct from scratch. */
      TNotification() {}

      /* Construct from stream in. */
      TNotification(Io::TBinaryInputStream &);

      /* True if this notification matches that one. */
      bool Matches(const TNotification &that) const;

      /* Stream out. */
      void Write(Io::TBinaryOutputStream &strm) const;

    };  // TNotification

  }  // Notification

}  // Stig
