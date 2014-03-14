/* <stig/server/tetris_manager.cc>

   Implements <stig/server/tetris_manager.h>.

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

#include <sys/syscall.h>
#include <unistd.h>

#include <base/debug_log.h>
#include <stig/indy/disk/util/volume_manager.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy;
using namespace Stig::Server;

bool TTetrisManager::IsPlayerPaused(const TUuid &parent_pov_id) const {
  assert(this);
  //lock_guard<mutex> lock(Mutex);
  Fiber::TFiberLock::TLock lock(FiberMutex);
  return PausedSet.find(parent_pov_id) != PausedSet.end();
}

void TTetrisManager::Join(const TUuid &parent_pov_id, const TUuid &child_pov_id) {
  assert(this);
  /* Lock the map and locate (or create) the slot where this parent pov's player would be. */
  //lock_guard<mutex> lock(Mutex);
  Fiber::TFiberLock::TLock lock(FiberMutex);
  auto iter = PlayerByParentPovId.insert(pair<TUuid, TPlayer *>(parent_pov_id, nullptr)).first;
  TPlayer *&player = iter->second;
  if (player) {
    /* The player for this parent pov already exists, so join the child to it. */
    player->Join(child_pov_id);
  } else {
    /* The player for this parent pov doesn't exist, so create one, starting with this child. */
    try {
      //lock_guard<mutex> lock(MasterLock);
      Fiber::TFiberLock::TLock master_lock(MasterFiberMutex);
      player = NewPlayer(parent_pov_id, child_pov_id, PausedSet.find(parent_pov_id) != PausedSet.end(), IsMaster);
    } catch (...) {
      PlayerByParentPovId.erase(iter);
      throw;
    }
  }
}

void TTetrisManager::Part(const TUuid &parent_pov_id, const TUuid &child_pov_id) {
  assert(this);
  /* Lock the map and locate the slot where this parent pov's player would be. */
  //lock_guard<mutex> lock(Mutex);
  Fiber::TFiberLock::TLock lock(FiberMutex);
  auto iter = PlayerByParentPovId.find(parent_pov_id);
  if (iter != PlayerByParentPovId.end()) {
    /* We found the player, so part the child from it. */
    if (!iter->second->Part(child_pov_id)) {
      /* This was the parent's last child, so we'll let it self-destruct quietly in another thread
         and remove it from the map.  The next time we try to join to this parent pov, we'll launch
         another player, even if the old one is still in the process of self-destructing. */
      iter->second->OnClose();
      PlayerByParentPovId.erase(iter);
    }
  }
}

void TTetrisManager::PausePlayer(const TUuid &parent_pov_id) {
  assert(this);
  //lock_guard<mutex> lock(Mutex);
  Fiber::TFiberLock::TLock lock(FiberMutex);
  PausedSet.insert(parent_pov_id);
  auto iter = PlayerByParentPovId.find(parent_pov_id);
  if (iter != PlayerByParentPovId.end()) {
    iter->second->Pause();
  }
}

void TTetrisManager::UnpausePlayer(const TUuid &parent_pov_id) {
  assert(this);
  //lock_guard<mutex> lock(Mutex);
  Fiber::TFiberLock::TLock lock(FiberMutex);
  if (PausedSet.erase(parent_pov_id)) {
    auto iter = PlayerByParentPovId.find(parent_pov_id);
    if (iter != PlayerByParentPovId.end()) {
      iter->second->Unpause();
    }
  }
}

void TTetrisManager::TPlayer::Join(const Base::TUuid &child_pov_id) {
  assert(this);
  ++ChildCount;
  OnJoin(child_pov_id);
}

bool TTetrisManager::TPlayer::Part(const Base::TUuid &child_pov_id) {
  assert(this);
  bool result = --ChildCount;
  if (result) {
    OnPart(child_pov_id);
  }
  return result;
}

void TTetrisManager::TPlayer::Pause() {
  assert(this);
  /* Create a semaphore and wait for the player thread to push it. */
  Paused = true;
  PausedSync.WaitForMore(1);
  PausedSync.Sync();
  //TEventSemaphore sem;
  //Paused = &sem;
  //sem.Pop();
}

void TTetrisManager::TPlayer::Stop() {
  assert(this);
  assert(!Stopped);
  /* Make a semaphore so we can know when the player has actually stopped. */
  CanWork.Push();
  Stopped = true;
  StoppedSync.WaitForMore(1);
  /* NOTE: After we set ChildCount to zero, the other thread could destroy this object at any time.
     We must therefore not dereference 'this' from here down. */
  ChildCount = 0;
  /* Wait for the player thread to stop. */
  /* TODO FIX: clearly StoppedSync is referencing this... */
  StoppedSync.Sync();
}

void TTetrisManager::TPlayer::Unpause() {
  assert(this);
  assert(Unpaused);
  /* Signal the player thread to continue. */
  UnpausedSync.Complete();
  Unpaused = false;
  //Unpaused->Push();
  //Unpaused = nullptr;
}

void TTetrisManager::TPlayer::OnClose() {
  CanWork.Push();
}

TTetrisManager::TPlayer::~TPlayer() {
  assert(this);
  if (Stopped) {
    StoppedSync.Complete();
    Stopped = false;
  }
  Fiber::FreeMyFrame(FramePool);
}

