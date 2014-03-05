/* <rpc/rpc.test.cc>

   Unit test for <rpc/rpc.h>.

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

#include <functional>
#include <thread>

#include <base/uuid.h>
#include <io/device.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Rpc;

class TSequence {
  public:

  TSequence()
      : From(0UL), To(0UL) {}

  TSequence(size_t from, size_t to)
      : From(from), To(to) {
    assert(From <= To);
  }

  bool operator==(const TSequence &that) const {
    return From == that.From && To == that.To;
  }

  bool operator!=(const TSequence &that) const {
    return From != that.From || To != that.To;
  }

  size_t GetFrom() const {
    return From;
  }

  size_t GetTo() const {
    return To;
  }

  void Write(Io::TBinaryOutputStream &strm) const {
    strm << ((To - From) + 1UL);
    for (size_t i = From; i <= To; ++i) {
      strm << i;
    }
  }

  void Read(Io::TBinaryInputStream &strm) {
    size_t num_elem, temp;
    strm >> num_elem;
    for (size_t i = 0; i < num_elem; ++i) {
      strm >> temp;
      if (i == 0) {
        From = temp;
      }
    }
    To = temp;
  }

  private:

  size_t From;

  size_t To;
};

/* Binary streamers for TSequence */
inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TSequence &sequence) { sequence.Write(strm); return strm; }
inline Io::TBinaryOutputStream &&operator<<(Io::TBinaryOutputStream &&strm, const TSequence &sequence) { sequence.Write(strm); return std::move(strm); }
inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TSequence &sequence) { sequence.Read(strm); return strm; }
inline Io::TBinaryInputStream &&operator>>(Io::TBinaryInputStream &&strm, TSequence &sequence) { sequence.Read(strm); return std::move(strm); }

class TMathContext
    : public TContext {
  NO_COPY_SEMANTICS(TMathContext);
  public:

  static const Rpc::TEntryId
      AddId  = 1001,
      DivId  = 1002,
      EchoId = 1003,
      ConsId = 1004;

  TMathContext(Base::TFd &&fd)
      : TContext(TProtocol::Protocol), Device(make_shared<TDevice>(move(fd))) {
    BinaryIoStream = make_shared<TBinaryIoStream>(Device);
  }

  void Shutdown() {
    IfLt0(shutdown(Device->GetFd(), SHUT_WR));
  }

  private:

  int Add(int a, int b) {
    return a + b;
  }

  double Div(double a, double b) {
    if (!b) {
      throw runtime_error("division by zero");
    }
    return a / b;
  }

  TUuid Echo(const TUuid &id) {
    return id;
  }

  TSequence Cons(size_t from, size_t to) {
    return TSequence(from, to);
  }

  class TProtocol
      : public Rpc::TProtocol {
    NO_COPY_SEMANTICS(TProtocol);
    public:

    static const TProtocol Protocol;

    private:

    TProtocol() {
      Register<TMathContext, int, int, int>(AddId, &TMathContext::Add);
      Register<TMathContext, double, double, double>(DivId, &TMathContext::Div);
      Register<TMathContext, TUuid, TUuid>(EchoId, &TMathContext::Echo);
      Register<TMathContext, TSequence, size_t, size_t>(ConsId, &TMathContext::Cons);
    }

  };  // TProtocol

  shared_ptr<TDevice> Device;

};  // TMathContext

const TMathContext::TProtocol TMathContext::TProtocol::Protocol;

static void MakeMathContexts(shared_ptr<TMathContext> &a, shared_ptr<TMathContext> &b) {
  TFd fd_a, fd_b;
  TFd::SocketPair(fd_a, fd_b, AF_UNIX, SOCK_STREAM, 0);
  a = make_shared<TMathContext>(move(fd_a));
  b = make_shared<TMathContext>(move(fd_b));
}

