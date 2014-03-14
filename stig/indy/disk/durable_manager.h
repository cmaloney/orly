/* <stig/indy/disk/durable_manager.h>

   TODO

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

#include <cstdint>
#include <functional>

#include <base/epoll.h>
#include <base/event_counter.h>
#include <base/event_semaphore.h>
#include <base/no_copy_semantics.h>
#include <base/scheduler.h>
#include <base/time.h>
#include <base/timer_fd.h>
#include <base/uuid.h>
#include <inv_con/unordered_list.h>
#include <stig/durable/kit.h>
#include <stig/indy/disk/in_file.h>
#include <stig/indy/disk/out_stream.h>
#include <stig/indy/disk/util/engine.h>
#include <stig/indy/disk/util/index_manager.h>
#include <stig/indy/replication.h>
#include <stig/indy/util/block_vec.h>
#include <stig/indy/util/lockless_pool.h>
#include <stig/indy/util/pool.h>

namespace Stig {

  namespace Server {
    /* Forward Declarations. */
    class TIndyReporter;
  }

  namespace Indy {

    namespace Disk {

      namespace Util {

        class TDiskEngine;

      }

    }

    /* TODO */
    namespace DurableManager {

      /* TODO */
      class TManager
          : public Fiber::TRunnable {
        NO_COPY_SEMANTICS(TManager);
        public:

        /* TODO */
        virtual TDurableReplication *NewDurableReplication(const Durable::TId &id, const Durable::TTtl &ttl, const std::string &serialized_form) const = 0;

        /* TODO */
        virtual void DeleteDurableReplication(TDurableReplication *durable_replication) NO_THROW = 0;

        /* TODO */
        virtual void EnqueueDurable(TDurableReplication *durable_replication) NO_THROW = 0;

        protected:

        /* TODO */
        TManager() {}

        /* TODO */
        virtual ~TManager() {}

      };  // TManager

    }  // DurableManager

    namespace Disk {

      /* TODO */
      class TDurableManager
          : public Durable::TManager {
        NO_COPY_SEMANTICS(TDurableManager);
        private:

        /* Forward Declarations. */
        class TDurableLayer;

        /* TODO */
        typedef InvCon::UnorderedList::TCollection<TDurableManager, TDurableLayer> TRemovalCollection;

        public:

        /* TODO */
        typedef uint64_t TSequenceNumber;
        typedef uint32_t TSerializedSize;

        /* TODO */
        typedef TStream<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage, 64UL> TInStream;
        typedef TOutStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::PageCheckedBlock> TDataOutStream;

        /* TODO */
        enum TOutcome { Survived, Expired, WasSuperceded };

        /* TODO */
        using TNotify = std::function<void (TSequenceNumber, Base::TUuid, TOutcome)>;

        /* TODO */
        TDurableManager(Base::TScheduler *scheduler,
                        Fiber::TRunner::TRunnerCons &runner_cons,
                        Base::TThreadLocalGlobalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
                        DurableManager::TManager *manager,
                        Util::TEngine *engine,
                        size_t max_cache_size,
                        size_t write_delay,
                        size_t merge_delay,
                        size_t layer_cleaning_interval_milliseconds,
                        size_t temp_file_consol_thresh,
                        bool create,
                        const TNotify *notify = nullptr);

        /* TODO */
        virtual ~TDurableManager();

        /* TODO */
        virtual bool CanLoad(const Durable::TId &id) override;

        /* TODO */
        virtual void CleanDisk(const Durable::TDeadline &now, Durable::TSem *sem) override;

        /* TODO */
        virtual void Delete(const Durable::TId &id, Durable::TSem *sem) override;

        /* TODO */
        virtual void RunLayerCleaner() override;

        /* TODO */
        virtual void Save(const Durable::TId &id, const Durable::TDeadline &deadline, const TTtl &ttl, const std::string &serialized_form, Durable::TSem *sem) override;

        /* TODO */
        virtual bool TryLoad(const Durable::TId &id, std::string &serialized_form_out) override;

        /* TODO */
        void RunWriter();

        /* TODO */
        void RunMerger();

        /* TODO */
        void static InitMappingPool(size_t num_obj) {
          TMapping::Pool.Init(num_obj);
        }

        /* TODO */
        static constexpr size_t GetMappingSize() {
          return sizeof(TMapping);
        }

        /* TODO */
        void static InitMappingEntryPool(size_t num_obj) {
          TMapping::TEntry::Pool.Init(num_obj);
        }

        /* TODO */
        static constexpr size_t GetMappingEntrySize() {
          return sizeof(TMapping::TEntry);
        }

        /* TODO */
        void static InitDurableLayerPool(size_t num_obj) {
          TDurableLayer::Pool.Init(num_obj);
        }

        /* TODO */
        static constexpr size_t GetDurableLayerSize() {
          return sizeof(TDurableLayer);
        }

        /* TODO */
        void static InitMemEntryPool(size_t num_obj) {
          TMemSlushLayer::TDurableEntry::Pool.Init(num_obj);
        }

        /* TODO */
        static constexpr size_t GetMemEntrySize() {
          return sizeof(TMemSlushLayer::TDurableEntry);
        }

        /* TODO */
        static const Base::TUuid DurableByIdFileId;

        private:

        /* Forward Declarations. */
        class TMemSlushLayer;
        class TMergeSortedByIdFile;

        /* TODO */
        class TSortedByIdFile {
          NO_COPY_SEMANTICS(TSortedByIdFile);
          public:

          /* TODO */
          TSortedByIdFile(TMemSlushLayer *mem_layer,
                          Util::TEngine *engine,
                          Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                          size_t gen_id,
                          size_t latest_deadline_count,
                          size_t temp_file_consol_thresh,
                          DiskPriority priority);

          /* TODO */
          inline size_t GetNumDurable() const;

          /*
             1. NumEntries
             2. NumBlocks
             3. HashIndexOffset
             4. HashFieldSize
          */

          static const size_t NumMetaFields = 4UL;
          /*
             1. durable id
             2. seq_num
             3. deadline count
             4. size of serialized string.
          */
            static const size_t DurableEntrySize = sizeof(uuid_t) + sizeof(TSequenceNumber) + sizeof(size_t) + sizeof(TSerializedSize);

          static const size_t HashEntrySize = sizeof(uuid_t) + sizeof(size_t);

          static const Base::TUuid NullId;

          private:

          /* TODO */
          class THashObj {
            public:

            /* TODO */
            THashObj(const uuid_t &id, size_t hash, size_t offset)
                : Hash(hash), Offset(offset) {
              uuid_copy(Id, id);
            }

            /* TODO */
            bool operator<(const THashObj &that) const {
              return Hash < that.Hash;
            }

            /* TODO */
            uuid_t Id;

            /* TODO */
            size_t Hash;

            /* TODO */
            size_t Offset;

          };  // THashObj

          /* TODO */
          typedef Util::TIndexManager<THashObj, Disk::Util::SortBufSize, Disk::Util::SortBufMinParallelSize> THashSorter;

          template <typename TOwner>
          class TMyFileTemp
              : public TInFile {
            NO_COPY_SEMANTICS(TMyFileTemp);
            public:

            /* TODO */
            TMyFileTemp(TOwner *file) : File(file) {}

            /* TODO */
            virtual ~TMyFileTemp() {}

            private:

            /* TODO */
            virtual size_t GetFileLength() const override {
              assert(this);
              return File->FileSize;
            }

            /* TODO */
            virtual size_t GetStartingBlock() const override {
              assert(this);
              assert(File->BlockVec.Size() > 0);
              return File->BlockVec.Front();
            }

            /* TODO */
            virtual void ReadMeta(size_t , size_t &) const override {
              throw;
            }

            /* TODO */
            virtual size_t FindPageIdOfByte(size_t offset) const override {
              assert(this);
              assert(offset <= GetFileLength());
              return ((File->BlockVec[offset / Disk::Util::LogicalBlockSize]) * Disk::Util::PagesPerBlock) + ((offset % Disk::Util::LogicalBlockSize) / Disk::Util::LogicalPageSize);
            }

            /* TODO */
            TOwner *File;

          };  // TMyFileTemp

          /* TODO */
          typedef TMyFileTemp<TSortedByIdFile> TMyFile;

          /* TODO */
          Util::TEngine *const Engine;

          /* TODO */
          const Disk::Util::TVolume::TDesc::TStorageSpeed StorageSpeed;

          /* TODO */
          Indy::Util::TBlockVec BlockVec;
          size_t FileSize;

          /* TODO */
          size_t NumDurable;

          /* TODO */
          friend class TMergeSortedByIdFile;

        };  // TSortedByIdFile

        /* TODO */
        class TMergeSortedByIdFile {
          NO_COPY_SEMANTICS(TMergeSortedByIdFile);
          public:

          /* TODO */
          TMergeSortedByIdFile(const std::vector<size_t> &gen_vec,
                               Util::TEngine *engine,
                               Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                               size_t gen_id,
                               size_t latest_deadline_count,
                               size_t temp_file_consol_thresh,
                               DiskPriority priority,
                               const TNotify *notify);


          /* TODO */
          inline size_t GetNumDurable() const;

          private:

          /* TODO */
          typedef TSortedByIdFile::THashObj THashObj;

          /* TODO */
          typedef TSortedByIdFile::THashSorter THashSorter;

          /* TODO */
          typedef TSortedByIdFile::TMyFileTemp<TMergeSortedByIdFile> TMyFile;

          /* TODO */
          Util::TEngine *Engine;

          /* TODO */
          Disk::Util::TVolume::TDesc::TStorageSpeed StorageSpeed;

          /* TODO */
          size_t NumDurable;

          /* TODO */
          Indy::Util::TBlockVec BlockVec;
          size_t FileSize;

          /* TODO */
          static const size_t LocalCacheSize = 64;

          /* TODO */
          friend TMyFile;

        };  // TMergeSortedByIdFile

        /* TODO */
        class TSortedInFile
            : public TInFile {
          NO_COPY_SEMANTICS(TSortedInFile);
          public:

          /* TODO */
          TSortedInFile(Util::TPageCache *page_cache,
                        DiskPriority priority,
                        size_t gen_id,
                        size_t starting_block_id,
                        size_t starting_block_offset,
                        size_t file_length);

          /* TODO */
          TSortedInFile(Util::TEngine *engine, DiskPriority priority, size_t gen_id);

          /* TODO */
          virtual ~TSortedInFile();

          /* TODO */
          virtual size_t GetFileLength() const override;

          /* TODO */
          virtual size_t GetStartingBlock() const override;

          /* TODO */
          virtual void ReadMeta(size_t offset, size_t &out) const override;

          /* TODO */
          virtual size_t FindPageIdOfByte(size_t offset) const override;

          /* TODO */
          inline size_t GetNumEntries() const;

          /* TODO */
          inline size_t GetNumBlocks() const;

          /* TODO */
          inline size_t GetStartOfDurableByIdIndex() const;

          /* TODO */
          inline size_t GetStartOfHashIndex() const;

          /* TODO */
          void FindInHash(TSequenceNumber &cur_max_seq_num, const Durable::TId &id, std::string &serialized_form_out) const;

          private:

          /* TODO */
          Util::TPageCache *PageCache;

          /* TODO */
          size_t FileLength;

          /* TODO */
          size_t StartingBlockId;

          /* TODO */
          size_t StartingBlockOffset;

          /* TODO */
          size_t NumEntries;

          /* TODO */
          size_t NumBlocks;

          /* TODO */
          size_t HashIndexOffset;

          /* TODO */
          size_t HashFieldSize;

          /* TODO */
          std::unique_ptr<TInStream> InStream;

          /* TODO */
          static const size_t LocalCacheSize = 64;

        };  // TSortedInFile

        /* TODO */
        class TMapping {
          NO_COPY_SEMANTICS(TMapping);
          public:

          /* Forward Declarations. */
          class TEntry;

          /* TODO */
          typedef InvCon::UnorderedList::TMembership<TMapping, TDurableManager> TManagerMembership;

          /* TODO */
          typedef InvCon::UnorderedList::TCollection<TMapping, TEntry> TEntryCollection;

          /* TODO */
          class TEntry {
            NO_COPY_SEMANTICS(TEntry);
            public:

            /* TODO */
            typedef InvCon::UnorderedList::TMembership<TEntry, TMapping> TMappingMembership;

            /* TODO */
            inline TEntry(TMapping *mapping, TDurableLayer *layer);

            /* TODO */
            inline ~TEntry();

            /* TODO */
            inline TDurableLayer *GetLayer() const;

            /* TODO */
            static void *operator new(size_t size) {
              return Pool.Alloc(size);
            }

            /* TODO */
            static void operator delete(void *ptr, size_t) {
              Pool.Free(ptr);
            }

            private:

            /* TODO */
            TMappingMembership::TImpl MappingMembership;

            /* TODO */
            TDurableLayer *Layer;

            /* TODO */
            static Indy::Util::TLocklessPool Pool;

            /* TODO */
            friend class TDurableManager;
            friend class Server::TIndyReporter;

          };  // TEntry

          /* TODO */
          class TView {
            NO_COPY_SEMANTICS(TView);
            public:

            /* TODO */
            TView(TDurableManager *manager)
                : Manager(manager) {
              std::lock_guard<std::mutex> data_lock(Manager->DataLock);
              std::lock_guard<std::mutex> mapping_lock(Manager->MappingLock);
              Mapping = Manager->MappingCollection.TryGetLastMember();
              CurMemLayer = Manager->CurMemoryLayer;
              assert(CurMemLayer);
              CurMemLayer->Incr();
              Mapping->Incr();
            }

            /* TODO */
            ~TView() {
              CurMemLayer->Decr();
              std::lock_guard<std::mutex> mapping_lock(Manager->MappingLock);
              Mapping->Decr();
            }

            /* TODO */
            inline TMapping *GetMapping() const;

            /* TODO */
            inline TMemSlushLayer *GetCurLayer() const;

            private:

            /* TODO */
            TDurableManager *Manager;

            /* TODO */
            TMapping *Mapping;

            /* TODO */
            TMemSlushLayer *CurMemLayer;

          };  // TView

          /* TODO */
          inline TMapping(TDurableManager *manager);

          /* TODO */
          ~TMapping();

          /* TODO */
          inline void Incr();

          /* TODO */
          inline void Decr();

          /* TODO */
          inline size_t GetRefCount() const;

          /* TODO */
          TEntryCollection *GetEntryCollection() const;

          /* TODO */
          static void *operator new(size_t size) {
            return Pool.Alloc(size);
          }

          /* TODO */
          static void operator delete(void *ptr, size_t) {
            Pool.Free(ptr);
          }

          private:

          /* TODO */
          TManagerMembership::TImpl ManagerMembership;

          /* TODO */
          mutable TEntryCollection::TImpl EntryCollection;

          /* TODO */
          size_t RefCount;

          /* TODO */
          bool MarkedForDelete;

          /* TODO */
          static Indy::Util::TLocklessPool Pool;

          /* TODO */
          friend class TDurableManager;
          friend class Server::TIndyReporter;

        };  // TMapping

        /* TODO */
        typedef InvCon::UnorderedList::TCollection<TDurableManager, TMapping> TMappingCollection;

        /* TODO */
        class TDurableLayer {
          NO_COPY_SEMANTICS(TDurableLayer);
          public:

          /* TODO */
          typedef InvCon::UnorderedList::TMembership<TDurableLayer, TDurableManager> TRemovalMembership;

          /* TODO */
          enum TKind {MemSlush, DiskOrdered};

          /* TODO */
          inline virtual ~TDurableLayer();

          /* TODO */
          inline void Incr();

          /* TODO */
          inline void Decr();

          /* TODO */
          inline void RemoveFromCollection();

          /* TODO */
          virtual TKind GetKind() const = 0;

          /* TODO */
          inline bool GetMarkedTaken() const;

          /* TODO */
          inline void MarkTaken();

          /* TODO */
          inline bool GetMarkedForDelete() const;

          /* TODO */
          inline void MarkForDelete();

          /* TODO */
          virtual void FindMax(TSequenceNumber &cur_max_seq, const Base::TUuid &id, std::string &serialized_form_out) const = 0;

          /* TODO */
          static void *operator new(size_t size) {
            return Pool.Alloc(size);
          }

          /* TODO */
          static void operator delete(void *ptr, size_t) {
            Pool.Free(ptr);
          }

          protected:

          /* TODO */
          inline TDurableLayer(TDurableManager *manager);

          private:

          /* TODO */
          TDurableManager *Manager;

          /* TODO */
          TRemovalMembership::TImpl RemovalMembership;

          /* TODO */
          size_t RefCount;

          /* TODO */
          bool MarkedForDelete;

          /* TODO */
          bool MarkedTaken;

          /* TODO */
          static Indy::Util::TPool Pool;

          /* TODO */
          friend class TDurableManager;
          friend class Server::TIndyReporter;

        };  // TDurableLayer

        /* TODO */
        void AddMapping(TDurableLayer *layer);

        /* TODO */
        class TMemSlushLayer
            : public TDurableLayer {
          NO_COPY_SEMANTICS(TMemSlushLayer);
          public:

          /* TODO */
          class TDurableEntry {
            NO_COPY_SEMANTICS(TDurableEntry);
            public:

            /* TODO */
            class TKey {
              public:

              /* TODO */
              inline TKey(const uuid_t &id, TSequenceNumber seq_num);

              /* TODO */
              inline bool operator==(const TKey &that) const;

              /* TODO */
              inline bool operator!=(const TKey &that) const;

              /* TODO */
              inline bool operator<=(const TKey &that) const;

              private:

              /* TODO */
              uuid_t Id;

              /* TODO */
              const TSequenceNumber SeqNum;

              /* TODO */
              friend class TDurableEntry;

            };  // TKey

            /* TODO */
            typedef InvCon::OrderedList::TMembership<TDurableEntry, TMemSlushLayer, TDurableEntry::TKey> TSlushMembership;

            /* TODO */
            inline TDurableEntry(TMemSlushLayer *mem_layer, const Base::TUuid &id, const Durable::TDeadline &deadline, std::string &&serialized_form, TSequenceNumber seq_num);

            /* TODO */
            inline const uuid_t &GetId() const;

            /* TODO */
            inline TSequenceNumber GetSeqNum() const;

            /* TODO */
            inline size_t GetDeadlineCount() const;

            /* TODO */
            inline TSerializedSize GetSerializedSize() const;

            /* TODO */
            inline const std::string &GetSerializedForm() const;

            /* TODO */
            static void *operator new(size_t size) {
              return Pool.Alloc(size);
            }

            /* TODO */
            static void operator delete(void *ptr, size_t) {
              Pool.Free(ptr);
            }

            private:

            /* TODO */
            TSlushMembership::TImpl SlushMembership;

            /* TODO */
            const size_t DeadlineCount;

            /* TODO */
            std::string SerializedForm;

            /* TODO */
            static Indy::Util::TPool Pool;

            /* TODO */
            friend class TDurableManager;
            friend class Server::TIndyReporter;

          };  // TDurableEntry

          /* TODO */
          typedef InvCon::OrderedList::TCollection<TMemSlushLayer, TDurableEntry, TDurableEntry::TKey> TEntryCollection;

          /* TODO */
          virtual TKind GetKind() const {
            assert(this);
            return TDurableLayer::MemSlush;
          }

          /* TODO */
          inline TMemSlushLayer(TDurableManager *manager);

          /* TODO */
          virtual ~TMemSlushLayer();

          /* TODO */
          inline size_t GetNumEntries() const;

          /* TODO */
          inline size_t GetTotalSerializedSize() const;

          /* TODO */
          inline TEntryCollection *GetEntryCollection() const;

          /* TODO */
          virtual void FindMax(TSequenceNumber &cur_max_seq, const Base::TUuid &id, std::string &serialized_form_out) const;

          private:

          /* TODO */
          mutable TEntryCollection::TImpl EntryCollection;

          /* TODO */
          size_t NumEntries;

          /* TODO */
          size_t TotalSerializedSize;

          /* TODO */
          friend class TDurableManager;

        };  // TMemSlushLayer

        /* TODO */
        class TDiskOrderedLayer
            : public TDurableLayer {
          NO_COPY_SEMANTICS(TDiskOrderedLayer);
          public:

          /* TODO */
          virtual TKind GetKind() const {
            assert(this);
            return TDurableLayer::DiskOrdered;
          }

          /* TODO */
          TDiskOrderedLayer(TDurableManager *manager, Util::TEngine *engine, size_t gen_id, size_t num_durable);

          /* TODO */
          virtual ~TDiskOrderedLayer();

          /* TODO */
          inline size_t GetGenId() const;

          /* TODO */
          inline size_t GetNumDurable() const;

          /* TODO */
          virtual void FindMax(TSequenceNumber &cur_max_seq, const Base::TUuid &id, std::string &serialized_form_out) const;

          private:

          /* TODO */
          Util::TEngine *const Engine;

          /* TODO */
          size_t GenId;

          /* TODO */
          size_t NumDurable;

        };  // TDiskOrderedLayer

        /* TODO */
        DurableManager::TManager *const Manager;

        /* TODO */
        Fiber::TRunner MergerScheduler;
        Fiber::TRunner WriterScheduler;
        Fiber::TFrame *MergerFrame;
        Fiber::TFrame *WriterFrame;
        Fiber::TSingleSem MergerFinishedSem;
        Fiber::TSingleSem WriterFinishedSem;

        /* TODO */
        Util::TEngine *const Engine;

        /* TODO */
        std::mutex DataLock;
        TMemSlushLayer *CurMemoryLayer;

        /* TODO */
        TSequenceNumber SeqNum;

        /* TODO */
        size_t NextSlushGenId;

        /* TODO */
        size_t NextDurableByIdGenId;

        /* TODO */
        size_t TempFileConsolThresh;

        /* TODO */
        size_t DurableWriteDelay;
        size_t DurableMergeDelay;

        /* TODO */
        bool ShutDown;
        Fiber::TSingleSem SlushSem;
        Fiber::TSingleSem MergeSem;

        /* TODO */
        mutable TMappingCollection::TImpl MappingCollection;

        /* TODO */
        mutable TRemovalCollection::TImpl RemovalCollection;
        Base::TTimerFd LayerCleanerTimer;

        std::mutex RemovalLock;

        /* TODO */
        std::mutex MappingLock;

        /* TODO */
        const TNotify *Notify;

        /* TODO */
        friend class Server::TIndyReporter;
        friend class Util::TDiskEngine;

      };  // TDurableManager

      /***************
        *** Inline ***
        *************/

      inline size_t TDurableManager::TSortedInFile::GetNumEntries() const {
        return NumEntries;
      }

      inline size_t TDurableManager::TSortedInFile::GetNumBlocks() const {
        return NumBlocks;
      }

      inline size_t TDurableManager::TSortedInFile::GetStartOfDurableByIdIndex() const {
        return (TSortedByIdFile::NumMetaFields + GetNumBlocks()) * sizeof(size_t);
      }

      inline size_t TDurableManager::TSortedInFile::GetStartOfHashIndex() const {
        return HashIndexOffset;
      }

      inline size_t TDurableManager::TSortedByIdFile::GetNumDurable() const {
        assert(this);
        return NumDurable;
      }

      inline size_t TDurableManager::TMergeSortedByIdFile::GetNumDurable() const {
        assert(this);
        return NumDurable;
      }

      inline TDurableManager::TMapping::TMapping(TDurableManager *manager)
          : ManagerMembership(this, &manager->MappingCollection),
          EntryCollection(this),
          RefCount(0U),
          MarkedForDelete(false) {}

      inline void TDurableManager::TMapping::Incr() {
        assert(this);
        __sync_add_and_fetch(&RefCount, 1U);
      }

      inline void TDurableManager::TMapping::Decr() {
        assert(this);
        size_t count = __sync_sub_and_fetch(&RefCount, 1U);
        if (this != ManagerMembership.TryGetCollection()->TryGetLastMember() && count == 0) {
          delete this;
        }
      }

      inline size_t TDurableManager::TMapping::GetRefCount() const {
        assert(this);
        return RefCount;
      }

      inline TDurableManager::TMapping::TEntryCollection *TDurableManager::TMapping::GetEntryCollection() const {
        assert(this);
        return &EntryCollection;
      }

      inline TDurableManager::TMapping::TEntry::TEntry(TMapping *mapping, TDurableLayer *layer)
          : MappingMembership(this, &mapping->EntryCollection),
            Layer(layer) {
        Layer->Incr();
      }

      inline TDurableManager::TMapping::TEntry::~TEntry() {
        assert(this);
        assert(Layer);
        Layer->Decr();
      }

      inline TDurableManager::TDurableLayer *TDurableManager::TMapping::TEntry::GetLayer() const {
        assert(this);
        return Layer;
      }

      inline TDurableManager::TDurableLayer::TDurableLayer(TDurableManager *manager)
        : Manager(manager),
          RemovalMembership(this),
          RefCount(0U),
          MarkedForDelete(false),
          MarkedTaken(false) {}

      inline TDurableManager::TDurableLayer::~TDurableLayer() {}

      inline void TDurableManager::TDurableLayer::Incr() {
        assert(this);
        __sync_add_and_fetch(&RefCount, 1U);
      }

      inline void TDurableManager::TDurableLayer::Decr() {
        assert(this);
        size_t count = __sync_sub_and_fetch(&RefCount, 1U);
        if (MarkedForDelete && count == 0) {
          std::lock_guard<std::mutex> removal_lock(Manager->RemovalLock);
          RemovalMembership.Insert(&Manager->RemovalCollection);
        }
      }

      inline void TDurableManager::TDurableLayer::RemoveFromCollection() {
        assert(this);
        RemovalMembership.Remove();
      }

      inline bool TDurableManager::TDurableLayer::GetMarkedForDelete() const {
        assert(this);
        return MarkedForDelete;
      }

      inline void TDurableManager::TDurableLayer::MarkForDelete() {
        assert(this);
        MarkedForDelete = true;
      }

      inline bool TDurableManager::TDurableLayer::GetMarkedTaken() const {
        assert(this);
        return MarkedTaken;
      }

      inline void TDurableManager::TDurableLayer::MarkTaken() {
        assert(this);
        MarkedTaken = true;
      }

      inline TDurableManager::TMemSlushLayer::TMemSlushLayer(TDurableManager *manager)
          : TDurableLayer(manager), EntryCollection(this), NumEntries(0UL), TotalSerializedSize(0UL) {}

      inline TDurableManager::TMemSlushLayer::~TMemSlushLayer() {
        assert(this);
        EntryCollection.DeleteEachMember();
      }

      inline size_t TDurableManager::TMemSlushLayer::GetNumEntries() const {
        assert(this);
        return NumEntries;
      }

      inline size_t TDurableManager::TMemSlushLayer::GetTotalSerializedSize() const {
        assert(this);
        return TotalSerializedSize;
      }

      inline TDurableManager::TMemSlushLayer::TEntryCollection *TDurableManager::TMemSlushLayer::GetEntryCollection() const {
        assert(this);
        return &EntryCollection;
      }

      inline TDurableManager::TMemSlushLayer::TDurableEntry::TKey::TKey(const uuid_t &id, TSequenceNumber seq_num)
          : SeqNum(seq_num) {
        uuid_copy(Id, id);
      }

      inline bool TDurableManager::TMemSlushLayer::TDurableEntry::TKey::operator==(const TKey &that) const {
        return SeqNum == that.SeqNum && uuid_compare(Id, that.Id) == 0;
      }

      inline bool TDurableManager::TMemSlushLayer::TDurableEntry::TKey::operator!=(const TKey &that) const {
        return SeqNum != that.SeqNum || uuid_compare(Id, that.Id) != 0;
      }

      inline bool TDurableManager::TMemSlushLayer::TDurableEntry::TKey::operator<=(const TKey &that) const {
        int uuid_comp = uuid_compare(Id, that.Id);
        return uuid_comp < 0 || (uuid_comp == 0 && SeqNum > that.SeqNum);
      }

      inline TDurableManager::TMemSlushLayer::TDurableEntry::TDurableEntry(TMemSlushLayer *mem_layer,
                                                                           const Base::TUuid &id,
                                                                           const Durable::TDeadline &deadline,
                                                                           std::string &&serialized_form,
                                                                           TSequenceNumber seq_num)
          : SlushMembership(this, TKey(id.GetRaw(), seq_num)),
            DeadlineCount(deadline.time_since_epoch().count()) {
        std::swap(SerializedForm, serialized_form);
        ++(mem_layer->NumEntries);
        mem_layer->TotalSerializedSize += SerializedForm.size();
        SlushMembership.Insert(mem_layer->GetEntryCollection());
      }

      inline const uuid_t &TDurableManager::TMemSlushLayer::TDurableEntry::GetId() const {
        assert(this);
        return SlushMembership.GetKey().Id;
      }

      inline TSequenceNumber TDurableManager::TMemSlushLayer::TDurableEntry::GetSeqNum() const {
        assert(this);
        return SlushMembership.GetKey().SeqNum;
      }

      inline size_t TDurableManager::TMemSlushLayer::TDurableEntry::GetDeadlineCount() const {
        assert(this);
        return DeadlineCount;
      }

      inline TDurableManager::TSerializedSize TDurableManager::TMemSlushLayer::TDurableEntry::GetSerializedSize() const {
        assert(this);
        return SerializedForm.size();
      }

      inline const std::string &TDurableManager::TMemSlushLayer::TDurableEntry::GetSerializedForm() const {
        assert(this);
        return SerializedForm;
      }

      inline TDurableManager::TDiskOrderedLayer::TDiskOrderedLayer(TDurableManager *manager, Util::TEngine *engine, size_t gen_id, size_t num_durable)
          : TDurableLayer(manager), Engine(engine), GenId(gen_id), NumDurable(num_durable) {}

      inline void TDurableManager::TDiskOrderedLayer::FindMax(TSequenceNumber &cur_max_seq, const Base::TUuid &id, std::string &serialized_form_out) const {
        assert(this);
        TSortedInFile(Engine, RealTime, GenId).FindInHash(cur_max_seq, id, serialized_form_out);
      }

      inline TDurableManager::TMapping *TDurableManager::TMapping::TView::GetMapping() const {
        assert(this);
        return Mapping;
      }

      inline TDurableManager::TMemSlushLayer *TDurableManager::TMapping::TView::GetCurLayer() const {
        assert(this);
        return CurMemLayer;
      }

      inline size_t TDurableManager::TDiskOrderedLayer::GetGenId() const {
        assert(this);
        return GenId;
      }

      inline size_t TDurableManager::TDiskOrderedLayer::GetNumDurable() const {
        assert(this);
        return NumDurable;
      }

    }  // Disk

  }  // Indy

}  // Stig