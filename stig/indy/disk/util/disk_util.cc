/* <stig/indy/disk/util/disk_util.cc>

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

#include <iostream>
#include <sstream>

#include <syslog.h>

#include <stig/indy/disk/util/device_util.h>
#include <stig/indy/disk/util/disk_util.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Indy::Disk::Util;

TDiskUtil::TDiskUtil(Base::TScheduler *scheduler,
                     TDiskController *controller,
                     const Base::TOpt<std::string> &instance_filter,
                     bool do_fsync,
                     const TCacheCb &cache_cb,
                     bool do_corruption_check)
    : Scheduler(scheduler), Controller(controller), CacheCb(cache_cb) {
  TDeviceUtil::ForEachDevice([this, &instance_filter, do_corruption_check, do_fsync](const char *path) {
    TDeviceUtil::TStigDevice device_info;
    string path_to_device = "/dev/";
    path_to_device += path;
    bool ret = TDeviceUtil::ProbeDevice(path_to_device.c_str(), device_info);
    AllDeviceSet.emplace(path_to_device);
    if (ret && (!instance_filter || device_info.VolumeId.GetInstanceName() == *instance_filter)) {
      StigDeviceMap.emplace(path_to_device, device_info);
      auto ret = VolumeById.find(device_info.VolumeId);
      if (ret != VolumeById.end()) {
        auto &volume = ret->second;
        /* this is where we check that all the devices in a volume have the same description of said volume */
        /* TODO: above */
        std::unique_ptr<TDevice> new_device(new TPersistentDevice(Controller,
                                                                  path_to_device.c_str(),
                                                                  path,
                                                                  device_info.LogicalBlockSize, /* logical block size */
                                                                  device_info.PhysicalBlockSize, /* physical block size */
                                                                  device_info.NumLogicalBlockExposed, /* num logical block */
                                                                  do_fsync,
                                                                  do_corruption_check));
        volume->AddDevice(new_device.get(), device_info.VolumeDeviceNumber);
        PersistentDeviceSet.insert(std::move(new_device));
      } else {
        if (device_info.ReplicationFactor <= 0) {
          throw std::runtime_error("Replication factor must be >= 1");
        }
        if ((device_info.StripeSizeKB * 1024) % Util::PhysicalBlockSize != 0) {
          throw std::runtime_error("Stripe size must be a multiple of 64K");
        }
        if (device_info.NumDevicesInVolume % device_info.ReplicationFactor != 0) {
          syslog(LOG_ERR, "The number of devices [%ld] in a volume must be a multiple of the replication factor [%ld]",
                 device_info.NumDevicesInVolume,
                 device_info.ReplicationFactor);
          throw std::runtime_error("The number of devices in a volume must be a multiple of the replication factor");
        }
        std::unique_ptr<TVolume> volume(new TVolume(TVolume::TDesc{
            device_info.VolumeStrategy == 1UL ? TVolume::TDesc::Striped : TVolume::TDesc::Chained,
            TDevice::TDesc {
              TDevice::TDesc::TKind::SSD, /* TODO */
              device_info.LogicalBlockSize, /* LogicalBlockSize */
              device_info.PhysicalBlockSize, /* PhysicalBlockSize*/
              device_info.NumLogicalBlockExposed, /* NumLogicalBlock */
              device_info.LogicalBlockSize * device_info.NumLogicalBlockExposed /* Capacity */
            },
            device_info.VolumeSpeed == 1UL ? TVolume::TDesc::TStorageSpeed::Fast : TVolume::TDesc::TStorageSpeed::Slow, /* storage speed */
            device_info.ReplicationFactor, /* replication factor */
            device_info.NumDevicesInVolume / device_info.ReplicationFactor, /* the number of logical extents */
            (device_info.StripeSizeKB * 1024) / device_info.LogicalBlockSize, /* num logical block per stripe */
            device_info.MinDiscardBlocks, /* MinDiscardAllocBlocks */
            0.85 /* HighUtilizationThreshold */
          }, CacheCb, Scheduler));
        std::unique_ptr<TDevice> new_device(new TPersistentDevice(Controller,
                                                                  path_to_device.c_str(),
                                                                  path,
                                                                  device_info.LogicalBlockSize, /* logical block size */
                                                                  device_info.PhysicalBlockSize, /* physical block size */
                                                                  device_info.NumLogicalBlockExposed, /* num logical block exposed */
                                                                  do_fsync,
                                                                  do_corruption_check));
        volume->AddDevice(new_device.get(), device_info.VolumeDeviceNumber);
        VolumeById.emplace(device_info.VolumeId, std::move(volume));
        PersistentDeviceSet.insert(std::move(new_device));
      }
    }
    return true;
  });
  /* for each volume, initialize it with the logical extent set's we've collected. */
  for (const auto &vol : VolumeById) {
    string instance_name(vol.first.InstanceName);
    TExtentSet extent_set;
    const size_t num_devices = vol.second->GetNumDevices();
    for (TVolume::TDeviceCollection::TCursor csr(vol.second->GetDeviceCollection()); csr; ++csr) {
      const TPersistentDevice *device = dynamic_cast<TPersistentDevice *>(&*csr);
      assert(device);
      auto ret = StigDeviceMap.find(std::string(device->GetDevicePath()));
      assert(ret != StigDeviceMap.end());
      const auto &device_info = ret->second;
      /* this is where we check that the description of the volume stored in the device matches what we've collected */
      if (device_info.NumDevicesInVolume != num_devices) {
        syslog(LOG_ERR, "Missing device(s) for volume, found [%ld], expected [%ld]", num_devices, device_info.NumDevicesInVolume);
      }
      extent_set.insert(TLogicalExtent{device_info.LogicalExtentStart, device_info.LogicalExtentSize});
    }
    vol.second->Init(extent_set);
    auto vol_man = VolumeManagerByInstance.find(instance_name);
    if (vol_man == VolumeManagerByInstance.end()) {
      VolumeManagerByInstance.emplace(instance_name, std::unique_ptr<TVolumeManager>(new TVolumeManager(Scheduler)));
    }
    auto &volume_manager = VolumeManagerByInstance.find(instance_name)->second;
    volume_manager->AddExistingVolume(vol.second.get(), vol.first.Id);
  }
}

