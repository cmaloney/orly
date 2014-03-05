/* <stig/notification/all.h>

   Operations which span all notification types.

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

#include <mpl/type_set.h>
#include <stig/notification/notification.h>
#include <stig/notification/pov_failure.h>
#include <stig/notification/system_shutdown.h>
#include <stig/notification/update_progress.h>
#include <visitor/visitor.h>

namespace Stig {

  namespace Notification {

    class TNotification::TVisitor
        : public Visitor::Single::TVisitor<Visitor::Pass, Mpl::TTypeSet<TPovFailure, TSystemShutdown, TUpdateProgress>> {};

    using Single = Visitor::Alias::Single<TNotification::TVisitor>;
    using Double = Visitor::Alias::Double<TNotification::TVisitor, TNotification::TVisitor>;

    /* True if the two notifications match. */
    bool Matches(const TNotification &lhs, const TNotification &rhs);

    /* Stream out. */
    void Write(Io::TBinaryOutputStream &strm, const TNotification *that);

    /* Construct from stream in. */
    TNotification *New(Io::TBinaryInputStream &strm);

  }  // Notification

}  // Stig
