/* <rpc/rpc.cc>

   Implements <rpc/rpc.h>.

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

#include <rpc/rpc.h>

#include <poll.h>

#include <base/debug_log.h>
#include <base/error_utils.h>
#include <base/no_default_case.h>
#include <base/thrower.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Rpc;

TContext::TUnexpectedResult::TUnexpectedResult()
    : runtime_error("unexpected result") {}

TContext::TSyntaxError::TSyntaxError()
    : runtime_error("syntax error") {}

TContext::~TContext() {}

shared_ptr<const TAnyRequest> TContext::Read() {
  assert(this);
  TBinaryIoStream &strm = GetBinaryIoStream();
  shared_ptr<const TAnyRequest> new_request;
  char introducer;
  TRequestId request_id;
  strm >> introducer >> request_id;
  switch (introducer) {
    case ErrorResultIntroducer: {
      string error_msg;
      strm >> error_msg;
      PopFuture(request_id)->SetErrorResult(error_msg);
      break;
    }
    case NormalResultIntroducer: {
      PopFuture(request_id)->SetNormalResult(strm);
      break;
    }
    case RequestIntroducer: {
      TEntryId entry_id;
      strm >> entry_id;
      new_request = Protocol.FindEntry(entry_id)->NewRequest(request_id, shared_from_this());
      ++UnhandledRequestCount;
      break;
    }
    default: {
      DEFINE_ERROR(error_t, runtime_error, "rpc; bad message introducer");
      THROW_ERROR(error_t) << static_cast<int>(introducer);
    }
  }
  return new_request;
}

void TContext::FailAllFutures(const string &error_msg) {
  assert(this);
  lock_guard<mutex> lock(FutureByRequestIdMutex);
  for (const auto &iter: FutureByRequestId) {
    string my_error = error_msg;
    iter.second->SetErrorResult(my_error);
  }
}

shared_ptr<TAnyFuture> TContext::PopFuture(TRequestId request_id) {
  assert(this);
  lock_guard<mutex> lock(FutureByRequestIdMutex);
  auto iter = FutureByRequestId.find(request_id);
  if (iter == FutureByRequestId.end()) {
    throw TUnexpectedResult();
  }
  shared_ptr<TAnyFuture> future = iter->second;
  FutureByRequestId.erase(iter);
  return future;
}

TAnyFuture::TRemoteError::TRemoteError(const string &error_msg)
    : runtime_error(error_msg) {}

TAnyFuture::~TAnyFuture() {}

TAnyFuture::operator bool() const {
  assert(this);
  bool is_ready = (ResultStatus.load(std::memory_order_consume) != NoResult);
  if (!is_ready) {
    pollfd temp;
    temp.fd = EventFd;
    temp.events = POLLIN;
    temp.revents = 0;
    IfLt0(poll(&temp, 1, 0));
    is_ready = (temp.revents & POLLIN);
  }
  return is_ready;
}

void TAnyFuture::Sync() const {
  assert(this);
  bool is_ready;
  do {
    switch (ResultStatus.load(std::memory_order_consume)) {
      case NoResult: {
        eventfd_t dummy;
        IfLt0(eventfd_read(EventFd, &dummy));
        is_ready = false;
        break;
      }
      case NormalResult: {
        is_ready = true;
        break;
      }
      case ErrorResult: {
        throw TRemoteError(ErrorMsg);
      }
      NO_DEFAULT_CASE;
    }
  } while (!is_ready);
}

TAnyFuture::TAnyFuture()
    : EventFd(eventfd(0, 0)), ResultStatus(NoResult) {}

void TAnyFuture::SetErrorResult(string &error_msg) {
  assert(this);
  assert(&error_msg);
  ErrorMsg = move(error_msg);
  SetResultStatus(ErrorResult);
}

void TAnyFuture::SetResultStatus(TResultStatus result_status) {
  assert(this);
  assert(result_status != NoResult);
  assert(ResultStatus == NoResult);
  ResultStatus.store(result_status, std::memory_order_release);
  IfLt0(eventfd_write(EventFd, 1));
}

TAnyRequest::~TAnyRequest() {}

void TAnyRequest::WriteError(TBinaryOutputStream &strm, TRequestId request_id, const exception &ex) {
  assert(&strm);
  assert(&ex);
  strm << ErrorResultIntroducer << request_id << ex.what();
  strm.Flush();
}

TAnyEntry::~TAnyEntry() {}

TProtocol::TUnknownEntry::TUnknownEntry()
    : runtime_error("unknown request factory") {}

TProtocol::~TProtocol() {
  assert(this);
  for (auto item: EntryById) {
    delete item.second;
  }
}

const TAnyEntry *TProtocol::FindEntry(TEntryId entry_id) const {
  assert(this);
  auto iter = EntryById.find(entry_id);
  if (iter == EntryById.end()) {
    throw TUnknownEntry();
  }
  return iter->second;
}
