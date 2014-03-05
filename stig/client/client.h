/* <stig/client/client.h>

   The client end of the full-duplex RPC connection.

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
#include <chrono>
#include <memory>
#include <thread>

#include <base/event_semaphore.h>
#include <base/fd.h>
#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <base/uuid.h>
#include <io/device.h>
#include <rpc/rpc.h>
#include <socket/address.h>
#include <stig/closure.h>
#include <stig/method_result.h>

namespace Stig {

  namespace Client {

    /* The client end of the full-duplex RPC connection. */
    class TClient
        : public Rpc::TContext {
      NO_COPY_SEMANTICS(TClient);
      public:

      /* TODO */
      virtual ~TClient();

      /* TODO */
      std::shared_ptr<Rpc::TFuture<std::string>> Echo(const std::string &msg);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> InstallPackage(const std::vector<std::string> &package_name, uint64_t version);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> UnInstallPackage(const std::vector<std::string> &package_name, uint64_t version);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<Base::TUuid>> NewFastPrivatePov(const Base::TOpt<Base::TUuid> &parent_pov_id,
                                                                   const std::chrono::seconds &ttl = std::chrono::seconds(600));

      /* TODO */
      std::shared_ptr<Rpc::TFuture<Base::TUuid>> NewSafePrivatePov(const Base::TOpt<Base::TUuid> &parent_pov_id,
                                                                   const std::chrono::seconds &ttl = std::chrono::seconds(600));

      /* TODO */
      std::shared_ptr<Rpc::TFuture<Base::TUuid>> NewFastSharedPov(const Base::TOpt<Base::TUuid> &parent_pov_id,
                                                                  const std::chrono::seconds &ttl = std::chrono::seconds(600));

      /* TODO */
      std::shared_ptr<Rpc::TFuture<Base::TUuid>> NewSafeSharedPov(const Base::TOpt<Base::TUuid> &parent_pov_id,
                                                                  const std::chrono::seconds &ttl = std::chrono::seconds(600));

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> PausePov(const Base::TUuid &pov_id);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> UnpausePov(const Base::TUuid &pov_id);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> SetUserId(const Base::TUuid &user_id);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> SetTimeToLive(const Base::TUuid &durable_id, const std::chrono::seconds &time_to_live);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<TMethodResult>> Try(const Base::TUuid &pov_id, const std::vector<std::string> &fq_name, const TClosure &closure);

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> BeginImport();

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> EndImport();

      /* TODO */
      std::shared_ptr<Rpc::TFuture<void>> TailGlobalPov();

      /* TODO */
      std::shared_ptr<Rpc::TFuture<std::string>> ImportCoreVector(const std::string &file_pattern, int64_t num_load_threads, int64_t num_merge_threads, int64_t merge_simultaneous);

      /* TODO */
      const Base::TOpt<Base::TUuid> &GetSessionId() const {
        assert(this);
        return SessionId;
      }

      /* TODO */
      const std::chrono::seconds &GetTimeToLive() const {
        assert(this);
        return TimeToLive;
      }

      protected:

      /* TODO */
      TClient(const Socket::TAddress &server_address, const Base::TOpt<Base::TUuid> &session_id, const std::chrono::seconds &time_to_live);

      /* TODO */
      virtual void OnPovFailed(const Base::TUuid &repo_id) = 0;

      /* TODO */
      virtual void OnUpdateAccepted(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) = 0;

      /* TODO */
      virtual void OnUpdateReplicated(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) = 0;

      /* TODO */
      virtual void OnUpdateDurable(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) = 0;

      /* TODO */
      virtual void OnUpdateSemiDurable(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) = 0;

      private:

      /* TODO */
      class TProtocol
          : public Rpc::TProtocol {
        NO_COPY_SEMANTICS(TProtocol);
        public:

        /* TODO */
        static const TProtocol Protocol;

        private:

        /* TODO */
        TProtocol();

      };  // TClient::TProtocol

      /* TODO */
      void DispatchMain();

      /* Handles background I/O with the server. */
      void IoMain();

      /* TODO */
      Socket::TAddress ServerAddress;

      /* TODO */
      Base::TOpt<Base::TUuid> SessionId;

      /* TODO */
      std::chrono::seconds TimeToLive;

      /* TODO */
      std::shared_ptr<Io::TDevice> Device;

      /* TODO */
      Base::TFd InternalSocket;

      /* Runs IoMain(). */
      std::thread DispatchThread, IoThread;

      /* TODO */
      Base::TEventSemaphore Destructing;

    };  // TClient

  }  // Client

}  // Stig
