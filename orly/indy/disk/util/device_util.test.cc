/* <orly/indy/disk/util/device_util.test.cc>

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

#include <test/kit.h>

using namespace Orly::Indy::Disk::Util;

FIXTURE(GetPathToDeviceInfo) {
  EXPECT_EQ(TDeviceUtil::GetPathToDeviceInfo("sda5"), "/sys/block/sda/");
  EXPECT_EQ(TDeviceUtil::GetPathToDeviceInfo("sr0"), "/sys/block/sr0/");
  EXPECT_EQ(TDeviceUtil::GetPathToDeviceInfo("loop0"), "/sys/block/loop0/");
  EXPECT_EQ(TDeviceUtil::GetPathToDeviceInfo("dm-2"), "/sys/block/dm-2/");

}

FIXTURE(GetPathToPartitionInfo) {
  EXPECT_EQ(TDeviceUtil::GetPathToPartitionInfo("sda5"), "/sys/block/sda/sda5/");
  EXPECT_EQ(TDeviceUtil::GetPathToPartitionInfo("sr0"), "/sys/block/sr0/");
  EXPECT_EQ(TDeviceUtil::GetPathToPartitionInfo("loop0"), "/sys/block/loop0/");
  EXPECT_EQ(TDeviceUtil::GetPathToPartitionInfo("dm-2"), "/sys/block/dm-2/");

  EXPECT_EQ(TDeviceUtil::GetPathToPartitionInfo("sdb6"), "/sys/block/sdb/sdb6/");
  EXPECT_EQ(TDeviceUtil::GetPathToPartitionInfo("sde0"), "/sys/block/sde/sde0/");
  EXPECT_EQ(TDeviceUtil::GetPathToPartitionInfo("sdaz19"), "/sys/block/sdaz/sdaz19/");
}