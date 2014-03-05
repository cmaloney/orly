/* <stig/method_request.cc>

   Implements <stig/method_request.h>.

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

#include <stig/method_request.h>

#include <syslog.h>

#include <stig/atom/transport_arena2.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Stig;
using namespace Stig::Atom;

TMethodRequest::TMethodRequest(const TId &pov_id, const TPackage &package, const TClosure &closure)
    : PovId(pov_id), TimeToLive(0), Package(package), Closure(closure) {
  assert(IsValid());
}

TMethodRequest::TMethodRequest(const TId &pov_id, const TPackage &package, const TClosure &closure, const TTimeToLive &time_to_live)
    : TMethodRequest(pov_id, package, closure) {
  TimeToLive = time_to_live;
}

TMethodRequest::TMethodRequest(const TId &pov_id, const TPackage &package, const TClosure &closure, const TId &tracking_id)
    : TMethodRequest(pov_id, package, closure) {
  TrackingId = tracking_id;
}

void TMethodRequest::Read(TBinaryInputStream &strm) {
  assert(this);
  assert(&strm);
  strm >> PovId >> TimeToLive >> TrackingId >> Package >> Closure;
  if (!IsValid()) {
    *this = TMethodRequest();
    syslog(LOG_ERR, "SyntaxError MethodRequest !IsValid()");
    throw TInputConsumer::TSyntaxError();
  }
}

void TMethodRequest::Write(TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  strm << PovId << TimeToLive << TrackingId << Package << Closure;
}

bool TMethodRequest::IsValid() const {
  assert(this);
  return !Package.empty() && !Closure.GetMethodName().empty() && (!TrackingId || !TimeToLive.count());
}
