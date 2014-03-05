/* <stig/server/pov.h>

   The durable object representing a point-of-view.

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

#include <vector>

#include <base/uuid.h>
#include <stig/durable/kit.h>
#include <stig/indy/manager.h>

namespace Stig {

  namespace Server {

    /* An open session. */
    class TPov final
        : public Durable::TObj {
      public:

      /* TODO */
      class TServer {
        public:

        virtual ~TServer() {}

        /* TODO */
        virtual const Indy::L0::TManager::TPtr<Indy::TRepo> &GetGlobalRepo() const = 0;

        /* TODO */
        virtual Stig::Indy::TManager *GetRepoManager() const = 0;

        protected:

        TServer() {}

      };  // TServer

      /* The shared parents between this pov and the global one.
         An empty vector means this pov is a direct child of global.
         If non-empty, then element[0] is a direct child of global,
         and this pov is a direct child of element[size() - 1]. */
      using TSharedParents = std::vector<Base::TUuid>;

      /* How many sessions can use this point-of-view? */
      enum class TAudience : char {

        /* This point-of-view belongs to a single session. */
        Private = 'P',

        /* This point-of-view is shared between multiple sessions. */
        Shared = 'H'

      };  // TPov::TAudience

      /* The point-of-view's policy regarding safety vs. speed trade-offs. */
      enum class TPolicy : char {

        /* Favor safety over speed.  Write through to disk aggressively. */
        Safe = 'S',

        /* Favor speed over safety.  Write to disk lazily. */
        Fast = 'F'

      };  // TPov::TPolicy

      /* See TPov::TAudience. */
      TAudience GetAudience() const {
        assert(this);
        return Audience;
      }

      /* TODO */
      virtual const char *GetKind() const noexcept override {
        assert(this);
        return "Pov";
      }

      /* See TPov::TPolicy. */
      TPolicy GetPolicy() const {
        assert(this);
        return Policy;
      }

      /* The repo which backs up this pov. */
      const Indy::L0::TManager::TPtr<Indy::TRepo> &GetRepo(const TServer *server) const;

      /* The id of the session which created this pov. */
      const Base::TUuid &GetSessionId() const {
        assert(this);
        return SessionId;
      }

      /* See TSharedParents. */
      const TSharedParents &GetSharedParents() const {
        assert(this);
        return SharedParents;
      }

      private:

      /* Construct a new pov. */
      TPov(Durable::TManager *manager,
           const Base::TUuid &id,
           const Durable::TTtl &ttl,
           const Base::TUuid &session_id,
           TAudience audience,
           TPolicy policy,
           const TSharedParents &shared_parents);

      /* Open an existing pov. */
      TPov(Durable::TManager *manager, const Base::TUuid &id, Io::TBinaryInputStream &strm);

      /* Do-little. */
      virtual ~TPov();

      /* See TDurable. */
      virtual void Write(Io::TBinaryOutputStream &strm) const override;

      /* See accessor. */
      Base::TUuid SessionId;

      /* See TPov::TAudience. */
      TAudience Audience;

      /* See TPov::TPolicy. */
      TPolicy Policy;

      /* See TSharedParents. */
      TSharedParents SharedParents;

      /* The repo which backs up this pov. */
      mutable Indy::L0::TManager::TPtr<Indy::TRepo> Repo;
      mutable std::mutex RepoLock;

      /* For access to constructors/destructor. */
      friend class Durable::TManager;

    };  // TPov

  }  // Server

}  // Stig
