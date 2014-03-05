/* <stig/indy/disk/util/volume_manager.h>

   TODO

   Copyright 2010-2014 Tagged

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
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <libaio.h>
#include <linux/fs.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <syslog.h>
#include <unistd.h>

#include <base/code_location.h>
#include <base/error_utils.h>
#include <base/event_semaphore.h>
#include <base/likely.h>
#include <base/no_copy_semantics.h>
#include <base/scheduler.h>
#include <base/sigma_calc.h>
#include <base/timer.h>
#include <inv_con/ordered_list.h>
#include <inv_con/unordered_list.h>
#include <inv_con/unordered_multimap.h>
#include <stig/indy/disk/priority.h>
#include <stig/indy/disk/result.h>
#include <stig/indy/disk/util/device_util.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        struct TLogicalExtent {
          const size_t Start;
          const size_t Span;
          inline size_t GetHash() const {
            return Start;
          }
          bool operator==(const TLogicalExtent &that) const {
            return Start == that.Start;
          }
          bool operator<(const TLogicalExtent &that) const {
            return Start < that.Start;
          }
        };

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig

namespace std {

  /* A standard hasher for Stig::Indy::Disk::Util::TLogicalExtent. */
  template <>
  struct hash<Stig::Indy::Disk::Util::TLogicalExtent> {
    typedef size_t result_type;
    typedef Stig::Indy::Disk::Util::TLogicalExtent argument_type;
    size_t operator()(const argument_type &that) const {
      return that.GetHash();
    }
  };

}  // std

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* Forward Declarations. */
        class TVolume;
        class TVolumeManager;

        /* TODO */
        typedef uint64_t TOffset;

        /* TODO */
        typedef std::set<TLogicalExtent> TExtentSet;
        typedef std::pair<size_t, size_t> TBlockRange;

        /* TODO */
        enum TBufKind {
          SectorCheckedBlock,
          PageCheckedBlock,
          CheckedSector,
          CheckedPage,
          CheckedBlock,
          FullSector,
          FullPage,
          FullBlock
        };

        /* TODO */
        enum TCacheInstr {
          CacheAll,
          CachePageOnly,
          CacheBlockOnly,
          ClearAll,
          ClearPageOnly,
          ClearBlockOnly,
          NoCache
        };

        /* TODO */
        constexpr size_t CorruptionDetectionSize = sizeof(uint64_t);

        /* TODO */
        constexpr size_t PhysicalSectorSize = 512UL;

        /* TODO */
        constexpr size_t PhysicalPageSize = 4096UL;

        /* TODO */
        constexpr size_t PhysicalBlockSize = PhysicalPageSize * 16UL;

        /* TODO */
        constexpr size_t SectorsPerBlock = PhysicalBlockSize / PhysicalSectorSize;

        /* TODO */
        constexpr size_t PagesPerBlock = PhysicalBlockSize / PhysicalPageSize;

        /* This is a 512 byte sector with only 1 uint64_t check value at the end. */
        constexpr size_t LogicalSectorSize = PhysicalSectorSize - CorruptionDetectionSize;

        /* This is a 4K page with only 1 uint64_t check value at the end. */
        constexpr size_t LogicalPageSize = PhysicalPageSize - CorruptionDetectionSize;

        /* This is a 64K block with a uint64_t check value at the end of every page.  */
        constexpr size_t LogicalBlockSize = PhysicalBlockSize - (PagesPerBlock * CorruptionDetectionSize);

        /* This is a 64K block with only 1 uint64_t check value at the end. */
        constexpr size_t LogicalCheckedBlockSize = PhysicalBlockSize - CorruptionDetectionSize;

        /* This is how many blocks a write group represents */
        constexpr size_t MaxOutBlockGroupSize = 16UL;

        /* TODO */
        constexpr size_t SortBufSize = 524288;
        constexpr size_t SortBufMinParallelSize = 32768;

        /* TODO */
        constexpr size_t MaxMemHashSize = 524288;

        /* TODO */
        constexpr size_t MaxMergeDiskKeyCache = 262144;

        /* TODO */
        constexpr size_t RemapBufSize = 100000;

        /* TODO */
        constexpr size_t NewRemapBufSize = 100000;

        /* TODO */
        constexpr size_t AccessBufSize = 100000;

        /* TODO */
        constexpr size_t SeqKeeperBufSize = 100000;

        /* TODO */
        constexpr size_t ArenaKeeperBufSize = 100000;

        /* TODO */
        constexpr size_t ScanAhead = 128;

        /* TODO */
        constexpr size_t MaxSegmentsPerIO = 1UL;

        /* TODO */
        static inline size_t GetPhysicalSize(TBufKind buf_kind) {
          switch (buf_kind) {
            case SectorCheckedBlock: {
              return PhysicalBlockSize;
            }
            case PageCheckedBlock: {
              return PhysicalBlockSize;
            }
            case CheckedSector: {
              return PhysicalSectorSize;
            }
            case CheckedPage: {
              return PhysicalPageSize;
            }
            case CheckedBlock: {
              return PhysicalBlockSize;
            }
            case FullSector: {
              return PhysicalSectorSize;
            }
            case FullPage: {
              return PhysicalPageSize;
            }
            case FullBlock: {
              return PhysicalBlockSize;
            }
          }
          return 0UL;
        }

        /* TODO */
        template <size_t BufKind>
        class TDataChunkSizeGetter {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return -1; }
          static constexpr size_t GetPhysicalDataChunkSize() { return -1; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<SectorCheckedBlock> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return LogicalSectorSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalSectorSize; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<PageCheckedBlock> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return LogicalPageSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalPageSize; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<CheckedSector> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return LogicalSectorSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalSectorSize; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<CheckedPage> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return LogicalPageSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalPageSize; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<CheckedBlock> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return LogicalCheckedBlockSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalBlockSize; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<FullSector> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return PhysicalSectorSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalSectorSize; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<FullPage> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return PhysicalPageSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalPageSize; }
        };  // TDataChunkSizeGetter

        template <>
        class TDataChunkSizeGetter<FullBlock> {
          NO_CONSTRUCTION(TDataChunkSizeGetter);
          public:
          static constexpr size_t GetLogicalDataChunkSize() { return PhysicalBlockSize; }
          static constexpr size_t GetPhysicalDataChunkSize() { return PhysicalBlockSize; }
        };  // TDataChunkSizeGetter

        /* TODO */
        template <size_t BufKind>
        constexpr size_t GetLogicalDataChunkSize() {
          return TDataChunkSizeGetter<BufKind>::GetLogicalDataChunkSize();
        }

        /* TODO */
        template <size_t BufKind>
        constexpr size_t GetPhysicalDataChunkSize() {
          return TDataChunkSizeGetter<BufKind>::GetPhysicalDataChunkSize();
        }

        /* TODO */
        typedef std::function<void (Stig::Indy::Disk::TDiskResult, const char *err_str)> TIOCallback;

        /* TODO */
        typedef std::function<void (TCacheInstr cache_instr, const TOffset logical_start_offset, void *buf, size_t count)> TCacheCb;

        /* Forward Declarations */
        class TPersistentDevice;
        class TGroupRequest;

        /* TODO */
        class TDiskController {
          NO_COPY_SEMANTICS(TDiskController);
          public:

          /* TODO */
          typedef InvCon::UnorderedList::TCollection<TDiskController, TPersistentDevice> TDeviceCollection;

          /* TODO */
          class TEvent
              : public Base::TThreadLocalPoolManager<TEvent>::TObjBase {
            NO_COPY_SEMANTICS(TEvent);
            public:

            /* TODO */
            typedef InvCon::UnorderedList::TMembership<TEvent, TPersistentDevice> TDeviceMembership;

            /* TODO */
            enum TKind {
              TriggeredRead,
              TriggeredReadV,
              TriggeredWrite,
              CallbackRead,
              CallbackReadV,
              CallbackWrite
            };

            /* TODO */
            TEvent() : Device(nullptr), NextEvent(nullptr), DeviceMembership(this), EventPool(nullptr) {}

            /* TODO */
            ~TEvent();

            /* TriggeredOp */
            void Init(TPersistentDevice *device,
                      Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                      const Base::TCodeLocation &code_location /* DEBUG */,
                      TKind kind,
                      DiskPriority priority,
                      const TOffset logical_start_offset,
                      TCompletionTrigger &trigger,
                      TBufKind buf_kind,
                      void *buf,
                      const TOffset offset,
                      long long nbytes,
                      bool abort_on_error);

            /* TriggeredVOp */
            void Init(TPersistentDevice *device,
                      Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                      const Base::TCodeLocation &code_location /* DEBUG */,
                      TKind kind,
                      DiskPriority priority,
                      const TOffset logical_start_offset,
                      TCompletionTrigger &trigger,
                      TBufKind buf_kind,
                      const std::vector<void *> &buf_vec,
                      const TOffset offset,
                      long long nbytes,
                      bool abort_on_error);

            /* CallbackOp */
            void Init(TPersistentDevice *device,
                      Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                      const Base::TCodeLocation &code_location /* DEBUG */,
                      TKind kind,
                      DiskPriority priority,
                      const TOffset logical_start_offset,
                      const TIOCallback &io_cb,
                      TBufKind buf_kind,
                      void *buf,
                      const TOffset offset,
                      long long nbytes,
                      bool abort_on_error);

            /* CallbackVOp */
            void Init(TPersistentDevice *device,
                      Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                      const Base::TCodeLocation &code_location /* DEBUG */,
                      TKind kind,
                      DiskPriority priority,
                      const TOffset logical_start_offset,
                      TGroupRequest *group_request,
                      TBufKind buf_kind,
                      const std::vector<void *> &buf_vec,
                      const TOffset offset,
                      long long nbytes,
                      bool abort_on_error);

            /* TODO */
            void Reset(bool back_to_pool);

            #ifndef NDEBUG
            void AssertCanFree() {
              /* do nothing */
            }
            #endif

            /* TODO */
            static Base::TThreadLocalPoolManager<Indy::Disk::Util::TDiskController::TEvent> DiskEventPoolManager;
            static __thread Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *LocalEventPool;

            private:

            /* TODO */
            inline void CommonInit(DiskPriority priority) {
              switch (priority) {
                case RealTime: {
                  Iocb.aio_reqprio = RealTimePriority;
                  break;
                }
                case Medium: {
                  Iocb.aio_reqprio = MediumPriority;
                  break;
                }
                case Low: {
                  Iocb.aio_reqprio = LowPriority;
                  break;
                }
              }
              Iocb.data = this;
            }

            /* TODO */
            TPersistentDevice *Device;

            /* TODO */
            TEvent *NextEvent;
            TDeviceMembership::TImpl DeviceMembership;

            /* TODO */
            Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *EventPool;

            /* TODO */
            TKind Kind;

            /* TODO */
            struct iocb Iocb;

            /* TODO */
            TBufKind BufKind;

            /* TODO */
            TOffset LogicalStartOffset;

            /* TODO */
            bool AbortOnError;

            #ifndef NDEBUG
            /* TODO */
            size_t RequestId;
            #endif

            Base::TCodeLocation CodeLocation; /* DEBUG */

            /* A union storing data dependent on what kind of event we are */
            union {
              TCompletionTrigger *TriggerOp;
              TIOCallback CallbackOp;
              struct {
                TCompletionTrigger *Trigger;
                struct iovec IoVec[MaxSegmentsPerIO];
                size_t IoVCnt;
              } TriggerVOp;
              struct {
                TGroupRequest *GroupRequest;
                struct iovec IoVec[MaxSegmentsPerIO];
                size_t IoVCnt;
              } CallbackVOp;
            };

            /* TODO */
            friend class TDiskController;
            friend class TPersistentDevice;

          };  // TEvent

          /* TODO */
          TDiskController();

          /* TODO */
          ~TDiskController();

          /* TODO */
          inline TDeviceCollection *GetDeviceCollection() const {
            assert(this);
            return &DeviceCollection;
          }

          /* TODO */
          void QueueRunner(std::vector<TPersistentDevice *> device_vec, bool no_realtime, size_t core);

          /* TODO */
          void Report(std::stringstream &ss, double elapsed_time) const;

          private:

          /* TODO */
          static constexpr int RealTimePriority = -2;
          static constexpr int MediumPriority = 2;
          static constexpr int LowPriority = 4;

          /* TODO */
          mutable TDeviceCollection::TImpl DeviceCollection;

          #ifndef NDEBUG
          /* TODO */
          std::unordered_set<size_t> OutstandingIdSet;
          size_t NextId;
          std::mutex OutstandingIdMutex;
          #endif

        };  // TDiskController

        /* TODO */
        class TDevice {
          NO_COPY_SEMANTICS(TDevice);
          public:

          /* TODO */
          struct TDesc {
            const enum TKind {SSD, HDD, FlashSAN, HDDSAN, Mem} Kind;
            const size_t LogicalBlockSize;
            const size_t PhysicalBlockSize;
            const size_t NumLogicalBlock;
            const size_t Capacity;
            bool operator !=(const TDesc &that) const {
              assert(this);
              assert(&that);
              return Kind != that.Kind
                || LogicalBlockSize != that.LogicalBlockSize
                || PhysicalBlockSize != that.PhysicalBlockSize
                || NumLogicalBlock != that.NumLogicalBlock
                || Capacity != that.Capacity;
            }
          };

          /* TODO */
          typedef InvCon::OrderedList::TMembership<TDevice, TVolume, size_t> TVolumeMembership;

          /* TODO */
          virtual ~TDevice() {}

          /* TODO */
          inline const TDesc &GetDesc() const {
            assert(this);
            return Desc;
          }

          /* TODO */
          TVolumeMembership *GetVolumeMembership() {
            assert(this);
            return &VolumeMembership;
          }

          /* TODO */
          virtual void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, const TOffset logical_start_offset,
                             TCompletionTrigger &trigger) = 0;

          /* TODO */
          virtual void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, const TOffset logical_start_offset,
                             const TIOCallback &cb) = 0;

          /* TODO */
          virtual void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger) = 0;

          /* TODO */
          virtual void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, const TIOCallback &cb) = 0;

          /* TODO */
          virtual void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                             const std::vector<void *> &buf_vec, const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger) = 0;

          /* TODO */
          virtual void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                             const std::vector<void *> &buf_vec, const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TGroupRequest *group_request) = 0;

          /* TODO */
          virtual void AsyncSyncFlush(std::mutex &mut, std::condition_variable &cond, size_t &num_finished, size_t &num_err) = 0;

          /* TODO */
          virtual void Sync() = 0;

          /* TODO */
          virtual size_t GetMaxSegments() const = 0;

          /* TODO */
          virtual size_t GetMaxSectorsKb() const = 0;

          /* TODO */
          virtual void DiscardAll() = 0;

          /* TODO */
          virtual void DiscardRange(uint64_t from, uint64_t num_bytes) = 0;

          protected:

          /* TODO */
          inline TDevice(TDesc desc, bool fsync_on, bool do_corruption_check)
              : VolumeMembership(this, 0UL),
                Desc(desc),
                FsyncOn(fsync_on),
                DoCorruptionCheck(do_corruption_check) {}

          /* TODO */
          void SetPos(size_t pos) {
            assert(this);
            VolumeMembership.SetKey(pos);
          }

          /* TODO */
          void ApplyCorruptionCheck(TBufKind buf_kind, void *buf, const TOffset offset, long long nbytes) const;

          /* TODO */
          bool CheckCorruptCheck(TBufKind buf_kind, void *buf, const TOffset offset, long long nbytes) const;

          /* TODO */
          TVolumeMembership::TImpl VolumeMembership;

          /* TODO */
          TDesc Desc;

          /* Controls whether this device actually fsyncs when requested. */
          bool FsyncOn;

          /* TODO */
          const bool DoCorruptionCheck;

          /* TODo */
          friend class TVolume;

        };  // TDevice

        /* TODO */
        typedef std::unordered_set<TDevice *> TDeviceSet;

        /* TODO */
        class TMemoryDevice
            : public TDevice {
          NO_COPY_SEMANTICS(TMemoryDevice);
          public:

          /* TODO */
          TMemoryDevice(size_t logical_block_size, size_t physical_block_size, size_t num_logical_block, bool fsync_on, bool do_corruption_check)
              : TDevice(TDesc{TDesc::Mem, logical_block_size, physical_block_size, num_logical_block, logical_block_size * num_logical_block}, fsync_on, do_corruption_check), Data(nullptr) {
            assert(Desc.Capacity % getpagesize() == 0);
            Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&Data), getpagesize(), PhysicalBlockSize /* super block */ + Desc.Capacity));
            Base::IfLt0(mlock(Data, PhysicalBlockSize /* super block */ + Desc.Capacity));
            try {
              memset(Data, 0, PhysicalBlockSize /* super block */ + Desc.Capacity);
            } catch (...) {
              free(Data);
            }
          }

          /* TODO */
          virtual ~TMemoryDevice() {
            free(Data);
          }

          /* TODO */
          virtual void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, const TOffset logical_start_offset,
                             TCompletionTrigger &trigger) override;

          /* TODO */
          virtual void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, const TOffset logical_start_offset,
                             const TIOCallback &cb) override;

          /* TODO */
          virtual void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger) override;

          /* TODO */
          virtual void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error, const TIOCallback &cb) override;

          /* TODO */
          virtual void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                             const std::vector<void *> &buf_vec, const TOffset offset, long long nbytes, DiskPriority priority,
                             bool abort_on_error, TCompletionTrigger &trigger) override;

          /* TODO */
          virtual void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                             const std::vector<void *> &buf_vec, const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TGroupRequest *group_request) override;

          /* TODO */
          virtual void AsyncSyncFlush(std::mutex &mut, std::condition_variable &cond, size_t &num_finished, size_t &num_err) override;

          /* TODO */
          virtual void Sync() override;

          /* TODO */
          virtual size_t GetMaxSegments() const override {
            return MaxSegmentsPerIO;
          }

          /* TODO */
          virtual size_t GetMaxSectorsKb() const override {
            return 64UL;
          }

          /* TODO */
          virtual void DiscardAll() override {
            #ifndef NDEBUG
            memset(Data + PhysicalBlockSize /* super block */, 0, Desc.Capacity);
            #endif
            /* do nothing */
          }

          /* TODO */
          virtual void DiscardRange(uint64_t from, uint64_t num_bytes) override {
            #ifndef NDEBUG
            memset(Data + from, 0, num_bytes);
            #endif
          }

          private:

          /* TODO */
          void WriteImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset offset,
                         long long nbytes, DiskPriority priority, bool abort_on_error);

          /* TODO */
          bool ReadImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset offset,
                        long long nbytes, DiskPriority priority, bool abort_on_error);

          /* TODO */
          char *Data;

        };  // TMemoryDevice

        /* TODO */
        class TPersistentDevice
            : public TDevice {
          NO_COPY_SEMANTICS(TPersistentDevice);
          public:

          /* TODO */
          typedef InvCon::UnorderedList::TMembership<TPersistentDevice, TDiskController> TControllerMembership;
          typedef InvCon::UnorderedList::TCollection<TPersistentDevice, TDiskController::TEvent> TEventQueue;

          /* TODO */
          TPersistentDevice(TDiskController *controller, const char *device_path, const char *device_name, size_t logical_block_size, size_t physical_block_size, size_t num_logical_block, bool fsync_on, bool do_corruption_check)
              : TDevice(TDesc{TDesc::SSD, logical_block_size, physical_block_size, num_logical_block, logical_block_size * num_logical_block}, fsync_on, do_corruption_check),
                ControllerMembership(this, controller->GetDeviceCollection()),
                IncomingEventQueue(nullptr),
                RealTimePrioEventQueue(this),
                MediumPrioEventQueue(this),
                LowPrioEventQueue(this),
                DevicePath(device_path),
                DiscardSupport(false),
                DiscardMaxBytes(0UL),
                DiscardGranularity(0UL),
                Inflight(0UL) {
            assert(Desc.Capacity % getpagesize() == 0);
            try {
              DiskFd = open(device_path, O_RDWR | O_DIRECT);
              Base::IfLt0(DiskFd);
            } catch (const std::exception &ex) {
              syslog(LOG_ERR, "Error while opening block device [%s] : %s", device_path, ex.what());
              throw;
            }
            const std::string path_to_device_info = TDeviceUtil::GetPathToDeviceInfo(std::string(device_name));
            std::string rq_affinity_path = path_to_device_info + std::string("queue/rq_affinity");
            const size_t rq_affinity = TDeviceUtil::GetRqAffinity(device_name);
            if (rq_affinity != 2) {
              syslog(LOG_ALERT, "Performance degradation: please set rq_affinity [%s] for device [%s] to 2", rq_affinity_path.c_str(), device_path);
            }
            MaxSegments = TDeviceUtil::GetMaxSegments(device_name);
            MaxSectorsKb = TDeviceUtil::GetMaxSectorsKb(device_name);
            DiscardMaxBytes = TDeviceUtil::GetDiscardMaxBytes(device_name);
            DiscardGranularity = TDeviceUtil::GetDiscardGranularity(device_name);
            DiscardSupport = DiscardMaxBytes > 0UL && DiscardGranularity > 0UL;
            MaxSegments = std::min(8UL, MaxSegments);
            MaxSectorsKb = std::min(64UL, MaxSectorsKb);
          }

          /* TODO */
          virtual ~TPersistentDevice() {}

          /* TODO */
          inline const char *GetDevicePath() const {
            assert(this);
            return DevicePath.c_str();
          }

          /* TODO */
          const Base::TFd &GetDiskFd() const {
            assert(this);
            return DiskFd;
          }

          /* TODO */
          inline virtual size_t GetMaxSegments() const override {
            assert(this);
            return MaxSegmentsPerIO;
          }

          /* TODO */
          inline virtual size_t GetMaxSectorsKb() const override {
            assert(this);
            return MaxSectorsKb;
          }

          /* Discard all the blocks except for the super block */
          virtual void DiscardAll() override {
            DiscardRange(PhysicalBlockSize /* skip super block */, Desc.Capacity - PhysicalBlockSize);
          }

          /* TODO */
          virtual void DiscardRange(uint64_t from, uint64_t num_bytes) override {
            assert(this);
            if (DiscardSupport) {
              uint64_t range[2];
              range[0] = from;
              range[1] = num_bytes;
              try {
                Base::IfLt0(ioctl(DiskFd, BLKDISCARD, &range));
              } catch (const std::exception &ex) {
                syslog(LOG_ERR, "Error calling BLKDISCARD from [%ld] for [%ld] bytes on device [%s]", range[0], range[1], DevicePath.c_str());
                throw;
              }
            }
          }

          /* TODO */
          virtual void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/, void */*buf*/,
                             const TOffset /*offset*/, long long /*nbytes*/, DiskPriority /*priority*/, bool abort_on_error,
                             const TOffset /*logical_start_offset*/, TCompletionTrigger &/*trigger*/) override;

          /* TODO */
          virtual void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/, void */*buf*/,
                             const TOffset /*offset*/, long long /*nbytes*/, DiskPriority /*priority*/, bool abort_on_error,
                             const TOffset /*logical_start_offset*/, const TIOCallback &/*cb*/) override;

          /* TODO */
          virtual void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/, void */*buf*/,
                            const TOffset /*offset*/, long long /*nbytes*/, DiskPriority /*priority*/, bool abort_on_error,
                            TCompletionTrigger &/*trigger*/) override;

          /* TODO */
          virtual void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/, void */*buf*/,
                            const TOffset /*offset*/, long long /*nbytes*/, DiskPriority /*priority*/, bool abort_on_error,
                            const TIOCallback &/*cb*/) override;

          /* TODO */
          virtual void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                             const std::vector<void *> &buf_vec, const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger) override;

          /* TODO */
          virtual void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                             const std::vector<void *> &buf_vec, const TOffset offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TGroupRequest *group_request) override;

          /* TODO */
          virtual void AsyncSyncFlush(std::mutex &/*mut*/, std::condition_variable &/*cond*/, size_t &/*num_finished*/, size_t &/*num_err*/) override;

          /* TODO */
          virtual void Sync() override;

          private:

          /* Append Event to synchronized incoming queue. */
          inline void Enqueue(TDiskController::TEvent *inbound_event) {
            assert(this);
            assert(inbound_event);
            assert(inbound_event->NextEvent == nullptr);
            do {
              assert(inbound_event->NextEvent != reinterpret_cast<void *>(0xbfffe0000));
              assert(IncomingEventQueue != reinterpret_cast<void *>(0xbfffe0000));
              inbound_event->NextEvent = IncomingEventQueue;
            } while (!__sync_bool_compare_and_swap(&IncomingEventQueue, inbound_event->NextEvent, inbound_event));
            assert(inbound_event->NextEvent != reinterpret_cast<void *>(0xbfffe0000));
          }

          /* TODO */
          TControllerMembership::TImpl ControllerMembership;

          /* TODO */
          TDiskController::TEvent *IncomingEventQueue;
          Base::TEventSemaphore EventQueueSem;
          std::mutex EventQueueMutex;

          /* TODO */
          TEventQueue::TImpl RealTimePrioEventQueue;
          TEventQueue::TImpl MediumPrioEventQueue;
          TEventQueue::TImpl LowPrioEventQueue;

          /* TODO */
          std::string DevicePath;

          /* TODO */
          Base::TFd DiskFd;

          /* TODO */
          size_t MaxSegments;

          /* TODO */
          size_t MaxSectorsKb;

          /* TODO */
          bool DiscardSupport;
          size_t DiscardMaxBytes;
          size_t DiscardGranularity;

          /* TODO */
          std::atomic<size_t> Inflight;

          /* TODO */
          friend class TDiskController;

        };  // TPersistentDevice

        /* TODO */
        class TVolume {
          NO_COPY_SEMANTICS(TVolume);
          public:

          struct TDesc {
            const enum TKind {Striped, Chained} Kind;
            const TDevice::TDesc DeviceDesc;
            const enum TStorageSpeed {Fast, Slow} StorageSpeed;
            const size_t ReplicationFactor;
            const size_t NumLogicalExtent;
            const size_t NumLogicalBlockPerStripe;
            const size_t MinDiscardAllocBlocks;
            const double HighUtilizationThreshold;
          };

          /* TODO */
          typedef InvCon::OrderedList::TCollection<TVolume, TDevice, size_t> TDeviceCollection;
          typedef InvCon::UnorderedList::TMembership<TVolume, TVolumeManager> TManagerMembership;

          /* TODO */
          TVolume(TDesc desc, const TCacheCb &cache_cb, Base::TScheduler *scheduler);

          /* TODO */
          virtual ~TVolume();

          /* TODO */
          void AddDevice(TDevice *device, size_t pos) {
            assert(this);
            assert(device);
            device->SetPos(pos);
            if (ManagerMembership.TryGetCollection()) {
              syslog(LOG_ERR, "Adding device to Mounted Volume is currently not supported.");
              throw std::runtime_error("Adding device to Mounted Volume is currently not supported.");
            }
            if (device->GetDesc() != Desc.DeviceDesc) {
              syslog(LOG_ERR, "Cannot add heterogenous device kinds to same volume.");
              throw std::runtime_error("Device kind mismatch error.");
            }
            DeviceCollection.Insert(device->GetVolumeMembership());
          }

          /* TODO */
          inline size_t GetVolumeId() const {
            assert(this);
            return VolumeId;
          }

          /* TODO */
          inline const TDesc &GetDesc() const {
            assert(this);
            return Desc;
          }

          /* TODO */
          TDeviceCollection *GetDeviceCollection() const {
            assert(this);
            return &DeviceCollection;
          }

          /* TODO */
          size_t GetNumDevices() const {
            assert(this);
            size_t count = 0UL;
            for (TDeviceCollection::TCursor csr(&DeviceCollection); csr; ++csr, ++count) {}
            return count;
          }

          /* TODO */
          const std::vector<TLogicalExtent> &GetLogicalExtentVec() const;

          /* TODO */
          TManagerMembership *GetManagerMembership() {
            assert(this);
            return &ManagerMembership;
          }

          /* TODO */
          template <typename... TArgs>
          void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset start_offset,
                     long long nbytes, DiskPriority priority, bool abort_on_error, TCacheInstr cache_instr, TArgs &...args);

          /* TODO */
          template <typename... TArgs>
          void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset start_offset,
                    long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

          /* TODO */
          template <typename... TArgs>
          void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array, size_t num_buf,
                     const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

          /* TODO */
          void TryAllocateSequentialBlocks(size_t num_blocks, const std::function<void (const TBlockRange &block_range)> &cb);

          /* TODO */
          void FreeSequentialBlocks(const TBlockRange &block_range);

          /* TODO */
          void MarkBlockRangeUsed(const TBlockRange &block_range);

          /* TODO */
          void AppendTouchedDevicesToSet(TDeviceSet &device_set, const TBlockRange &block_range) const;

          /* TODO */
          std::pair<size_t, size_t> AppendUsage(std::stringstream &ss) const;

          /* TODO */
          void DiscardAll();

          private:

          /* TODO */
          inline void DoCache(TCacheInstr cache_instr, const TOffset start_offset, void *buf, long long nbytes);

          /* Forward declaration of PIMPL */
          class TStrategy;
          class TStripedStrategy;
          class TChainedStrategy;

          /* TODO */
          bool Init(const TExtentSet &extent_set);

          /* TODO */
          inline void SetVolumeId(size_t vol_id) {
            assert(this);
            VolumeId = vol_id;
          }

          /* TODO */
          mutable TDeviceCollection::TImpl DeviceCollection;

          /* TODO */
          TManagerMembership::TImpl ManagerMembership;

          /* TODO */
          size_t VolumeId;

          /* TODO */
          const TDesc Desc;

          /* TODO */
          TStrategy *Strategy;

          /* TODO */
          Base::TScheduler *Scheduler;

          /* TODO */
          const TCacheCb CacheCb;

          /* TODO */
          friend class TDiskUtil;
          friend class TVolumeManager;

        };  // TVolume

        /* TODO */
        class TVolumeManager {
          NO_COPY_SEMANTICS(TVolumeManager);
          public:

          /* TODO */
          typedef InvCon::UnorderedList::TCollection<TVolumeManager, TVolume> TVolumeCollection;

          /* TODO */
          TVolumeManager(Base::TScheduler *scheduler);

          /* TODO */
          ~TVolumeManager();

          /* TODO */
          bool AddNewVolume(TVolume *volume);

          /* TODO */
          void AddExistingVolume(TVolume *volume, size_t volume_id);

          /* TODO */
          inline void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset start_offset, long long nbytes, DiskPriority priority, TCacheInstr cache_instr, TCompletionTrigger &trigger,
                            bool abort_on_error = true);

          /* TODO */
          inline void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset start_offset, long long nbytes, DiskPriority priority, TCacheInstr cache_instr, TCompletionTrigger &trigger,
                            const TIOCallback &cb, bool abort_on_error = true);

          /* TODO */
          inline void WriteAndFlush(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                    const TOffset start_offset, long long nbytes, DiskPriority priority, TCacheInstr cache_instr,
                                    TCompletionTrigger &trigger, bool abort_on_error = true);

          /* TODO */
          inline void WriteBlock(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                 size_t block_id, DiskPriority priority, TCacheInstr cache_instr, TCompletionTrigger &trigger,
                                 bool abort_on_error = true);

          /* TODO */
          inline void WriteBlock(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                 size_t block_id, DiskPriority priority, TCacheInstr cache_instr, TCompletionTrigger &trigger, const TIOCallback &cb,
                                 bool abort_on_error = true);

          /* TODO */
          inline void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                           const TOffset start_offset, long long nbytes, DiskPriority priority, TCompletionTrigger &trigger,
                           bool abort_on_error = true);

          /* TODO */
          inline void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                           const TOffset start_offset, long long nbytes, DiskPriority priority, TCompletionTrigger &trigger, const TIOCallback &cb,
                           bool abort_on_error = true);

          /* TODO */
          inline void ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                            size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, TCompletionTrigger &trigger,
                            const TIOCallback &cb, bool abort_on_error = true);

          /* TODO */
          inline void ReadBlock(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, size_t block_id,
                                DiskPriority priority, TCompletionTrigger &trigger, bool abort_on_error = true);

          /* TODO */
          void TryAllocateSequentialBlocks(TVolume::TDesc::TStorageSpeed storage_speed, size_t num_blocks, const std::function<void (const TBlockRange &block_range)> &cb);

          /* TODO */
          void MarkBlockRangeUsed(const TBlockRange &block_range);

          /* TODO */
          void FreeSequentialBlocks(const TBlockRange &block_range);

          /* TODO */
          void SyncToDisk(const std::vector<TBlockRange> &block_range_vec);

          /* TODO */
          void AppendVolumeUsageReport(std::stringstream &ss) const;

          /* TODO */
          void DiscardAllDevices();

          private:

          /* TODO */
          size_t RequestNewVolumeId() const {
            assert(this);
            size_t max_id = 0UL;
            for (TVolumeCollection::TCursor csr(&VolumeCollection); csr; ++csr) {
              max_id = std::max(max_id, csr->GetVolumeId());
            }
            return max_id + 1;
          }

          /* TODO */
          template <typename ...TArgs>
          void WriteImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                         const TOffset start_offset, long long nbytes, DiskPriority priority, TCacheInstr cache_instr, bool abort_on_error,
                         TArgs &...args);

          /* TODO */
          template <typename ...TArgs>
          void ReadImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                        const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

          /* TODO */
          template <typename ...TArgs>
          void ReadVImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array, size_t num_buf,
                         const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

          /* TODO */
          void AllocateLogicalExtents(TExtentSet &logical_extent_set, size_t num_extent, size_t extent_size, TVolume *volume);

          /* TODO */
          mutable TVolumeCollection::TImpl VolumeCollection;

          /* TODO */
          Base::TScheduler *Scheduler;

          /* TODO */
          std::vector<bool> AllocatedExtentBlocks;

          /* TODO */
          std::unordered_map<size_t, TVolume *> LogicalExtentStartToVolumeMap;

          /* TODO */
          static constexpr size_t ExtentAllocationBlockSize = 16UL * 1024UL * 1024UL * 1024UL * 1024UL; /* 16 TB */

          /* TODO */
          friend class TDiskUtil;

        };  // TVolumeManager

        template <>
        void TVolume::Write<TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                                const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                                TCacheInstr cache_instr, TCompletionTrigger &trigger);

        template <>
        void TVolume::Write<TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                   uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes,
                                                                   DiskPriority priority, bool abort_on_error, TCacheInstr cache_instr,
                                                                   TCompletionTrigger &trigger, const TIOCallback &cb);

        template <>
        void TVolume::Read<TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                               const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                               TCompletionTrigger &trigger);

        template <>
        void TVolume::Read<TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                  uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes,
                                                                  DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger,
                                                                  const TIOCallback &cb);

        template <>
        void TVolume::ReadV<TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                                                void **buf_array, size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority,
                                                bool abort_on_error, TCompletionTrigger &trigger);

        template <>
        void TVolume::ReadV<TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                   uint8_t util_src, void **buf_array, size_t num_buf, const TOffset start_offset,
                                                                   long long nbytes, DiskPriority priority, bool abort_on_error,
                                                                   TCompletionTrigger &trigger, const TIOCallback &cb);

        template <typename ...TArgs>
        void TVolumeManager::WriteImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                       const TOffset start_offset, long long nbytes, DiskPriority priority, TCacheInstr cache_instr,
                                       bool abort_on_error, TArgs &...args) {
          assert(this);
          const size_t logical_extent_block_start = (start_offset / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
          const size_t logical_extent_block_end = ((start_offset + nbytes - 1) / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
          TVolume *const start_vol = LogicalExtentStartToVolumeMap[logical_extent_block_start];
          TVolume *const end_vol = LogicalExtentStartToVolumeMap[logical_extent_block_end];
          if (likely(start_vol == end_vol)) {
            assert(start_vol);
            start_vol->Write(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, cache_instr, args...);
          } else {
            throw std::logic_error("TODO: implement support for cross-volume writes");
          }
        }

        template <typename ...TArgs>
        void TVolumeManager::ReadImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                      const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args) {
          assert(this);
          const size_t logical_extent_block_start = (start_offset / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
          const size_t logical_extent_block_end = ((start_offset + nbytes - 1) / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
          TVolume *const start_vol = LogicalExtentStartToVolumeMap[logical_extent_block_start];
          TVolume *const end_vol = LogicalExtentStartToVolumeMap[logical_extent_block_end];
          if (likely(start_vol == end_vol)) {
            assert(start_vol);
            start_vol->Read(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, args...);
          } else {
            throw std::logic_error("TODO: implement support for cross-volume reads");
          }
        }

        template <typename ...TArgs>
        void TVolumeManager::ReadVImpl(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                                       size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                       TArgs &...args) {
          assert(this);
          const size_t logical_extent_block_start = (start_offset / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
          const size_t logical_extent_block_end = ((start_offset + nbytes - 1) / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
          TVolume *const start_vol = LogicalExtentStartToVolumeMap[logical_extent_block_start];
          TVolume *const end_vol = LogicalExtentStartToVolumeMap[logical_extent_block_end];
          if (likely(start_vol == end_vol)) {
            assert(start_vol);
            start_vol->ReadV(code_location, buf_kind, util_src, buf_array, num_buf, start_offset, nbytes, priority, abort_on_error, args...);
          } else {
            throw std::logic_error("TODO: implement support for cross-volume reads");
          }
        }

        /*** Inline ***/

        inline void TVolume::DoCache(TCacheInstr cache_instr, const TOffset start_offset, void *buf, long long nbytes) {
          CacheCb(cache_instr, start_offset, buf, nbytes);
        }

        inline void TVolumeManager::Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                          const TOffset start_offset, long long nbytes, DiskPriority priority, TCacheInstr cache_instr,
                                          TCompletionTrigger &trigger, bool abort_on_error) {
          WriteImpl(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, cache_instr, abort_on_error, trigger);
        }

        inline void TVolumeManager::Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                          const TOffset start_offset, long long nbytes, DiskPriority priority, TCacheInstr cache_instr,
                                          TCompletionTrigger &trigger, const TIOCallback &cb, bool abort_on_error) {
          WriteImpl(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, cache_instr, abort_on_error, trigger, cb);
        }

        inline void TVolumeManager::WriteAndFlush(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                                                  void *buf, const TOffset start_offset, long long nbytes, DiskPriority priority,
                                                  TCacheInstr cache_instr, TCompletionTrigger &trigger, bool abort_on_error) {
          WriteImpl(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, cache_instr, abort_on_error, trigger);
          std::vector<TBlockRange> block_range_vec;
          const size_t num_consec_blocks = ((start_offset + nbytes - 1) / PhysicalBlockSize) - (start_offset / PhysicalBlockSize) + 1;
          trigger.Wait();
          SyncToDisk(std::vector<TBlockRange>{TBlockRange{start_offset / PhysicalBlockSize, num_consec_blocks}});
        }

        inline void TVolumeManager::Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                         const TOffset start_offset, long long nbytes, DiskPriority priority, TCompletionTrigger &trigger,
                                         bool abort_on_error) {
          ReadImpl(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger);
        }

        inline void TVolumeManager::Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                         const TOffset start_offset, long long nbytes, DiskPriority priority, TCompletionTrigger &trigger,
                                         const TIOCallback &cb, bool abort_on_error) {
          ReadImpl(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
        }

        inline void TVolumeManager::ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                                          size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority,
                                          TCompletionTrigger &trigger, const TIOCallback &cb, bool abort_on_error) {
          ReadVImpl(code_location, buf_kind, util_src, buf_array, num_buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
        }

        inline void TVolumeManager::WriteBlock(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                               size_t block_id, DiskPriority priority, TCacheInstr cache_instr, TCompletionTrigger &trigger,
                                               bool abort_on_error) {
          Write(code_location, buf_kind, util_src, buf, block_id * PhysicalBlockSize, PhysicalBlockSize, priority, cache_instr, trigger,
                abort_on_error);
        }

        inline void TVolumeManager::WriteBlock(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                               size_t block_id, DiskPriority priority, TCacheInstr cache_instr, TCompletionTrigger &trigger,
                                               const TIOCallback &cb, bool abort_on_error) {
          Write(code_location, buf_kind, util_src, buf, block_id * PhysicalBlockSize, PhysicalBlockSize, priority, cache_instr, trigger, cb,
                abort_on_error);
        }

        inline void TVolumeManager::ReadBlock(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                              size_t block_id, DiskPriority priority, TCompletionTrigger &trigger, bool abort_on_error) {
          Read(code_location, buf_kind, util_src, buf, block_id * PhysicalBlockSize, PhysicalBlockSize, priority, trigger, abort_on_error);
        }

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig
