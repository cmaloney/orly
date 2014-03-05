/* <stig/perf/kv_exercise.cc>

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
#include <condition_variable>
#include <mutex>
#include <signal.h>
#include <unistd.h>
#include <unordered_set>
#include <sys/wait.h>

#include <base/error_utils.h>
#include <base/hash.h>
#include <base/log.h>
#include <base/time.h>
#include <base/timer.h>
#include <stig/protocol.h>
#include <stig/client/client.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Socket;
using namespace Stig;
using namespace Client;

class TExerciseClient final
    : public TClient {
  public:

  TExerciseClient(const Base::TOpt<TUuid> &session_id, const TAddress &addr)
      : TClient(addr, session_id, seconds(600)) {}

  void WaitForReplication(const Base::TUuid &tracker_id, const Base::TUuid &repo_id) {
    std::unique_lock<std::mutex> lock(ReplicationMutex);
    auto search_pair = make_pair(tracker_id, repo_id);
    auto ret = ReplicatedUpdateToRepo.find(search_pair);
    while (ret == ReplicatedUpdateToRepo.end()) {
      ReplicationCond.wait(lock);
      ret = ReplicatedUpdateToRepo.find(search_pair);
    }
    ReplicatedUpdateToRepo.erase(ret);
  }

  void WaitForTetris(const Base::TUuid &tracker_id, const Base::TUuid &repo_id) {
    std::unique_lock<std::mutex> lock(TetrisMutex);
    auto search_pair = make_pair(tracker_id, repo_id);
    auto ret = TetrisedUpdateToRepo.find(search_pair);
    while (ret == TetrisedUpdateToRepo.end()) {
      TetrisCond.wait(lock);
      ret = TetrisedUpdateToRepo.find(search_pair);
    }
    TetrisedUpdateToRepo.erase(ret);
  }

  private:

  virtual void OnPovFailed(const Base::TUuid &/*repo_id*/) override {}

  virtual void OnUpdateAccepted(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override {
    std::lock_guard<std::mutex> lock(TetrisMutex);
    auto ret = TetrisedUpdateToRepo.insert(make_pair(tracking_id, repo_id));
    assert(ret.second);
    TetrisCond.notify_all();
  }

  virtual void OnUpdateReplicated(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override {
    std::lock_guard<std::mutex> lock(ReplicationMutex);
    auto ret = ReplicatedUpdateToRepo.insert(make_pair(tracking_id, repo_id));
    assert(ret.second);
    ReplicationCond.notify_all();
  }

  virtual void OnUpdateDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

  virtual void OnUpdateSemiDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

  std::unordered_set<std::pair<Base::TUuid, Base::TUuid>> ReplicatedUpdateToRepo;
  std::mutex ReplicationMutex;
  std::condition_variable ReplicationCond;
  std::unordered_set<std::pair<Base::TUuid, Base::TUuid>> TetrisedUpdateToRepo;
  std::mutex TetrisMutex;
  std::condition_variable TetrisCond;

};

static const int64_t NumIter = 5000L;
static std::atomic<size_t> Completed(0UL);
static const int64_t NumUsers = 300000000L;

/* Command-line arguments. */
class TCmd final
    : public Base::TLog::TCmd {
  public:

  /* Construct with defaults. */
  TCmd() : Addr(TAddress::IPv4Loopback, DefaultPortNumber), NumThreads(1UL), NumReadsPerWrite(10UL), FlushIntervalMs(10UL), SlowStartUpTime(5UL) {}

  /* Construct from argc/argv. */
  TCmd(int argc, char *argv[])
      : TCmd() {
    Parse(argc, argv, TMeta());
  }

  /* TODO  */
  Socket::TAddress Addr;

  /* TODO */
  size_t NumThreads;
  size_t NumReadsPerWrite;
  size_t FlushIntervalMs;
  size_t SlowStartUpTime;

  private:

  /* Our meta-type. */
  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    /* Registers our fields. */
    TMeta()
        : Base::TLog::TCmd::TMeta("The kv exercise.") {
      Param(
          &TCmd::Addr, "addr", Optional, "addr\0",
          "Address of the server."
      );
      Param(
          &TCmd::NumThreads, "num_threads", Optional, "num_threads\0",
          "Number of threads executing client code."
      );
      Param(
          &TCmd::NumReadsPerWrite, "rw_ratio", Optional, "rw_ratio\0",
          "Number of reads performed per write."
      );
      Param(
          &TCmd::FlushIntervalMs, "flush_interval", Optional, "flush_interval\0",
          "Minimum interval (ms) between operations in a single thread."
      );
      Param(
          &TCmd::SlowStartUpTime, "slow_startup_time", Optional, "slow_startup_time\0",
          "Number of seconds between the start of each client thread."
      );
    }

  };  // TCmd::TMeta

};  // TCmd