void TDiskUtil::List(std::stringstream &ss) const {
  assert(this);
  ss << "Volumes:" << endl;
  for (const auto &vol : VolumeById) {
    ss << "ID: " << vol.first.Id << "\t" << vol.first.InstanceName << "\t" << vol.second->GetNumDevices() << endl;
    for (TVolume::TDeviceCollection::TCursor csr(vol.second->GetDeviceCollection()); csr; ++csr) {
      ss << "\tdevice" << endl;
    }
  }
}

void TDiskUtil::CreateVolume(const std::string &instance_name,
                             const size_t num_devices,
                             const std::set<std::string> &device_set,
                             const TVolume::TDesc::TKind kind,
                             const size_t replication_factor,
                             const size_t stripe_size_in_kb,
                             const TVolume::TDesc::TStorageSpeed storage_speed,
                             bool do_fsync) {
  assert(this);
  const size_t logical_block_size = 512; /* TODO */
  const size_t physical_block_size = 512; /* TODO */

  size_t discard_support_count = 0UL;
  size_t discard_gran = 0UL;
  /* figure out our discard granularity and support */
  for (const auto &dev_name : device_set) {
    const size_t discard_max_bytes = TDeviceUtil::GetDiscardMaxBytes(dev_name);
    const size_t discard_granularity = TDeviceUtil::GetDiscardGranularity(dev_name);
    if (discard_support_count == 0) {
      discard_gran = discard_granularity;
    } else {
      if (discard_granularity != discard_gran) {
        throw std::runtime_error("All devices in a volume must have the same discard granularity");
      }
    }
    discard_support_count += (discard_max_bytes > 0UL && discard_granularity > 0UL) ? 1UL : 0UL;
  }
  if (discard_support_count != device_set.size() && discard_support_count != 0) {
    throw std::runtime_error("All devices in a volume must support or not support discard. Please do not mix and match.");
  }
  const size_t num_blocks_required_for_discard = static_cast<size_t>(ceil(static_cast<double>(discard_gran) / Util::PhysicalBlockSize));
  if (discard_support_count > 0) {
    const size_t num_bytes_required_for_discard = num_blocks_required_for_discard * Util::PhysicalBlockSize;
    if ((stripe_size_in_kb * 1024) % num_bytes_required_for_discard != 0) {
      throw std::runtime_error("Stripe size must be a multiple of ceil(discard_granularity / StigBlockSize) * StigBlockSize");
    }
  }

  if (instance_name.size() < 1 || instance_name.size() > MaxInstanceNameSize) {
    throw std::runtime_error("Instance name must be of size >= 1 and <= 24");
  }
  if (num_devices != device_set.size()) {
    throw std::runtime_error("Number of devices provided does not match");
  }
  if (num_devices == 0) {
    throw std::runtime_error("Cannot create new volume with 0 devices");
  }
  if (num_devices % replication_factor != 0) {
    throw std::runtime_error("Number of devices must be a multiple of the replication factor");
  }
  if (((stripe_size_in_kb * 1024) % (Util::PhysicalBlockSize * 8UL) != 0)) {
    stringstream ss;
    ss << "Stripe size must be a multiple of 8 * PhysicalBlockSize(" << Util::PhysicalBlockSize << ")";
    throw std::runtime_error(ss.str());
  }
  if (replication_factor == 0) {
    throw std::runtime_error("Replication factor must be >= 1");
  }
  /* find the minimum number of logical blocks supported amongst the set, then round down to a multiple of the block size, and subtract the super
     block. */
  size_t min_logical_blocks = -1;
  for (const auto &dev_path : device_set) {
    /* TODO: some devices won't use this path, figure out how to handle all devices. */
    string path_to_device_info = TDeviceUtil::GetPathToPartitionInfo(dev_path) + "size";
    Base::TFd size_fd;
    try {
      size_fd = open(path_to_device_info.c_str(), O_RDONLY);
    } catch (const std::exception &ex) {
      syslog(LOG_ERR, "Can't open size file [%s], error: [%s]", path_to_device_info.c_str(), ex.what());
      throw;
    }
    char size_max_bytes[64];
    char *ptr = size_max_bytes;
    Base::IfLt0(pread(size_fd, size_max_bytes, 64, 0));
    size_t num_logical_blocks = strtol(ptr, &ptr, 10);
    min_logical_blocks = std::min(min_logical_blocks, num_logical_blocks);
  }
  const size_t num_logical_per_stig_physical = Util::PhysicalBlockSize / logical_block_size;
  /* take off 1 for the super block */
  min_logical_blocks = ((min_logical_blocks / num_logical_per_stig_physical) - 1) * num_logical_per_stig_physical;
  /* round down to the striping factor if they want to use one */
  switch (kind) {
    case TVolume::TDesc::Striped: {
      min_logical_blocks = (((min_logical_blocks * logical_block_size) / (stripe_size_in_kb * 1024)) * (stripe_size_in_kb * 1024)) / logical_block_size;
      break;
    }
    case TVolume::TDesc::Chained: {
      break;
    }
    default: {
      throw std::logic_error("Unhandled volume strategy");
    }
  }

  auto device_iter = device_set.begin();
  TDeviceUtil::TStigDevice new_device_info;
  std::unique_ptr<TVolume> volume(new TVolume(TVolume::TDesc{
    kind,
    TDevice::TDesc {
      TDevice::TDesc::TKind::SSD, /* TODO */
      logical_block_size,
      512, /* PhysicalBlockSize*/
      min_logical_blocks, /* NumLogicalBlock */
      min_logical_blocks * logical_block_size /* Capacity */
    },
    storage_speed,
    replication_factor,
    num_devices / replication_factor,
    (stripe_size_in_kb * 1024) / logical_block_size, /* num logical block per stripe */
    num_blocks_required_for_discard, /* MinDiscardAllocBlocks */
    0.85 /* HighUtilizationThreshold */
  }, CacheCb, Scheduler));
  auto vol_man = VolumeManagerByInstance.find(instance_name);
  if (vol_man == VolumeManagerByInstance.end()) {
    VolumeManagerByInstance.emplace(instance_name, std::unique_ptr<TVolumeManager>(new TVolumeManager(Scheduler)));
  }
  auto &volume_manager = VolumeManagerByInstance.find(instance_name)->second;
  size_t volume_id_num = volume_manager->RequestNewVolumeId();
  size_t device_num = 0UL;
  for (const auto &dev_path : device_set) {
    string path_to_device = "/dev/" + dev_path;
    std::unique_ptr<TDevice> new_device(new TPersistentDevice(Controller,
                                                              path_to_device.c_str(),
                                                              dev_path.c_str(),
                                                              logical_block_size, /* logical block size */
                                                              512, /* physical block size */
                                                              min_logical_blocks, /* num logical block */
                                                              do_fsync,
                                                              true));
    volume->AddDevice(new_device.get(), device_num);
    ++device_num;
    PersistentDeviceSet.insert(std::move(new_device));
  }
  volume_manager->AddNewVolume(volume.get());
  const std::vector<TLogicalExtent> &extent_vec = volume->GetLogicalExtentVec();
  TVolumeId vol_id;
  vol_id.Id = volume_id_num;
  memset(vol_id.InstanceName, 0, MaxInstanceNameSize);
  memcpy(vol_id.InstanceName, instance_name.c_str(), instance_name.size());
  VolumeById.emplace(vol_id, std::move(volume));

  new_device_info.VolumeId = vol_id;
  new_device_info.NumDevicesInVolume = num_devices;
  new_device_info.ReplicationFactor = replication_factor;
  switch (kind) {
    case TVolume::TDesc::Striped: {
      new_device_info.VolumeStrategy = 1UL;
      new_device_info.StripeSizeKB = stripe_size_in_kb;
      break;
    }
    case TVolume::TDesc::Chained: {
      new_device_info.VolumeStrategy = 2UL;
      new_device_info.StripeSizeKB = 0UL;
      break;
    }
    default: {
      throw std::logic_error("Unhandled volume strategy");
    }
  }
  switch (storage_speed) {
    case TVolume::TDesc::TStorageSpeed::Fast: {
      new_device_info.VolumeSpeed = 1UL;
      break;
    }
    case TVolume::TDesc::TStorageSpeed::Slow: {
      new_device_info.VolumeSpeed = 2UL;
      break;
    }
    default: {
      throw std::logic_error("Unhandled volume speed");
    }
  }
  new_device_info.LogicalBlockSize = logical_block_size;
  new_device_info.PhysicalBlockSize = physical_block_size;
  new_device_info.NumLogicalBlockExposed = min_logical_blocks;
  new_device_info.MinDiscardBlocks = std::max(8UL, num_blocks_required_for_discard);
  for (size_t i = 0; i < num_devices; ++i, ++device_iter) {
    new_device_info.VolumeDeviceNumber = i;
    new_device_info.LogicalExtentStart = extent_vec[i / replication_factor].Start;
    new_device_info.LogicalExtentSize = extent_vec[i / replication_factor].Span;
    assert(device_iter != device_set.end());
    string path_to_device = "/dev/";
    path_to_device += *device_iter;
    TDeviceUtil::ModifyDevice(path_to_device.c_str(), new_device_info);
  }
  syslog(LOG_INFO, "Creating volume with [%ld] devices for volume id [%s, %ld]", num_devices, instance_name.c_str(), volume_id_num);
}

TVolumeManager *TDiskUtil::GetVolumeManager(const std::string &instance_name) const {
  assert(this);
  auto ret = VolumeManagerByInstance.find(instance_name);
  if (ret == VolumeManagerByInstance.end()) {
    throw std::runtime_error("No volume(s) for given instance name. Please use the stig disk manager to create one.");
  }
  return ret->second.get();
}