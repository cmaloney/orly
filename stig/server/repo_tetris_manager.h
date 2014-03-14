/* <stig/server/repo_tetris_manager.h>

   The manager and players of repo-based tetris.

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
#include <memory>
#include <mutex>
#include <unordered_map>

#include <base/no_copy_semantics.h>
#include <stig/indy/context.h>
#include <stig/indy/manager.h>
#include <stig/package/manager.h>
#include <stig/server/meta_record.h>
#include <stig/server/session.h>
#include <stig/server/tetris_manager.h>

namespace Stig {

  namespace Server {

    /* TODO */
    class TRepoTetrisManager final
        : public TTetrisManager {
      public:

      /* TODO */
      TRepoTetrisManager(
          Base::TScheduler *scheduler,
          Indy::Fiber::TRunner::TRunnerCons &runner_cons,
          Base::TThreadLocalGlobalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
          const std::function<void (Indy::Fiber::TRunner *)> &runner_setup_cb,
          bool is_master,
          Indy::TManager *repo_manager,
          Package::TManager *package_manager,
          Durable::TManager *durable_manager,
          bool log_assertion_failures);

      /* TODO */
      virtual ~TRepoTetrisManager();

      /* TODO */
      std::atomic<size_t> PushCount;
      std::atomic<size_t> PopCount;
      std::atomic<size_t> FailCount;
      std::atomic<size_t> RoundCount;

      /* TODO */
      Base::TSigmaCalc TetrisSnapshotCPUTime;
      Base::TSigmaCalc TetrisSortCPUTime;
      Base::TSigmaCalc TetrisPlayCPUTime;
      Base::TSigmaCalc TetrisCommitCPUTime;
      std::mutex TetrisTimerLock;

      private:

      /* TODO */
      class TPlayer final
          : public TTetrisManager::TPlayer {
        public:

        /* TODO */
        TPlayer(TRepoTetrisManager *repo_tetris_manager, const Base::TUuid &parent_pov_id, const Base::TUuid &child_pov_id, bool is_paused, bool is_master);

        /* TODO */
        virtual ~TPlayer();

        private:

        /* TODO */
        class TChild {
          NO_COPY_SEMANTICS(TChild);
          public:

          /* TODO */
          TChild(TPlayer *player, const Base::TUuid &child_pov_id);

          /* TODO */
          bool Play(
              const std::unique_ptr<Indy::L1::TTransaction, std::function<void (Indy::L1::TTransaction *)>> &transaction, Indy::TContext &context);

          /* TODO */
          bool Refresh(const std::unique_ptr<Indy::L1::TTransaction, std::function<void (Indy::L1::TTransaction *)>> &transaction);

          /* TODO */
          static bool SortsBefore(const TChild *lhs, const TChild *rhs);

          private:

          /* TODO */
          void Flush();

          /* TODO */
          bool TestAssertions(Indy::TContext &context) const;

          /* The player which owns us.  Never null. */
          TPlayer *Player;

          /* The number of rounds of tetris we have played. */
          size_t Age;

          /* The number of times we have tested our assertions and failed. */
          size_t FailureCount;

          /* The repo which backs up this child pov. */
          Indy::L0::TManager::TPtr<Indy::TRepo> Repo;

          /* TODO */
          std::shared_ptr<Indy::TUpdate> PeekedUpdate;

          /* TODO */
          TMetaRecord MetaRecord;

          /* TODO */
          std::unordered_map<Base::TUuid, Package::TFuncHolder::TPtr> FuncHolderByUpdateId;

        };  // TRepoTetrisManager::TPlayer::TChild

        /* See TRepoTetrisManager::TPlayer. */
        virtual void OnJoin(const Base::TUuid &child_pov_id) override;

        /* See TRepoTetrisManager::TPlayer. */
        virtual void OnPart(const Base::TUuid &child_pov_id) override;

        /* See TRepoTetrisManager::TPlayer. */
        virtual void OnPause() override;

        /* See TRepoTetrisManager::TPlayer. */
        virtual void OnUnpause() override;

        /* See TRepoTetrisManager::TPlayer. */
        virtual void Play() override;

        /* Our manager.  Never null. */
        TRepoTetrisManager *RepoTetrisManager;

        /* The repo which backs up this parent pov. */
        Indy::L0::TManager::TPtr<Indy::TRepo> Repo;

        /* Covers 'ChildByPovId', below. */
        std::mutex Mutex;

        /* A mapping from a child pov id to the object managing our relationship with that child. */
        std::unordered_map<Base::TUuid, TChild *> ChildByPovId;

      };  // TRepoTetrisManager::TPlayer

      /* TODO */
      virtual TTetrisManager::TPlayer *NewPlayer(const Base::TUuid &parent_pov_id, const Base::TUuid &child_pov_id, bool is_paused, bool is_master) override;

      private:

      /* Our manager of repos.  Never null. */
      Indy::TManager *RepoManager;

      /* Our manager of packages.  Never null. */
      Package::TManager *PackageManager;

      /* Our manager of durables.  Never null. */
      Durable::TManager *DurableManager;

      /* TODO */
      bool LogAssertionFailures;

    };  // TRepoTetrisManager

  }  // Server

}  // Stig
