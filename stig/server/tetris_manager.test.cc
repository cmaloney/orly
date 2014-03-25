/* <stig/server/tetris_manager.test.cc>

   Unit test for <stig/server/tetris_manager.h>.

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

#include <stig/server/tetris_manager.h>

#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <base/event_semaphore.h>
#include <base/scheduler.h>
#include <base/uuid.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig;
using namespace Stig::Indy::Fiber;

/* Our tetris manager for testing purposes.
   This does more work than a real tetris manager because it manages the pov objects as well. */
class TTetrisManager final
    : public Stig::Server::TTetrisManager {
  public:

  /* TODO */
  class TPov {
    NO_COPY_SEMANTICS(TPov);
    public:

    /* Construct a non-global pov. */
    TPov(TPov *parent_pov) {
      assert(parent_pov);
      TetrisManager = parent_pov->TetrisManager;
      ParentPov = parent_pov;
      FinishInit();
    }

    /* The unique id of this pov. */
    const TUuid &GetId() const {
      assert(this);
      return Id;
    }

    /* The tetris manager which owns us. */
    TTetrisManager *GetTetrisManager() const {
      assert(this);
      return TetrisManager;
    }

    /* Push a new value onto this pov's queue. */
    void Push(int value) {
      assert(this);
      //lock_guard<mutex> Lock(Mutex);
      TFiberLock::TLock lock(FiberMutex);
      if (Values.empty()) {
        IsEmpty.WaitForMore(1UL);
        if (ParentPov) {
          TetrisManager->Join(ParentPov->Id, Id);
        }
      }
      Values.push(value);
    }

    /* Sum all the values in this pov's queue, emptying the queue in the process. */
    int Sum() {
      assert(this);
      int result = 0;
      //lock_guard<mutex> Lock(Mutex);
      TFiberLock::TLock lock(FiberMutex);
      if (!Values.empty()) {
        do {
          result += Values.front();
          Values.pop();
        } while (!Values.empty());
        IsEmpty.Complete();
        if (ParentPov) {
          TetrisManager->Part(ParentPov->Id, Id);
        }
      }
      return result;
    }

    /* Our parent point of view, if any.
       Only the global pov has no parent. */
    TPov *TryGetParentPov() const {
      assert(this);
      return ParentPov;
    }

    /* Try to pop a value off this pov's queue.
       If the queue is non-empty, pop the next value off it, return the value via out-param, and return true.
       If the queue is empty, leave the out-param alone and return false. */
    bool TryPop(int &value) {
      assert(this);
      assert(&value);
      //lock_guard<mutex> Lock(Mutex);
      TFiberLock::TLock lock(FiberMutex);
      bool success = !Values.empty();
      if (success) {
        value = Values.front();
        Values.pop();
        if (Values.empty()) {
          IsEmpty.Complete();
          if (ParentPov) {
            TetrisManager->Part(ParentPov->Id, Id);
          }
        }
      }
      return success;
    }

    /* Blocks as long as there are values in this pov's queue or until the timeout expires.
       Returns true if the queue is empty or false if the timeout expires.
       A timeout of -1 will never expire. */
    bool WaitUntilEmpty(int timeout = -1) const {
      assert(this);
      IsEmpty.Sync();
      return true;
      //return IsEmpty.GetFd().IsReadable(timeout);
    }

    private:

    /* Construct a global pov for the given tetris manager.
       A tetris manager always has exactly one such pov. */
    TPov(TTetrisManager *tetris_manager) {
      assert(tetris_manager);
      TetrisManager = tetris_manager;
      ParentPov = nullptr;
      FinishInit();
    }

    /* Do-little. */
    ~TPov() {}

    /* Finishes the initialization of a newly constructed pov. */
    void FinishInit() {
      assert(this);
      assert(TetrisManager);
      Id = TUuid::Best;
      TetrisManager->PovById.insert(make_pair(Id, this));
    }

    /* See accessor. */
    TTetrisManager *TetrisManager;

    /* See accessor. */
    TPov *ParentPov;

    /* See accessor. */
    TUuid Id;

    /* Readable only when the queue has no values in it. */
    mutable TSafeSync IsEmpty;

    /* Convers 'Values', below. */
    //mutex Mutex;
    TFiberLock FiberMutex;

    /* The queue of values moving down the pov. */
    queue<int> Values;

    /* For private constructor and destructor. */
    friend class TTetrisManager;

  };  // TTetrisManager::TPov

  /* Do-little. */
  TTetrisManager(TScheduler *scheduler,
                 Stig::Indy::Fiber::TRunner::TRunnerCons &runner_cons,
                 Base::TThreadLocalGlobalPoolManager<Stig::Indy::Fiber::TFrame, size_t, Stig::Indy::Fiber::TRunner *> *frame_pool_manager,
                 const std::function<void (TRunner *)> &runner_setup_cb,
                 bool is_master = true)
      : Stig::Server::TTetrisManager(scheduler, runner_cons, frame_pool_manager, runner_setup_cb, is_master) {
    GlobalPov = new TPov(this);
  }

  /* Do-little. */
  ~TTetrisManager() {
    assert(this);
    StopAllPlayers();  // This call is mandatory!
    for (const auto &item: PovById) {
      delete item.second;
    }
  }

  /* The global pov of this tetris manager.  Never null. */
  TPov *GetGlobalPov() const {
    assert(this);
    return GlobalPov;
  }

  /* Look up the given pov id and return the pov to which it maps.
     The id must refer to a valid pov. */
  TPov *GetPov(const TUuid &id) {
    assert(this);
    auto iter = PovById.find(id);
    assert(iter != PovById.end());
    return iter->second;
  }

  private:

  /* Our tetris player for testing purposes. */
  class TPlayer final
      : public Stig::Server::TTetrisManager::TPlayer {
    public:

    /* Caches the pointer to the tetris manager, sets the initial parent and child points of view, and starts playing. */
    TPlayer(TTetrisManager *tetris_manager, const TUuid &parent_pov_id, const TUuid &child_pov_id, bool is_paused, bool is_master)
        : Stig::Server::TTetrisManager::TPlayer(tetris_manager), TetrisManager(tetris_manager),
          ParentPov(tetris_manager->GetPov(parent_pov_id)), ChildPovs({ tetris_manager->GetPov(child_pov_id) }) {
      Start(is_paused, is_master);  // This call is mandatory!
    }

    private:

    /* See our base class.  We add the child pov to our set, with proper syncrhonization against Play(). */
    virtual void OnJoin(const TUuid &child_pov_id) override {
      assert(this);
      //lock_guard<mutex> lock(Mutex);
      TFiberLock::TLock lock(FiberMutex);
      ChildPovs.insert(TetrisManager->GetPov(child_pov_id));
    }

    /* See our base class.  We remove the child pov from our set, with proper syncrhonization against Play(). */
    virtual void OnPart(const TUuid &child_pov_id) override {
      assert(this);
      //lock_guard<mutex> lock(Mutex);
      TFiberLock::TLock lock(FiberMutex);
      ChildPovs.erase(TetrisManager->GetPov(child_pov_id));
    }

    /* See our base class.  This is a do-nothing for us. */
    virtual void OnPause() override {}

    /* See our base class.  This is a do-nothing for us. */
    virtual void OnUnpause() override {}

    /* See our base class.  We play the tetris here. */
    virtual void Play() override {
      assert(this);
      /* Nab a copy of our set of child points of view.
         We have to take this copy and release the lock so that our
         children can part from us when we pop them dry. */
      unordered_set<TPov *> child_povs;
      /* extra */ {
        //lock_guard<mutex> lock(Mutex);
        TFiberLock::TLock lock(FiberMutex);
        child_povs = ChildPovs;
      }
      /* Sum all the values we can pop from the children. */
      int sum = 0;
      bool popped = false;
      for (auto *pov: child_povs) {
        int value;
        if (pov->TryPop(value)) {
          sum += value;
          popped = true;
        }
      }
      /* If we popped at least one value, push the sum to the parent. */
      if (popped) {
        ParentPov->Push(sum);
      }
    }

    /* Our tetris manager. */
    TTetrisManager *TetrisManager;

    /* The id of the parent pov to which we push. */
    TPov *ParentPov;

    /* Covers 'ChildPovIds', below. */
    //mutex Mutex;
    TFiberLock FiberMutex;

    /* The ids of the child points of view from which we pop. */
    unordered_set<TPov *> ChildPovs;

  };  // TTetrisManager::TPlayer

  /* See our base class.  We construct one of our players. */
  virtual Stig::Server::TTetrisManager::TPlayer *NewPlayer(const TUuid &parent_pov_id, const TUuid &child_pov_id, bool is_paused, bool is_master) override {
    assert(&parent_pov_id);
    assert(&child_pov_id);
    return new TPlayer(this, parent_pov_id, child_pov_id, is_paused, is_master);
  }

  /* See accessor. */
  TPov *GlobalPov;

  /* A map from pov id to pov.  Never empty, as the global pov is always here. */
  unordered_map<TUuid, TPov *> PovById;

};  // TTetrisManager

