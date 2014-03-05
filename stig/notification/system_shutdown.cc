/* <stig/notification/system_shutdown.cc>

   Implements <stig/notification/system_shutdown.h>.

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

#include <stig/notification/system_shutdown.h>

using namespace Io;
using namespace Stig::Notification;

bool TSystemShutdown::Matches(const TSystemShutdown &that) const {
  assert(this);
  assert(&that);
  return TNotification::Matches(that) && Ttl == that.Ttl;
}

void TSystemShutdown::Write(TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  TNotification::Write(strm);
  strm << Ttl;
}

TSystemShutdown::TSystemShutdown(TBinaryInputStream &strm)
    : TNotification(strm) {
  assert(&strm);
  strm >> Ttl;
}
