/* <orly/indy/disk/util/device_util.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <fcntl.h>
#include <string.h>
#include <syslog.h>

#include <functional>

#include <base/class_traits.h>
#include <base/dir_iter.h>
#include <base/fd.h>
#include <base/murmur.h>

namespace Orly {

  namespace Indy {

    namespace Disk {

      namespace Util {

        static constexpr size_t MaxInstanceNameSize = 24UL;

        /* TODO */
        struct TVolumeId {
          size_t Id;
          char InstanceName[MaxInstanceNameSize];
          inline bool operator==(const TVolumeId &that) const {
            return Id == that.Id && memcmp(InstanceName, that.InstanceName, MaxInstanceNameSize) == 0;
          }
          inline size_t GetHash() const {
            return Id;
          }
          inline std::string GetInstanceName() const {
            return std::string(InstanceName);
          }
        };

      }  // Util

    }  // Disk

  }  // Indy

}  // Orly

namespace std {

  /* A standard hasher for Orly::Indy::Disk::Util::TVolumeId. */
  template <>
  struct hash<Orly::Indy::Disk::Util::TVolumeId> {
    typedef size_t result_type;
    typedef Orly::Indy::Disk::Util::TVolumeId argument_type;
    size_t operator()(const argument_type &that) const {
      return that.GetHash();
    }
  };

}  // std

namespace Orly {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        class TDeviceUtil {
          NO_CONSTRUCTION(TDeviceUtil);
          public:

          static constexpr uint64_t OrlyFSMagicNumber = 15684509348693311000UL;
          static constexpr size_t BlockSize = 4096 * 16;
          static constexpr size_t CorruptionCheckSize = sizeof(uint64_t);
          static_assert(sizeof(TVolumeId) % sizeof(uint64_t) == 0, "TVolumeId size must be a multiple of 8 bytes");
          static constexpr size_t NumDataElem = (BlockSize - CorruptionCheckSize) / sizeof(uint64_t);
          static constexpr size_t MagicNumberPos = 0UL;
          static constexpr size_t VolumeIdPos = 1UL;
          static constexpr size_t VolumeDeviceNumberPos = VolumeIdPos + sizeof(TVolumeId) / sizeof(uint64_t);
          static constexpr size_t NumDevicesInVolumePos = VolumeDeviceNumberPos + 1UL;
          static constexpr size_t LogicalExtentStartPos = NumDevicesInVolumePos + 1UL;
          static constexpr size_t LogicalExtentSizePos = LogicalExtentStartPos + 1UL;
          static constexpr size_t VolumeStrategyPos = LogicalExtentSizePos + 1UL;
          static constexpr size_t VolumeSpeedPos = VolumeStrategyPos + 1UL;
          static constexpr size_t ReplicationFactorPos = VolumeSpeedPos + 1UL;
          static constexpr size_t StripeSizeKBPos = ReplicationFactorPos + 1UL;
          static constexpr size_t LogicalBlockSizePos = StripeSizeKBPos + 1UL;
          static constexpr size_t PhysicalBlockSizePos = LogicalBlockSizePos + 1UL;
          static constexpr size_t NumLogicalBlockExposedPos = PhysicalBlockSizePos + 1UL;
          static constexpr size_t MinDiscardBlocksPos = NumLogicalBlockExposedPos + 1UL;

          struct TOrlyDevice {
            TVolumeId VolumeId;
            uint64_t VolumeDeviceNumber;
            uint64_t NumDevicesInVolume;
            uint64_t LogicalExtentStart;
            uint64_t LogicalExtentSize;
            uint64_t VolumeStrategy;
            uint64_t VolumeSpeed;
            uint64_t ReplicationFactor;
            uint64_t StripeSizeKB;
            uint64_t LogicalBlockSize;
            uint64_t PhysicalBlockSize;
            uint64_t NumLogicalBlockExposed;
            uint64_t MinDiscardBlocks;
          };

          /* TODO */
          static bool ProbeDevice(const char *path, TOrlyDevice &out_device) {
            try {
              Base::TFd fd = open(path, O_RDONLY);
              size_t *buf = nullptr;
              Base::IfNe0(posix_memalign(reinterpret_cast<void **>(&buf), BlockSize, BlockSize));
              try {
                Base::IfLt0(pread(fd, buf, BlockSize, 0UL));
                if (buf[MagicNumberPos] != OrlyFSMagicNumber) {
                  free(buf);
                  return false;
                }
                uint64_t check = Base::Murmur(buf, NumDataElem, 0UL);
                if (check != buf[NumDataElem]) {
                  throw std::runtime_error("Orly system block corrupt");
                }
                memcpy(&(out_device.VolumeId), &buf[VolumeIdPos], sizeof(TVolumeId));
                out_device.VolumeDeviceNumber = buf[VolumeDeviceNumberPos];
                out_device.NumDevicesInVolume = buf[NumDevicesInVolumePos];
                out_device.LogicalExtentStart = buf[LogicalExtentStartPos];
                out_device.LogicalExtentSize = buf[LogicalExtentSizePos];
                out_device.VolumeStrategy = buf[VolumeStrategyPos];
                out_device.VolumeSpeed = buf[VolumeSpeedPos];
                out_device.ReplicationFactor = buf[ReplicationFactorPos];
                out_device.StripeSizeKB = buf[StripeSizeKBPos];
                out_device.LogicalBlockSize = buf[LogicalBlockSizePos];
                out_device.PhysicalBlockSize = buf[PhysicalBlockSizePos];
                out_device.NumLogicalBlockExposed = buf[NumLogicalBlockExposedPos];
                out_device.MinDiscardBlocks = buf[MinDiscardBlocksPos];
              } catch (...) {
                free(buf);
                throw;
              }
              free(buf);
            } catch (const std::exception &ex) {
              return false;
            }
            return true;
          }

