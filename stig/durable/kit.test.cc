/* <stig/durable/kit.test.cc>

   Unit test for <stig/durable/kit.h>.

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

#include <stig/durable/kit.h>

#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>

#include <unistd.h>

#include <stig/durable/test_manager.h>
#include <test/kit.h>

using namespace std;
using namespace Io;
using namespace Stig::Durable;

class TSubModule final
    : public TObj {
  public:

  TSubModule(TManager *manager, const TId &id, const TTtl &ttl)
      : TObj(manager, id, ttl) {}

  TSubModule(TManager *manager, const TId &id, TBinaryInputStream &strm)
      : TObj(manager, id, strm) {}

  virtual const char *GetKind() const noexcept {
    return "submodule";
  }

  private:

  virtual ~TSubModule() {}

};

class TFile final
    : public TObj {
  public:

  TFile(TManager *manager, const TId &id, const TTtl &ttl, int val)
      : TObj(manager, id, ttl), Val(val) {}

  TFile(TManager *manager, const TId &id, TBinaryInputStream &strm)
      : TObj(manager, id, strm) {
    strm >> Val;
  }

  void NewMod(TManager *manager, const TTtl &time_to_live) {
    assert(this);
    lock_guard<mutex> lock(Mutex);
    Mods.push_back(manager->New<TSubModule>(Base::TUuid::Twister, time_to_live));
  }

  virtual const char *GetKind() const noexcept {
    return "file";
  }

  int GetVal() const {
    assert(this);
    return Val;
  }

  virtual void Write(TBinaryOutputStream &strm) const override {
    assert(this);
    TObj::Write(strm);
    strm << Val;
  }

  virtual bool ForEachDependentPtr(const function<bool (TAnyPtr &)> &cb) noexcept override {
    assert(this);
    assert(&cb);
    for (auto &mod: Mods) {
      if (!cb(mod)) {
        return false;
      }
    }
    Mods.clear();
    return true;
  }

  private:

  virtual ~TFile() {
    assert(this);
    Val = -99;
  }

  int Val;

  mutex Mutex;

  vector<TPtr<TSubModule>> Mods;

};

static void TestZeroTtl(bool with_cache) {
  TTestManager manager(with_cache ? 1 : 0);
  auto file = manager.New<TFile>(TId::Best, TTtl(0), 101);
  auto id = file->GetId();
  file.Reset();
  bool caught = false;
  try {
    manager.Open<TFile>(id);
  } catch (const TDoesntExist &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}

FIXTURE(ZeroTtlWithCache) {
  TestZeroTtl(true);
}

FIXTURE(ZeroTtlWithoutCache) {
  TestZeroTtl(false);
}

static void TestReopen(bool with_cache) {
  TTestManager manager(with_cache ? 1 : 0);
  auto file = manager.New<TFile>(TId::Best, TTtl(7), 101);
  auto id = file->GetId();
  file.Reset();
  file = manager.Open<TFile>(id);
  EXPECT_TRUE(file->GetId() == id);
  EXPECT_TRUE(file->GetTtl() == TTtl(7));
  EXPECT_TRUE(file->GetVal() == 101);
  file->SetTtl(TTtl(0));
  file.Reset();
  bool caught = false;
  try {
    manager.Open<TFile>(id);
  } catch (const TDoesntExist &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}

FIXTURE(ReopenWithCache) {
  TestReopen(true);
}

FIXTURE(ReopenWithoutCache) {
  TestReopen(false);
}

static void TestCleaning(bool with_cache) {
  TTestManager manager(with_cache ? 1 : 0);
  auto file = manager.New<TFile>(TId::Best, TTtl(1), 101);
  auto id = file->GetId();
  file.Reset();
  file = manager.Open<TFile>(id);
  EXPECT_TRUE(file->GetId() == id);
  EXPECT_TRUE(file->GetTtl() == TTtl(1));
  EXPECT_TRUE(file->GetVal() == 101);
  file.Reset();
  sleep(1);
  manager.Clean();
  bool caught = false;
  try {
    manager.Open<TFile>(id);
  } catch (const TDoesntExist &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}

FIXTURE(CleaningWithCache) {
  TestCleaning(true);
}

FIXTURE(CleaningWithoutCache) {
  TestCleaning(false);
}

static mutex Mutex;
static condition_variable Cv;
static bool Go = false;

static void Worker(TManager &manager, const vector<TId> &ids, size_t repeat_count, atomic_size_t &success_count) {
  /* extra */ {
    unique_lock<mutex> lock(Mutex);
    while (!Go) {
      Cv.wait(lock);
    }
  }
  bool success = true;
  for (size_t i = 0; success && i < repeat_count; ++i) {
    auto file = manager.Open<TFile>(ids[i % ids.size()]);
    file->NewMod(&manager, TTtl(999));
    success = !(file->GetDeadline());
  }
  if (success) {
    ++success_count;
  }
}

static void TestThunderingHerd(size_t cache_size, size_t id_count, size_t worker_count, size_t repeat_count) {
  // openlog("stig/durable/kit.test", LOG_PERROR, LOG_USER);
  vector<TId> ids;
  TTestManager manager(cache_size);
  for (size_t i = 0; i < id_count; ++i) {
    ids.push_back(manager.New<TFile>(TId::Best, TTtl(999), i)->GetId());
  }
  vector<thread> workers;
  Go = false;
  atomic_size_t success_count(0);
  for (size_t i = 0; i < worker_count; ++i, next_permutation(ids.begin(), ids.end())) {
    workers.push_back(thread(Worker, ref(manager), ids, repeat_count, ref(success_count)));
  }
  /* extra */ {
    unique_lock<mutex> lock(Mutex);
    Go = true;
    Cv.notify_all();
  }
  for (auto &worker: workers) {
    worker.join();
  }
  EXPECT_EQ(success_count, worker_count);
}

FIXTURE(ThunderingHerdWithFullCache) {
  TestThunderingHerd(505, 5, 100, 1000);
}

FIXTURE(ThunderingHerdWithPartialCache) {
  TestThunderingHerd(100, 5, 100, 1000);
}

FIXTURE(ThunderingHerdWithoutCache) {
  TestThunderingHerd(0, 5, 100, 1000);
}