FIXTURE(Typical) {
  const size_t stack_size = 1024UL * 1024UL;
  TRunner::TRunnerCons runner_cons(2);
  TRunner runner(runner_cons);
  Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> frame_pool_manager(30UL, stack_size, &runner);
  if (!TFrame::LocalFramePool) {
    TFrame::LocalFramePool = new Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(&frame_pool_manager);
  }
  auto launch_fiber_sched = [](TRunner *runner, Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> *frame_pool_manager) {
    if (!TFrame::LocalFramePool) {
      TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(frame_pool_manager);
    }
    runner->Run();
    delete TFrame::LocalFramePool;
    TFrame::LocalFramePool = nullptr;
  };

  std::thread t1(std::bind(launch_fiber_sched, &runner, &frame_pool_manager));

  class TTest : public TRunnable {
    NO_COPY_SEMANTICS(TTest);
    public:

    TTest(TRunner *runner,
          TRunner::TRunnerCons &runner_cons,
          Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> *frame_pool_manager,
          std::mutex &mut,
          std::condition_variable &cond,
          bool &finished)
        : RunnerCons(runner_cons),
          FramePoolManager(frame_pool_manager),
          Mutex(mut),
          Cond(cond),
          Finished(finished) {
      TFrame *frame = TFrame::LocalFramePool->Alloc();
      try {
        frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TTest::Run));
      } catch (...) {
        TFrame::LocalFramePool->Free(frame);
        throw;
      }
    }

    void Run() {
      /* tetris manager scope */ {
        TScheduler scheduler(TScheduler::TPolicy(10, 20, milliseconds(30000)));
        TTetrisManager tetris_manager(&scheduler, RunnerCons, FramePoolManager, [](TRunner *) -> void{});
        /* Build a pov tree with three shared povs, each with four private povs. */
        auto global_pov = tetris_manager.GetGlobalPov();
        tetris_manager.PausePlayer(global_pov->GetId());
        unordered_set<TTetrisManager::TPov *> shared_povs, private_povs;
        for (size_t i = 0; i < 3; ++i) {
          auto shared_pov = new TTetrisManager::TPov(global_pov);
          for (size_t j = 0; j < 4; ++j) {
            private_povs.insert(new TTetrisManager::TPov(shared_pov));
          }
          shared_povs.insert(shared_pov);
        }
        /* Push to the private povs. */
        int expected_sum = 0;
        for (size_t i = 1; i <= 3; ++i) {
          for (auto *pov: private_povs) {
            pov->Push(i);
            expected_sum += i;
          }
        }
        tetris_manager.UnpausePlayer(global_pov->GetId());
        /* Wait for all the private povs to drain. */
        for (auto *pov: private_povs) {
          pov->WaitUntilEmpty();
        }
        /* Wait for all the shared povs to drain. */
        for (auto *pov: shared_povs) {
          pov->WaitUntilEmpty();
        }
        /* Sum the global pov.  All our values should be here now. */
        EXPECT_EQ(global_pov->Sum(), expected_sum);
      }  // end tetris manager scope
      std::lock_guard<std::mutex> lock(Mutex);
      assert(!Finished);
      Finished = true;
      Cond.notify_one();
      Indy::Fiber::FreeMyFrame(TFrame::LocalFramePool);
    }

    private:

    TRunner::TRunnerCons &RunnerCons;
    Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> *FramePoolManager;

    std::mutex &Mutex;
    std::condition_variable &Cond;
    bool &Finished;
  };
  std::mutex mut;
  std::condition_variable cond;
  bool fin = false;
  TTest test(&runner, runner_cons, &frame_pool_manager, mut, cond, fin);
  std::unique_lock<std::mutex> lock(mut);
  while (!fin) {
    cond.wait(lock);
  }
  runner.ShutDown();
  delete TFrame::LocalFramePool;
  t1.join();
}