TTetrisManager::TPlayer::TPlayer(TTetrisManager *tetris_manager)
    : TetrisManager(tetris_manager), ChildCount(1), Stopped(nullptr), /*Paused(nullptr), */ Paused(false), /*Unpaused(nullptr)*/ Unpaused(false) {
  assert(tetris_manager);
  assert(Fiber::TFrame::LocalFramePool);
  FramePool = Fiber::TFrame::LocalFramePool;
  TetrisFrame = FramePool->Alloc();
}

void TTetrisManager::TPlayer::Start(bool is_paused, bool is_master) {
  assert(this);
  if (is_master) {
    CanWork.Push();
  }
  if (is_paused) {
    //TEventSemaphore sem;
    //Paused = &sem;
    Paused = true;
    PausedSync.WaitForMore(1);
    try {
      TetrisFrame->Latch(&TetrisManager->FiberScheduler, this, static_cast<Fiber::TRunnable::TFunc>(&TTetrisManager::TPlayer::Main));
    } catch (...) {
      FramePool->Free(TetrisFrame);
      TetrisFrame = nullptr;
      throw;
    }
    PausedSync.Sync();
    /* TODO: we should try to join some form of queue of frames that will get re-activated when the sem is pushed. */
    //Fiber::YieldSlow();
    //sem.Pop();
  } else {
    try {
      TetrisFrame->Latch(&TetrisManager->FiberScheduler, this, static_cast<Fiber::TRunnable::TFunc>(&TTetrisManager::TPlayer::Main));
    } catch (...) {
      FramePool->Free(TetrisFrame);
      TetrisFrame = nullptr;
      throw;
    }
  }
}

void TTetrisManager::TPlayer::Main() {
  assert(this);
  try {
    //DEBUG_LOG("tetris player %p: entering Main()", this);
    /* wait to see if we're allowed to play tetris. This will trigger if we're master, if we just became master, or if this player should be destroyed. */
    CanWork.Pop();
    for (;;) {
      if (Paused) {
        PausedSync.Complete();
        //Paused->Push();
        //Paused = nullptr;
        Paused = false;
        DEBUG_LOG("tetris player %p: pausing", this);
        OnPause();
        Unpaused = true;
        UnpausedSync.WaitForMore(1);
        //TEventSemaphore sem;
        //Unpaused = &sem;
        DEBUG_LOG("tetris player %p: paused", this);
        UnpausedSync.Sync();
        //sem.Pop();
        DEBUG_LOG("tetris player %p: unpausing", this);
        OnUnpause();
        DEBUG_LOG("tetris player %p: unpaused", this);
      } else if (ChildCount) {
        //DEBUG_LOG("tetris player %p: playing tetris; child_count = %ld", this, ChildCount);
        Play();
        if (usleep(0) < 0) {
          DEBUG_LOG("tetris player %p: signal detected", this);
          break;
        }
      } else {
        break;
      }
    }
    //DEBUG_LOG("tetris player %p: self-destructing", this);
    delete this;
    //DEBUG_LOG("tetris player %p: exiting Main()", this);
  } catch (const std::exception &ex) {
    syslog(LOG_EMERG, "TetrisManager::Player exception: %s", ex.what());
    throw;
  }
}

void TTetrisManager::TPlayer::BecomeMaster() {
  CanWork.Push();
}

TTetrisManager::TTetrisManager(Base::TScheduler *scheduler,
                               Stig::Indy::Fiber::TRunner::TRunnerCons &runner_cons,
                               Base::TThreadLocalGlobalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
                               const std::function<void (Indy::Fiber::TRunner *)> &runner_setup_cb,
                               bool is_master)
    : Scheduler(scheduler), FiberScheduler(runner_cons), IsMaster(is_master) {
  assert(scheduler);
  Base::TEventSemaphore setup_is_complete;
  auto launch_sched = [this, runner_setup_cb, &setup_is_complete](Fiber::TRunner *runner,
                                                                  Base::TThreadLocalGlobalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager) {
    if (!Fiber::TFrame::LocalFramePool) {
      Fiber::TFrame::LocalFramePool = new Base::TThreadLocalGlobalPoolManager<Fiber::TFrame, size_t, Fiber::TRunner *>::TThreadLocalPool(frame_pool_manager);
      FramePool = Fiber::TFrame::LocalFramePool;
    }
    runner_setup_cb(runner);
    setup_is_complete.Push();
    runner->Run();
    delete FramePool;
  };
  FiberThread = std::unique_ptr<std::thread>(new std::thread(std::bind(launch_sched, &FiberScheduler, frame_pool_manager)));
  setup_is_complete.Pop();
}

TTetrisManager::~TTetrisManager() {
  assert(this);
  assert(PlayerByParentPovId.empty());
  FiberScheduler.ShutDown();
  assert(FiberThread);
  assert(FiberThread->get_id() != std::this_thread::get_id());
  FiberThread->join();
}

void TTetrisManager::StopAllPlayers() {
  assert(this);
  //lock_guard<mutex> lock(Mutex);
  Fiber::TFiberLock::TLock lock(FiberMutex);
  for (const auto &item: PlayerByParentPovId) {
    item.second->Stop();
  }
  PlayerByParentPovId.clear();
}

void TTetrisManager::BecomeMaster() {
  assert(this);
  //lock_guard<mutex> lock(Mutex);
  Fiber::TFiberLock::TLock lock(FiberMutex);
  //lock_guard<mutex> master_lock(MasterLock);
  Fiber::TFiberLock::TLock master_lock(MasterFiberMutex);
  IsMaster = true;
  for (const auto &item: PlayerByParentPovId) {
    item.second->BecomeMaster();
  }
}