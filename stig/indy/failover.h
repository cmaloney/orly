/* <stig/indy/failover.h>

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
#include <condition_variable>
#include <mutex>

#include <base/no_copy_semantics.h>
#include <base/uuid.h>
#include <io/device.h>
#include <rpc/rpc.h>
#include <stig/indy/file_sync.h>
#include <stig/indy/replication.h>
#include <stig/indy/sequence_number.h>
#include <stig/indy/util/context_streamer.h>

namespace Stig {

  namespace Indy {

    /* TEMP */
    class TUpdateContext {
      public:

      void Write(Io::TBinaryOutputStream &/*strm*/) const {}

      void Read(Io::TBinaryInputStream &/*strm*/) {}

    };

    /* TODO */
    class TConnectionFailed
        : public std::runtime_error {
      public:

      /* Do-little. */
      TConnectionFailed(const std::string &error_msg);

    };  // TConnectionFailed

    /* TODO */
    class TCommonContext
        : public Rpc::TContext {
      NO_COPY_SEMANTICS(TCommonContext);
      public:

      /* TODO */
      static const Rpc::TEntryId PingId = 3001;

      /* TODO */
      virtual ~TCommonContext();

      /* TODO */
      void Shutdown();

      /* TODO */
      virtual bool Queue();

      /* TODO */
      virtual bool Work();

      /* TODO */
      virtual void Join();

      protected:

      /* TODO */
      class TProtocol
          : public Rpc::TProtocol {
        NO_COPY_SEMANTICS(TProtocol);
        protected:

        /* TODO */
        TProtocol() {
          Register<TCommonContext, void>(PingId, &TCommonContext::Ping);
        }

      };  // TProtocol

      /* TODO */
      TCommonContext(const Rpc::TProtocol &protocol, const Base::TFd &fd);

      /* TODO */
      std::mutex Mutex;

      /* TODO */
      Base::TEventSemaphore Sem;

      /* TODO */
      std::queue<std::shared_ptr<const Rpc::TAnyRequest>> RequestQueue;

      private:

      /* TODO */
      enum TReaderState {Connected, NotConnected};

      /* TODO */
      enum TWorkerState {Running, Exited, ExitedOnWriteFailure};

      /* TODO */
      virtual void Ping() = 0;

      /* TODO */
      Base::TFd Fd;

      /* TODO */
      std::shared_ptr<Io::TDevice> Device;

      /* TODO */
      TReaderState ReaderState;

      /* TODO */
      TWorkerState WorkerState;
      std::mutex WorkerMutex;
      Base::TEventSemaphore WorkerStateChanged;

    };  // TCommonContext

    /* TODO */
    class TMasterContext
        : public TCommonContext {
      NO_COPY_SEMANTICS(TMasterContext);
      public:

      /* TODO */
      static const Rpc::TEntryId FetchUpdatesId = 3500;
      static const Rpc::TEntryId NotifyFinishSyncInventoryId = 3501;
      static const Rpc::TEntryId GetViewId = 3502;
      static const Rpc::TEntryId SyncFileId = 3503;

      /* Low Seq, High Seq, GenId, NumKeys */
      typedef std::tuple<TSequenceNumber, TSequenceNumber, size_t, size_t> TFileTuple;
      typedef std::vector<TFileTuple> TViewDef;

      protected:

      /* TODO */
      class TProtocol
          : public TCommonContext::TProtocol {
        NO_COPY_SEMANTICS(TProtocol);
        public:

        /* TODO */
        static const TProtocol Protocol;

        private:

        /* TODO */
        TProtocol() {
          Register<TMasterContext, Util::TContextInputStreamer, Base::TUuid, TSequenceNumber, TSequenceNumber>(FetchUpdatesId, &TMasterContext::FetchUpdates);
          Register<TMasterContext, void>(NotifyFinishSyncInventoryId, &TMasterContext::NotifyFinishSyncInventory);
          Register<TMasterContext, TViewDef, Base::TUuid>(GetViewId, &TMasterContext::GetView);
          Register<TMasterContext, TFileSync, Base::TUuid, size_t, size_t>(SyncFileId, &TMasterContext::SyncFile);
        }

      };  // TProtocol

      /* TODO */
      TMasterContext(const Base::TFd &fd);

      /* TODO */
      virtual ~TMasterContext();

      /* TODO */
      virtual Util::TContextInputStreamer FetchUpdates(const Base::TUuid &repo_id, TSequenceNumber lowest, TSequenceNumber highest) = 0;

      /* TODO */
      virtual void NotifyFinishSyncInventory() = 0;

      /* TODO */
      virtual TViewDef GetView(const Base::TUuid &repo_id) = 0;

      /* TODO */
      virtual TFileSync SyncFile(const Base::TUuid &file_id, size_t gen_id, size_t context) = 0;

    };  // TMasterContext

    /* TODO */
    class TSlaveContext
        : public TCommonContext {
      NO_COPY_SEMANTICS(TSlaveContext);
      public:

      /* TODO */
      static const Rpc::TEntryId InventoryId = 3100;
      static const Rpc::TEntryId PushNotificationsId = 3101;
      static const Rpc::TEntryId TransitionToSlaveId = 3102;
      static const Rpc::TEntryId SyncInventoryId = 3103;
      static const Rpc::TEntryId IndexId = 3104;

      protected:

      /* TODO */
      class TProtocol
          : public TCommonContext::TProtocol {
        NO_COPY_SEMANTICS(TProtocol);
        public:

        /* TODO */
        static const TProtocol Protocol;

        private:

        /* TODO */
        TProtocol() {
          Register<TSlaveContext, void,
            Base::TUuid,
            size_t,
            Base::TOpt<Base::TUuid>,
            bool,
            Base::TOpt<TSequenceNumber>,
            Base::TOpt<TSequenceNumber>,
            TSequenceNumber>(InventoryId, &TSlaveContext::Inventory);
          Register<TSlaveContext, void, TIndexMapReplica>(IndexId, &TSlaveContext::Index);
          Register<TSlaveContext, void, TReplicationStreamer>(PushNotificationsId, &TSlaveContext::PushNotifications);
          Register<TSlaveContext, void>(TransitionToSlaveId, &TSlaveContext::TransitionToSlave);
          Register<TSlaveContext, void>(SyncInventoryId, &TSlaveContext::ScheduleSyncInventory);
        }

      };  // TProtocol

      /* TODO */
      TSlaveContext(const Base::TFd &fd);

      /* TODO */
      virtual ~TSlaveContext();

      /* TODO */
      virtual void Inventory(const Base::TUuid &repo_id,
                             size_t ttl,
                             const Base::TOpt<Base::TUuid> &parent_repo_id,
                             bool is_safe,
                             const Base::TOpt<TSequenceNumber> &lowest,
                             const Base::TOpt<TSequenceNumber> &highest,
                             TSequenceNumber) = 0;

      /* TODO */
      virtual void Index(const TIndexMapReplica &index_map_replica) = 0;

      /* TODO */
      virtual void PushNotifications(const TReplicationStreamer &replication_streamer) = 0;

      /* TODO */
      virtual void TransitionToSlave() = 0;

      /* TODO */
      virtual void ScheduleSyncInventory() = 0;

    };  // TSlaveContext

    /* Binary streamers for Stig::Indy::TUpdateContext */
    inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TUpdateContext &context) { context.Write(strm); return strm; }
    inline Io::TBinaryOutputStream &&operator<<(Io::TBinaryOutputStream &&strm, const TUpdateContext &context) { context.Write(strm); return std::move(strm); }
    inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TUpdateContext &context) { context.Read(strm); return strm; }
    inline Io::TBinaryInputStream &&operator>>(Io::TBinaryInputStream &&strm, TUpdateContext &context) { context.Read(strm); return std::move(strm); }

  }  // Indy

}  // Stig