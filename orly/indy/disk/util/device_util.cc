/* <orly/indy/disk/util/device_util.cc>

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

#include <orly/indy/disk/util/device_util.h>

#include <fstream>
#include <istream>
#include <stdexcept>

#include <base/thrower.h>
#include <base/mem_aligned_ptr.h>

using namespace Orly::Indy::Disk::Util;
using namespace std;

bool TDeviceUtil::ProbeDevice(const char *path, TOrlyDevice &out_device) {
  try {
    Base::TFd fd = open(path, O_RDONLY);
    auto buf = Base::MemAlignedAlloc<uint64_t>(BlockSize, BlockSize);
    try {
      Base::IfLt0(pread(fd, buf.get(), BlockSize, 0UL));
      if (buf.get()[MagicNumberPos] != OrlyFSMagicNumber) {
        return false;
      }
      uint64_t check = Base::Murmur(buf.get(), NumDataElem, 0UL);
      if (check != buf.get()[NumDataElem]) {
        throw std::runtime_error("Orly system block corrupt");
      }
      memcpy(&(out_device.VolumeId), &buf.get()[VolumeIdPos], sizeof(TVolumeId));
      out_device.VolumeDeviceNumber = buf.get()[VolumeDeviceNumberPos];
      out_device.NumDevicesInVolume = buf.get()[NumDevicesInVolumePos];
      out_device.LogicalExtentStart = buf.get()[LogicalExtentStartPos];
      out_device.LogicalExtentSize = buf.get()[LogicalExtentSizePos];
      out_device.VolumeStrategy = buf.get()[VolumeStrategyPos];
      out_device.VolumeSpeed = buf.get()[VolumeSpeedPos];
      out_device.ReplicationFactor = buf.get()[ReplicationFactorPos];
      out_device.StripeSizeKB = buf.get()[StripeSizeKBPos];
      out_device.LogicalBlockSize = buf.get()[LogicalBlockSizePos];
      out_device.PhysicalBlockSize = buf.get()[PhysicalBlockSizePos];
      out_device.NumLogicalBlockExposed = buf.get()[NumLogicalBlockExposedPos];
      out_device.MinDiscardBlocks = buf.get()[MinDiscardBlocksPos];
    } catch (...) {
      throw;
    }
  } catch (const std::exception &ex) {
    return false;
  }
  return true;
}

void TDeviceUtil::ModifyDevice(const char *path, TOrlyDevice &new_device_info) {
  Base::TFd fd = open(path, O_RDWR);
  auto buf = Base::MemAlignedAlloc<uint64_t>(BlockSize, BlockSize);
  try {
    memset(buf.get(), 0, BlockSize);
    buf.get()[MagicNumberPos] = OrlyFSMagicNumber;
    memcpy(&buf.get()[VolumeIdPos], &(new_device_info.VolumeId), sizeof(TVolumeId));
    buf.get()[VolumeDeviceNumberPos] = new_device_info.VolumeDeviceNumber;
    buf.get()[NumDevicesInVolumePos] = new_device_info.NumDevicesInVolume;
    buf.get()[LogicalExtentStartPos] = new_device_info.LogicalExtentStart;
    buf.get()[LogicalExtentSizePos] = new_device_info.LogicalExtentSize;
    buf.get()[VolumeStrategyPos] = new_device_info.VolumeStrategy;
    buf.get()[VolumeSpeedPos] = new_device_info.VolumeSpeed;
    buf.get()[ReplicationFactorPos] = new_device_info.ReplicationFactor;
    buf.get()[StripeSizeKBPos] = new_device_info.StripeSizeKB;
    buf.get()[LogicalBlockSizePos] = new_device_info.LogicalBlockSize;
    buf.get()[PhysicalBlockSizePos] = new_device_info.PhysicalBlockSize;
    buf.get()[NumLogicalBlockExposedPos] = new_device_info.NumLogicalBlockExposed;
    buf.get()[MinDiscardBlocksPos] = new_device_info.MinDiscardBlocks;
    buf.get()[NumDataElem] = Base::Murmur(buf.get(), NumDataElem, 0UL);
    Base::IfLt0(pwrite(fd, buf.get(), BlockSize, 0UL));
    fsync(fd);
  } catch (...) {
    throw;
  }
}
void TDeviceUtil::ZeroSuperBlock(const char *path) {
  Base::TFd fd = open(path, O_RDWR);
  auto buf = Base::MemAlignedAlloc<uint64_t>(BlockSize, BlockSize);
  try {
    memset(buf.get(), 0, BlockSize);
    Base::IfLt0(pwrite(fd, buf.get(), BlockSize, 0UL));
    fsync(fd);
  } catch (...) {
    throw;
  }
}

bool TDeviceUtil::ForEachDevice(const function<bool(const char *)> &cb) {
  ifstream partitions("/proc/partitions");
  if(!partitions.is_open()) {
    THROW << "Unable to open /proc/partitions";
  }

  std::string line;

  //Make sure the formatting is what we expect (first line)
  getline(partitions, line);
  if (line != "major minor  #blocks  name") {
    THROW << "Unkwnown format of /proc/partitions";
  }

  // Skip empty line
  getline(partitions, line);

  //Read each entry, extract the last field 'name', and send it to the callback
  while(getline(partitions, line)) {
    if(!cb(line.substr(line.rfind(' ') + 1).c_str())) {
      return false;
    }
  }

  return true;
}

bool TDeviceUtil::IsHardDrive(const std::string &dev_path) {
  return dev_path.compare(0, 2, "sd") == 0;
}

std::string TDeviceUtil::GetPathToPartitionInfo(const std::string &dev_path) {
  if(IsHardDrive(dev_path)) {
    return GetPathToDeviceInfo(dev_path) + dev_path + '/';
  }
  return GetPathToDeviceInfo(dev_path);
}

std::string TDeviceUtil::GetPathToDeviceInfo(const std::string &dev_path) {
  std::string path_to_partition_info = "/sys/block/";
  if (IsHardDrive(dev_path)) {
    auto digit_start = dev_path.find_last_not_of("0123456789");
    if (digit_start == std::string::npos) {
      path_to_partition_info += dev_path;
    } else {
      path_to_partition_info += dev_path.substr(0, digit_start + 1);
    }
  } else {
    path_to_partition_info += dev_path;
  }
  // NOTE: We add the '/'  on a seperate line from the return so that we can take advantage of NRVO
  path_to_partition_info += '/';

  return path_to_partition_info;
}

size_t TDeviceUtil::GetValFromDeviceInfo(const std::string &dev_name, const std::string &path_to_field) {
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