void Runner(int64_t start, int64_t limit, const ::TCmd &cmd) {
  printf("Runner() starting \n");
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Base::TTime till_next;
  till_next.Now();
  std::mt19937_64 engine(rand());
  std::vector<uint64_t> inserted_id_vec;
  inserted_id_vec.reserve(100000000);
  try {
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, cmd.Addr);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(10));
    printf("Runner() new fast pov \n");
    Base::TUuid id_to_use = **pov_id;
    printf("Runner() has POV \n");
    for (;;) {
      for (int64_t i = limit - 1; i >= start; --i) {
        //printf("Runner() [%ld] \n", i);
        for (;;) { /* error retry loop */
          try {
            till_next += cmd.FlushIntervalMs;
            if (i % 1000 == 0) {
              std::cout << i << std::endl;
            }
            int64_t seconds = till_next.Remaining() / 1000;
            timespec wait_time {seconds, static_cast<int64_t>(till_next.Remaining() % 1000) * 1000000L};
            nanosleep(&wait_time, nullptr);
            /* insert data */ {
              int64_t uid = engine() % NumUsers;
              Base::TTimer timer;
              timer.Start();
              auto push_result = client->Try(id_to_use, { "kv" }, TClosure(string("insert"),
                                                                           string("uid"), uid,
                                                                           string("val"), uid * 2L));
              bool out;
              Sabot::ToNative(*Sabot::State::TAny::TWrapper((*push_result)->GetValue().NewState((*push_result)->GetArena().get(), state_alloc)), out);
              timer.Stop();
              inserted_id_vec.push_back(uid);
              if (timer.Total() > 0.1) {
                stringstream ss;
                ss << "Write time [" << timer.Total() << "]" << std::endl;
                std::cout << ss.str();
              }
            }
            /* read data */ {
              for (size_t j = 0; j < cmd.NumReadsPerWrite; ++j) {
                int64_t uid = inserted_id_vec[engine() % inserted_id_vec.size()];
                Base::TTimer timer;
                timer.Start();
                auto read_result = client->Try(id_to_use, { "kv" }, TClosure(string("get_key"),
                                                                             string("uid"), uid));
                std::vector<int64_t> out;
                Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
                if (out.size() != 1UL) {
                  throw std::logic_error("Read back of inserted value is broken");
                } else if (out[0] != uid) {
                  throw std::logic_error("Read back of inserted value did not return correct result");
                }
                timer.Stop();
                inserted_id_vec.push_back(uid);
                if (timer.Total() > 0.1) {
                  stringstream ss;
                  ss << "Read time [" << timer.Total() << "]" << std::endl;
                  std::cout << ss.str();
                }
              }
            }
            ++Completed;
            break;
          } catch (const exception &ex) {
            std::cerr << "caught error while writing : " << ex.what() << std::endl;
            usleep(20);
          }
        }
      }
    }

  } catch (const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
  }
  std::cout << "Done" << std::endl;
}

int main(int argc, char *argv[]) {
  ::TCmd cmd(argc, argv);
  TLog log(cmd);
  sleep(5);
  /* install kv.1 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, cmd.Addr);
    auto ack = client->InstallPackage({ "kv" }, 1);
    assert(ack);
    ack->Sync();
  }
  string error;
  Base::TTimer timer;
  timer.Start();
  vector<thread *> thread_vec;
  for (size_t i = 0; i < cmd.NumThreads; ++i) {
    thread_vec.push_back(new thread(Runner, i * (NumIter / cmd.NumThreads), (i + 1) * (NumIter / cmd.NumThreads), cref(cmd)));
    sleep(cmd.SlowStartUpTime);
  }
  for (auto iter : thread_vec) {
    iter->join();
    delete iter;
  }
  timer.Stop();
  cout << "Time : " << timer.Total() << " for [" << Completed.load() << "] is [" << (Completed.load() / timer.Total()) << " / s]" << endl;
}