/* <stig/notification/all.cc>

   Implements <stig/notification/all.h>.

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

#include <stig/notification/all.h>

#include <stig/notification/pov_failure.h>
#include <stig/notification/system_shutdown.h>
#include <stig/notification/update_progress.h>

using namespace Io;
using namespace Stig::Notification;

void TPovFailure::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(*this);
}

void TSystemShutdown::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(*this);
}

void TUpdateProgress::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(*this);
}

bool Stig::Notification::Matches(const TNotification &lhs, const TNotification &rhs) {
  typedef Notification::Double::TComputer<bool> computer_t;
  class visitor_t : public computer_t {
    public:
    visitor_t(bool &result) : computer_t(result) {}
    virtual void operator()(const TPovFailure &lhs, const TPovFailure &rhs) const override {
      Result = lhs.Matches(rhs);
    }
    virtual void operator()(const TPovFailure &, const TSystemShutdown &) const override {
      Result = false;
    }
    virtual void operator()(const TPovFailure &, const TUpdateProgress &) const override {
      Result = false;
    }
    virtual void operator()(const TSystemShutdown &, const TPovFailure &) const override {
      Result = false;
    }
    virtual void operator()(const TSystemShutdown &lhs, const TSystemShutdown &rhs) const override {
      Result = lhs.Matches(rhs);
    }
    virtual void operator()(const TSystemShutdown &, const TUpdateProgress &) const override {
      Result = false;
    }
    virtual void operator()(const TUpdateProgress &, const TPovFailure &) const override {
      Result = false;
    }
    virtual void operator()(const TUpdateProgress &, const TSystemShutdown &) const override {
      Result = false;
    }
    virtual void operator()(const TUpdateProgress &lhs, const TUpdateProgress &rhs) const override {
      Result = lhs.Matches(rhs);
    }
  };
  return Visitor::Double::Accept<visitor_t>(lhs, rhs);
}

void Stig::Notification::Write(TBinaryOutputStream &strm, const TNotification *that) {
  class visitor_t : public Notification::Single::TComputer<void> {
    public:
    visitor_t(TBinaryOutputStream &strm) : Strm(strm) {}
    virtual void operator()(const TPovFailure &that) const override {
      Strm << 'F';
      that.Write(Strm);
    }
    virtual void operator()(const TSystemShutdown &that) const override {
      Strm << 'S';
      that.Write(Strm);
    }
    virtual void operator()(const TUpdateProgress &that) const override {
      Strm << 'U';
      that.Write(Strm);
    }
    private:
    TBinaryOutputStream &Strm;
  };
  assert(&strm);
  assert(that);
  that->Accept(visitor_t(strm));
}

TNotification *Stig::Notification::New(TBinaryInputStream &strm) {
  assert(&strm);
  TNotification *result;
  char code;
  strm >> code;
  switch (code) {
    case 'F': {
      result = TPovFailure::New(strm);
      break;
    }
    case 'S': {
      result = TSystemShutdown::New(strm);
      break;
    }
    case 'U': {
      result = TUpdateProgress::New(strm);
      break;
    }
    default: {
      throw TInputConsumer::TSyntaxError();
    }
  }
  return result;
}