          /* TODO */
          static void ModifyDevice(const char *path, TOrlyDevice &new_device_info) {
            Base::TFd fd = open(path, O_RDWR);
            size_t *buf = nullptr;
            Base::IfNe0(posix_memalign(reinterpret_cast<void **>(&buf), BlockSize, BlockSize));
            try {
              memset(buf, 0, BlockSize);
              buf[MagicNumberPos] = OrlyFSMagicNumber;
              memcpy(&buf[VolumeIdPos], &(new_device_info.VolumeId), sizeof(TVolumeId));
              buf[VolumeDeviceNumberPos] = new_device_info.VolumeDeviceNumber;
              buf[NumDevicesInVolumePos] = new_device_info.NumDevicesInVolume;
              buf[LogicalExtentStartPos] = new_device_info.LogicalExtentStart;
              buf[LogicalExtentSizePos] = new_device_info.LogicalExtentSize;
              buf[VolumeStrategyPos] = new_device_info.VolumeStrategy;
              buf[VolumeSpeedPos] = new_device_info.VolumeSpeed;
              buf[ReplicationFactorPos] = new_device_info.ReplicationFactor;
              buf[StripeSizeKBPos] = new_device_info.StripeSizeKB;
              buf[LogicalBlockSizePos] = new_device_info.LogicalBlockSize;
              buf[PhysicalBlockSizePos] = new_device_info.PhysicalBlockSize;
              buf[NumLogicalBlockExposedPos] = new_device_info.NumLogicalBlockExposed;
              buf[MinDiscardBlocksPos] = new_device_info.MinDiscardBlocks;
              buf[NumDataElem] = Base::Murmur(buf, NumDataElem, 0UL);
              Base::IfLt0(pwrite(fd, buf, BlockSize, 0UL));
              fsync(fd);
            } catch (...) {
              free(buf);
              throw;
            }
            free(buf);
          }

          /* TODO */
          static void ZeroSuperBlock(const char *path) {
            Base::TFd fd = open(path, O_RDWR);
            size_t *buf = nullptr;
            Base::IfNe0(posix_memalign(reinterpret_cast<void **>(&buf), BlockSize, BlockSize));
            try {
              memset(buf, 0, BlockSize);
              Base::IfLt0(pwrite(fd, buf, BlockSize, 0UL));
              fsync(fd);
            } catch (...) {
              free(buf);
              throw;
            }
            free(buf);
          }

          /* TODO */
          static bool ForEachDevice(const std::function<bool (const char *)> &cb);

          static bool IsHardDrive(const std::string &dev_path);

          static std::string GetPathToPartitionInfo(const std::string &dev_path);

          static std::string GetPathToDeviceInfo(const std::string &dev_path);

          static size_t GetValFromDeviceInfo(const std::string &dev_name, const std::string &path_to_field) {
            const std::string path_to_device_info = TDeviceUtil::GetPathToDeviceInfo(std::string(dev_name));
            std::string info_path = path_to_device_info + path_to_field;
            size_t val = 0UL;
            try {
              Base::TFd fd = open(info_path.c_str(), O_RDONLY);
              Base::IfLt0(fd);
              char buf[64];
              char *ptr = buf;
              Base::IfLt0(pread(fd, buf, 64, 0));
              val = strtol(ptr, &ptr, 10);
            } catch (const std::exception &ex) {
              syslog(LOG_ERR, "Error while opening [%s] file [%s] for device [%s] : %s", path_to_field.c_str(), info_path.c_str(), dev_name.c_str(), ex.what());
              throw;
            }
            return val;
          }

          static size_t GetDiscardGranularity(const std::string &dev_name) {
            return GetValFromDeviceInfo(dev_name, "queue/discard_granularity");
          }

          static size_t GetDiscardMaxBytes(const std::string &dev_name) {
            return GetValFromDeviceInfo(dev_name, "queue/discard_max_bytes");
          }

          static size_t GetRqAffinity(const std::string &dev_name) {
            return GetValFromDeviceInfo(dev_name, "queue/rq_affinity");
          }

          static size_t GetMaxSegments(const std::string &dev_name) {
            return GetValFromDeviceInfo(dev_name, "queue/max_segments");
          }

          static size_t GetMaxSectorsKb(const std::string &dev_name) {
            return GetValFromDeviceInfo(dev_name, "queue/max_sectors_kb");
          }

        };  // TDeviceUtil

      }  // Util

    }  // Disk

  }  // Indy

}  // Orly