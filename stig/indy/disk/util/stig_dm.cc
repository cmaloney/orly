/* <stig/indy/disk/util/stig_dm.cc> 

   The 'main' of the Stig disk manager.

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

#include <iostream>

#include <base/booster.h>
#include <base/cmd.h>
#include <base/log.h>
#include <stig/indy/disk/util/disk_util.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Indy::Fiber;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;

/* Command-line arguments. */
class TCmd final
    : public Base::TLog::TCmd {
  public:

  /* Construct with defaults. */
  TCmd()
      : ZeroSuperBlock(false),
        List(false),
        CreateVolume(false),
        InstanceName(""),
        VolumeKind("Stripe"),
        DeviceSpeed(""),
        NumDevicesInVolume(0UL),
        ReplicationFactor(1UL),
        StripeSizeKB(64),
        ReadTest(false) {}

  /* Construct from argc/argv. */
  TCmd(int argc, char *argv[])
      : TCmd() {
    Parse(argc, argv, TMeta());
  }

  /* If true, clear the superblock of the provided devices */
  bool ZeroSuperBlock;

  /* If true, then list all the devices. */
  bool List;

  /* If true, then create a new volume. */
  bool CreateVolume;
  std::string InstanceName;
  std::string VolumeKind;
  std::string DeviceSpeed;
  size_t NumDevicesInVolume;
  size_t ReplicationFactor;
  size_t StripeSizeKB;

  /* If true, perform a read test on the specified instance volume */
  bool ReadTest;

  /* The device set. */
  std::set<std::string> DeviceSet;

  private:

  /* Our meta-type. */
  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    /* Registers our fields. */
    TMeta()
        : Base::TLog::TCmd::TMeta("Stig disk utility") {
      Param(
          &TCmd::ZeroSuperBlock, "zero-super-block", Optional, "zero-super-block\0",
          "Zero out the super block of the provded devices."
      );
      Param(
          &TCmd::ReadTest, "read-test", Optional, "read-test\0",
          "Perform a read test on the specified instance volume. (Not Implemented)"
      );
      Param(
          &TCmd::List, "list", Optional, "list\0l\0",
          "List all the devices on this system."
      );
      Param(
          &TCmd::CreateVolume, "create-volume", Optional, "create-volume\0cb\0",
          "Create a new volume."
      );
      Param(
          &TCmd::InstanceName, "instance-name", Optional, "instance-name\0iname\0",
          "The instance name used for the newly created volume."
      );
      Param(
          &TCmd::VolumeKind, "strategy", Optional, "strategy\0strat\0",
          "(stripe | chain). The strategy used when creating a new volume."
      );
      Param(
          &TCmd::DeviceSpeed, "device-speed", Optional, "device-speed\0speed\0",
          "(fast | slow). The speed of the devices used in creating a new volume."
      );
      Param(
          &TCmd::NumDevicesInVolume, "num-devices", Optional, "num-devices\0nd\0",
          "The number of device names expected when creating a new volume."
      );
      Param(
          &TCmd::ReplicationFactor, "replication-factor", Optional, "replication-factor\0rf\0",
          "The replication factor used when creating a new volume."
      );
      Param(
          &TCmd::StripeSizeKB, "stripe-size", Optional, "stripe-size\0",
          "The stripe size (in KB) used when creating a new striped volume."
      );
      Param(
          &TCmd::DeviceSet, "dev", Optional, /*"dev\0",*/
          "The list of devices on which to act."
      );
    }

  };  // TCmd::TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  ::TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  Base::TScheduler scheduler(Base::TScheduler::TPolicy(1, 64, milliseconds(10)));
  if(cmd.ZeroSuperBlock) {
    const auto &device_set = cmd.DeviceSet;
    for (const auto &device : device_set) {
      string device_path = "/dev/" + device;
      TDeviceUtil::ZeroSuperBlock(device_path.c_str());
    }
    return EXIT_SUCCESS;
  } else if (cmd.ReadTest) {
    throw std::logic_error("Read test is not implemented");
  }
  Util::TCacheCb cache_cb = [](TCacheInstr /*cache_instr*/, const TOffset /*logical_start_offset*/, void */*buf*/, size_t /*count*/) {};
  TDiskController controller;
  TDiskUtil disk_util(&scheduler, &controller, Base::TOpt<std::string>(), false /*fsync*/, cache_cb, true);
  if (cmd.List) {
    stringstream ss;
    //disk_util.List(ss);
    cout << ss.str();
    cout << "Device\t\tStigFS\tVolumeId\tVolume Device #\t# Devices in Volume\tLogical Extent Start\tLogical Extent Size\tVolume Kind\tInstance Name" << endl;
    TDeviceUtil::ForEachDevice([](const char *path) {
      TDeviceUtil::TStigDevice device_info;
      string path_to_device = "/dev/";
      path_to_device += path;
      bool ret = TDeviceUtil::ProbeDevice(path_to_device.c_str(), device_info);
      cout << "/dev/" << path << "\t";
      if (ret) {
        cout << "YES\t[" << string(device_info.VolumeId.InstanceName) << ", " << device_info.VolumeId.Id << "]"
          << "\t" << device_info.VolumeDeviceNumber
          << "\t" << device_info.NumDevicesInVolume
          << "\t" << device_info.LogicalExtentStart
          << "\t" << device_info.LogicalExtentSize
          << "\t";
        switch (device_info.VolumeStrategy) {
          case 1UL: {
            cout << "Stripe";
            break;
          }
          case 2UL: {
            cout << "Chain";
            break;
          }
        }
      }
      cout << endl;
      return true;
    });
    return EXIT_SUCCESS;
  } else if (cmd.CreateVolume) {
    TVolume::TDesc::TKind strategy;
    TVolume::TDesc::TStorageSpeed speed;
    if (strncmp(cmd.VolumeKind.c_str(), "stripe", 5) == 0) {
      strategy = TVolume::TDesc::Striped;
    } else if (strncmp(cmd.VolumeKind.c_str(), "chain", 4) == 0) {
      strategy = TVolume::TDesc::Chained;
    } else {
      throw std::runtime_error("strategy must be (stripe | chain)");
    }
    if (strncmp(cmd.DeviceSpeed.c_str(), "fast", 4) == 0) {
      speed = TVolume::TDesc::TStorageSpeed::Fast;
    } else if (strncmp(cmd.DeviceSpeed.c_str(), "slow", 4) == 0) {
      speed = TVolume::TDesc::TStorageSpeed::Slow;
    } else {
      throw std::runtime_error("device speed must be (fast | slow)");
    }
    disk_util.CreateVolume(cmd.InstanceName, cmd.NumDevicesInVolume, cmd.DeviceSet, strategy, cmd.ReplicationFactor, cmd.StripeSizeKB, speed, false /* fsync */);
  }
}