FIXTURE(Typical) {
  /* Make a pair of contexts.  They should start out idle. */
  shared_ptr<TMathContext> a, b;
  MakeMathContexts(a, b);
  EXPECT_TRUE(a->IsIdle());
  EXPECT_TRUE(b->IsIdle());
  for (int i = 0; i < 2; ++i) {
    /* Start an RPC call which will return an int. */
    auto future = a->Write<int>(TMathContext::AddId, 1, 2);
    /* The pointer to the future is non-null. */
    EXPECT_TRUE(future);
    /* The future is not ready to read. */
    EXPECT_FALSE(*future);
    /* The requesting context is no longer idle because it's waiting for a response,
       but the responding context is still idle because it hasn't yet read the request. */
    EXPECT_FALSE(a->IsIdle());
    EXPECT_TRUE(b->IsIdle());
    /* Run the server thru one cycle. */ {
      /* Read the request. */
      auto request = b->Read();
      /* The request pointer is non-null. */
      EXPECT_TRUE(request);
      /* The responding context is now non-idle because it has work to do. */
      EXPECT_FALSE(b->IsIdle());
      /* Service the request. */
      (*request)();
      /* The responding context has finished its work and has become idle again. */
      EXPECT_TRUE(b->IsIdle());
    }
    /* Read the result. */
    a->Read();
    /* The future is now ready to read. */
    EXPECT_TRUE(*future);
    /* The future contains the correct value. */
    EXPECT_EQ(**future, 3);
    /* The requesting context has gotten its answer and is again idle. */
    EXPECT_TRUE(a->IsIdle());
    /* Make the server into the client and vice-versa. */
    swap(a, b);
  }
}

FIXTURE(Exception) {
  shared_ptr<TMathContext> a, b;
  MakeMathContexts(a, b);
  EXPECT_TRUE(a->IsIdle());
  EXPECT_TRUE(b->IsIdle());
  for (int i = 0; i < 2; ++i) {
    auto future = a->Write<double>(TMathContext::DivId, 10.0, 0.0);
    EXPECT_FALSE(a->IsIdle());
    EXPECT_TRUE(b->IsIdle());
    EXPECT_TRUE(future);
    EXPECT_FALSE(*future);
    /* extra */ {
      auto request = b->Read();
      EXPECT_FALSE(b->IsIdle());
      EXPECT_TRUE(request);
      (*request)();
      EXPECT_TRUE(b->IsIdle());
    }
    a->Read();
    EXPECT_TRUE(*future);
    bool caught;
    try {
      **future;
      caught = false;
    } catch (const TAnyFuture::TRemoteError &) {
      caught = true;
    }
    EXPECT_TRUE(caught);
    EXPECT_TRUE(a->IsIdle());
    swap(a, b);
  }
}

static void Run(shared_ptr<TContext> context) {
  assert(context);
  try {
    for (;;) {
      auto request = context->Read();
      if (request) {
        (*request)();
      }
    }
  } catch (...) {}
}

FIXTURE(Background) {
  TUuid uuid;
  uuid.Parse("1b4e28ba-2fa1-11d2-883f-b9a761bde3fb");
  shared_ptr<TMathContext> cli, svr;
  MakeMathContexts(cli, svr);
  thread bg_svr(bind(Run, svr)), bg_cli(Run, cli);
  EXPECT_EQ(**(cli->Write<int>(TMathContext::AddId, 1, 2)), 3);
  EXPECT_EQ(**(svr->Write<int>(TMathContext::AddId, 3, 4)), 7);
  EXPECT_EQ(**(cli->Write<TUuid>(TMathContext::EchoId, uuid)), uuid);
  EXPECT_EQ(**(svr->Write<TUuid>(TMathContext::EchoId, uuid)), uuid);
  cli->Shutdown();
  svr->Shutdown();
  bg_cli.join();
  bg_svr.join();
}

FIXTURE(LargeReturn) {
  shared_ptr<TMathContext> cli, svr;
  MakeMathContexts(cli, svr);
  thread bg_svr(bind(Run, svr)), bg_cli(Run, cli);
  EXPECT_TRUE(**(cli->Write<TSequence>(TMathContext::ConsId, 1UL, 2UL)) == TSequence(1UL, 2UL));
  EXPECT_FALSE(**(cli->Write<TSequence>(TMathContext::ConsId, 1UL, 2UL)) != TSequence(1UL, 2UL));
  EXPECT_TRUE(**(cli->Write<TSequence>(TMathContext::ConsId, 1UL, 200000UL)) == TSequence(1UL, 200000UL));
  EXPECT_FALSE(**(cli->Write<TSequence>(TMathContext::ConsId, 1UL, 200000UL)) != TSequence(1UL, 200000UL));
  cli->Shutdown();
  svr->Shutdown();
  bg_cli.join();
  bg_svr.join();
}