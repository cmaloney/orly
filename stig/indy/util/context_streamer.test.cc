/* <stig/indy/util/context_streamer.test.cc>

   Unit test for <stig/indy/util/context_streamer.h>.

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

#include <stig/indy/util/context_streamer.h>

#include <condition_variable>
#include <thread>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/device.h>
#include <io/recorder_and_player.h>
#include <stig/atom/kit2.h>
#include <rpc/rpc.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Stig;
using namespace Stig::Indy;
using namespace Stig::Indy::Util;

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 100UL);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Update Entry", 200UL);

FIXTURE(InputToInput) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  int64_t num_iter = 10000L;
  TSequenceNumber seq = 0UL;
  Base::TUuid repo_id(TUuid::Best);
  TContextInputStreamer out_context;
  TContextInputStreamer in_context;
  for (int64_t i = 0L; i < num_iter; ++i) {
    TContextInputStreamer::TUpdate update;
    update.RepoId = repo_id;
    update.SequenceNumber = ++seq;
    update.Metadata = Atom::TCore(1L, nullptr, state_alloc);
    update.Metadata = Atom::TCore(Base::TUuid(TUuid::Best), nullptr, state_alloc);
    out_context.UpdateVec.push_back(std::move(update));
  }
  auto recorder = make_shared<TRecorder>();
  /* Write the context to the recorder. */ {
    TBinaryOutputOnlyStream strm(recorder);
    strm << out_context;
  }
  /* Read the ids back. */ {
    TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
    strm >> in_context;
  }
  EXPECT_EQ(in_context.UpdateVec.size(), out_context.UpdateVec.size());
}

class TApi
    : public Rpc::TContext {
  NO_COPY_SEMANTICS(TApi);
  public:

  static const Rpc::TEntryId
      RunId  = 1001;

  TApi(Base::TFd &&fd)
      : TContext(TProtocol::Protocol), Device(make_shared<TDevice>(move(fd))) {
    BinaryIoStream = make_shared<TBinaryIoStream>(Device);
  }

  void Shutdown() {
    IfLt0(shutdown(Device->GetFd(), SHUT_WR));
  }

  private:

  TContextInputStreamer Run(int64_t num_iter) {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TSequenceNumber seq = 0UL;
    Base::TUuid repo_id(TUuid::Best);
    TContextInputStreamer out_context;
    for (int64_t i = 0L; i < num_iter; ++i) {
      TContextInputStreamer::TUpdate update;
      update.RepoId = repo_id;
      update.SequenceNumber = ++seq;
      update.Metadata = Atom::TCore(1L, nullptr, state_alloc);
      update.Metadata = Atom::TCore(Base::TUuid(TUuid::Best), nullptr, state_alloc);
      out_context.UpdateVec.push_back(std::move(update));
    }
    return out_context;
  }

  class TProtocol
      : public Rpc::TProtocol {
    NO_COPY_SEMANTICS(TProtocol);
    public:

    static const TProtocol Protocol;

    private:

    TProtocol() {
      Register<TApi, TContextInputStreamer, int64_t>(RunId, &TApi::Run);
    }

  };  // TProtocol

  shared_ptr<TDevice> Device;

};

const TApi::TProtocol TApi::TProtocol::Protocol;

static void MakeApiContexts(shared_ptr<TApi> &a, shared_ptr<TApi> &b) {
  TFd fd_a, fd_b;
  TFd::SocketPair(fd_a, fd_b, AF_UNIX, SOCK_STREAM, 0);
  a = make_shared<TApi>(move(fd_a));
  b = make_shared<TApi>(move(fd_b));
}

static std::mutex Mutex;
static std::condition_variable Cond;
static std::queue<std::shared_ptr<const Rpc::TAnyRequest>> Queue;
static bool Running;

static void Runner(shared_ptr<Rpc::TContext> context) {
  assert(context);
  try {
    for (; Running;) {
      std::shared_ptr<const Rpc::TAnyRequest> request;
      /* lock queue */ {
        std::unique_lock<std::mutex> lock(Mutex);
        while (Queue.empty() && Running) {
          Cond.wait(lock);
        }
        if (Running) {
          request = Queue.front();
          Queue.pop();
        }
      }  // unlock queue
      if (Running) {
        assert(request);
        (*request)();
      }
    }
  } catch (...) {}
}

static void Queuer(shared_ptr<Rpc::TContext> context) {
  assert(context);
  try {
    for (;;) {
      auto request = context->Read();
      if (request) {
        std::lock_guard<std::mutex> lock(Mutex);
        Queue.push(request);
        Cond.notify_one();
      }
    }
  } catch (...) {}
}

FIXTURE(RPCInputToInput) {
  Running = true;
  int64_t num_iter = 10000L;
  shared_ptr<TApi> cli, svr;
  MakeApiContexts(cli, svr);
  thread svr_queue(bind(Queuer, svr)), svr_run(bind(Runner, svr)), cli_queue(bind(Queuer, cli)), cli_run(Runner, cli);
  auto future = cli->Write<TContextInputStreamer>(TApi::RunId, num_iter);
  assert(future);
  *future;
  TContextInputStreamer context = **future;
  std::cout << "Shutting down client.." << std::endl;
  cli->Shutdown();
  std::cout << "Shutting down server.." << std::endl;
  svr->Shutdown();
  std::cout << "Joining client queue.." << std::endl;
  cli_queue.join();
  std::cout << "Joining server queue.." << std::endl;
  svr_queue.join();
  Running = false;
  Cond.notify_all();
  std::cout << "Joining client runner.." << std::endl;
  cli_run.join();
  std::cout << "Joining server runner.." << std::endl;
  svr_run.join();
}