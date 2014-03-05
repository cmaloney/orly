/* <stig/server/tetris_manager.h>

   The base for all managers and players of the tetris.

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

#pragma once

#include <cassert>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include <base/event_semaphore.h>
#include <base/no_copy_semantics.h>
#include <base/scheduler.h>
#include <base/uuid.h>
#include <stig/indy/fiber/fiber.h>

namespace Stig {

  namespace Server {

    /* The base for all managers of tetris players. */
    class TTetrisManager {
      NO_COPY_SEMANTICS(TTetrisManager);
      public:

      /* True iff. the given player is paused. */
      bool IsPlayerPaused(const Base::TUuid &parent_pov_id) const;

      /* Advise tetris that a child pov has something to tell its parent parent pov.
         If the parent pov is new to the manager, this will launch a new tetris player.
         The player may or may not have started by the time this function returns. */
      void Join(const Base::TUuid &parent_pov_id, const Base::TUuid &child_pov_id);

      /* Advise tetris that a child pov has nothing more to tell its parent pov.
         If this is the parent pov's last child, this will kill the parent's tetris player.
         The player may or may not have stopped by the time this function returns. */
      void Part(const Base::TUuid &parent_pov_id, const Base::TUuid &child_pov_id);

      /* Cause the player for the given pov to stop playing.  The player will continue to
         exist and will continue to handle children joining and parting, but it will not
         attempt to make progress.  This function will not return until the player has
         paused. */
      void PausePlayer(const Base::TUuid &parent_pov_id);

      /* Cause a player that was previously paused to resume normal operations. */
      void UnpausePlayer(const Base::TUuid &parent_pov_id);

      /* TODO */
      void BecomeMaster();

      protected:

      /* The base class for all players of the tetris. */
      class TPlayer
          : public Indy::Fiber::TRunnable {
        NO_COPY_SEMANTICS(TPlayer);
        public:

        /* Increments the child count and calls OnJoin(). */
        void Join(const Base::TUuid &child_pov_id);

        /* Decrements the child count.
           If the result is non-zero, this function then calls OnPart() and returns true.
           If the result is zero, this function does not call OnPart() and returns false. */
        bool Part(const Base::TUuid &child_pov_id);

        /* Pause operations.  This function will not return until we're actually paused. */
        void Pause();

        /* Stops this player and causes it to self-destruct.  Blocks until the player does so. */
        void Stop();

        /* Resume normal operations. */
        void Unpause();

        /* TODO */
        void OnClose();

        /* TODO */
        void BecomeMaster();

        protected:

        /* Caches the pointer to the tetris manager and gets ready to run but doesn't launch a job.
           You must call Start() in the constructor of your derived player or tetris will not be played. */
        TPlayer(TTetrisManager *tetris_manager);

        /* A players always dies by self-destruction, either because the last of its children parts from it
           or because the manager calls Stop().  If the player is self-destructing as a result of Stop(),
           this destructor will unblock the stopper. */
        virtual ~TPlayer();

        /* Overide to respond to a child pov joining to us.
           Note that this function is not called for the joining of our first child, as that is implied by our construction. */
        virtual void OnJoin(const Base::TUuid &child_pov_id) = 0;

        /* Overide to respond to a child pov parting from us.
           Note that this function is not called for the parting of our last child, as that causes us to self-destruct. */
        virtual void OnPart(const Base::TUuid &child_pov_id) = 0;

        /* Override to respond to the player being paused.
           This is called by the player thread after it ack's the pauser and before it goes to sleep waiting to unpause. */
        virtual void OnPause() = 0;

        /* Override to respond to the player being unpaused.
           This is called by the player thread after it wakes up but before it begins playing again. */
        virtual void OnUnpause() = 0;

        /* Override to play tetris.  This function will be called repeatedly as long as we have children or
           until we are told to stop, so play as efficiently as possible.  This function runs in its own thread,
           separate from the threads which call OnJoin() and OnPart(), so use proper syncrhonization around any
           data structures shared between these functions. */
        virtual void Play() = 0;

        /* Call this in the constructor of your derived player in order to launch the job which will play tetris. */
        void Start(bool is_paused, bool is_master);

        private:

        /* Loops, calling Play(), as long as there are child points of view joined to us.
           When last last child goes, this function deletes this player and exits. */
        void Main();

        /* Our manager.  Never null. */
        TTetrisManager *TetrisManager;

        /* The number of children currently joined to us.  This starts at one, as we must be constructed with a single child.
           Main() will run as long as this value is non-zero. */
        size_t ChildCount;

        /* Usually this pointer is null; however, Stop() points us at a semaphore we are to push in our destructor.  That's
           how we unblock Stop(). */
        //Base::TEventSemaphore *Stopped;
        bool Stopped;
        Indy::Fiber::TSafeSync StoppedSync;

        /* Usually this pointer is null; however, Pause() points us at a semaphore we are to push when we are paused.  That's
           how we unblock Pause(). */
        //Base::TEventSemaphore *Paused;
        bool Paused;
        Indy::Fiber::TSafeSync PausedSync;

        /* Usually this pointer is null; however, while paused, the player points this at a semaphore that Unpause() can push
           to unpause the player. */
        //Base::TEventSemaphore *Unpaused;
        bool Unpaused;
        Indy::Fiber::TSafeSync UnpausedSync;

        /* TODO */
        Base::TEventSemaphore CanWork;

        /* The fiber frame used to run our logic. */
        Indy::Fiber::TFrame *TetrisFrame;
        Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *>::TThreadLocalRegisteredPool *FramePool;

      };  // TTetrisManager::TPlayer

      /* Caches the pointer to the scheduler. */
      TTetrisManager(Base::TScheduler *scheduler,
                     Indy::Fiber::TRunner::TRunnerCons &runner_cons,
                     Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
                     const std::function<void (Indy::Fiber::TRunner *)> &runner_setup_cb,
                     bool is_master);

      /* You must call StopAllPlayers() in the destructor of your derived tetris manager or this destructor will fail. */
      virtual ~TTetrisManager();

      /* Override to construct a new tetris player connecting the given parent and child points of view. */
      virtual TPlayer *NewPlayer(const Base::TUuid &parent_pov_id, const Base::TUuid &child_pov_id, bool is_paused, bool is_master) = 0;

      /* Call this in the destructor of your derived tetris manager.  It will block until all tetris has stopped. */
      void StopAllPlayers();

      /* TODO */
      Base::TScheduler *GetScheduler() const {
        return Scheduler;
      }

      private:

      /* The scheduler we use when creating indy contexts. */
      Base::TScheduler *Scheduler;

      /* The Fiber Runners we use when launching tetris players. */
      Indy::Fiber::TRunner FiberScheduler;
      std::unique_ptr<std::thread> FiberThread;
      Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *>::TThreadLocalRegisteredPool *FramePool;

      /* Covers 'Players' and 'PausedSet', below. */
      //mutable std::mutex Mutex;
      mutable Indy::Fiber::TFiberLock FiberMutex;

      /* A mapping from parent point of view's id to its running tetris player.
         A particular parent pov will appear in this map only after at least one call to Join() has named it,
         and will remain in this map until all its children have been removed by calls to Part().  Therefore,
         this map contains only those (and all those) parent points of view who should be playing tetris. */
      std::unordered_map<Base::TUuid, TPlayer *> PlayerByParentPovId;

      /* The ids of the parent points of view which are currently paused. */
      std::unordered_set<Base::TUuid> PausedSet;

      /* TODO */
      bool IsMaster;
      //std::mutex MasterLock;
      Indy::Fiber::TFiberLock MasterFiberMutex;

    };  // TTetrisManager

  }  // Server

}  // Stig
