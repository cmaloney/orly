/* <stig/protocol.cc>

   Implements <stig/protocol.h>.

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

#include <stig/protocol.h>

#include <cstdlib>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Handshake;

THeader::THeader()
    : Intro(0), Version(0), TimeToLive(0), RequestKind('\0') {}

THeader::THeader(TRequestKind request_kind, const seconds &time_to_live)
    : Intro(MagicIntro), Version(CurrentVersion), TimeToLive(time_to_live.count()), RequestKind(request_kind) {}

THeader::TRequestKind THeader::GetRequestKind() const {
  assert(this);
  if (Intro != MagicIntro || Version != CurrentVersion) {
    THROW_ERROR(TBadHeader) << "intro = 0x" << hex << Intro << ", version = 0x" << hex << Version;
  }
  return RequestKind;
}

seconds THeader::GetTimeToLive() const {
  assert(this);
  return seconds(TimeToLive);
}

THealthCheck::TReply::TReply()
    : Result(TResult::Uninitialized) {}

THealthCheck::TReply::TReply(TResult result)
    : Result(result) {}

THealthCheck::TReply::TResult THealthCheck::TReply::GetResult() const {
  assert(this);
  return Result;
}

TNewSession::TReply::TReply() {
  memset(&SessionId, 0, sizeof(SessionId));
}

TNewSession::TReply::TReply(const TUuid &session_id) {
  memcpy(&SessionId, &(session_id.GetRaw()), sizeof(SessionId));
}

TUuid TNewSession::TReply::GetSessionId() const {
  assert(this);
  return TUuid(SessionId);
}

TOldSession::TReply::TReply()
    : Result(TResult::Uninitialized) {}

TOldSession::TReply::TReply(TResult result)
    : Result(result) {}

TOldSession::TReply::TResult TOldSession::TReply::GetResult() const {
  assert(this);
  return Result;
}

TOldSession::TOldSession() {
  memset(&SessionId, 0, sizeof(SessionId));
}

TOldSession::TOldSession(const TUuid &session_id) {
  memcpy(&SessionId, &(session_id.GetRaw()), sizeof(SessionId));
}

TUuid TOldSession::GetSessionId() const {
  assert(this);
  return TUuid(SessionId);
}
