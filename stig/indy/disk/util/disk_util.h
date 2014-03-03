/* <stig/indy/disk/util/disk_util.h> 

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

#include <base/no_copy_semantics.h>
#include <stig/indy/disk/util/device_util.h>
#include <stig/indy/disk/util/volume_manager.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        class TDiskUtil {
          NO_COPY_SEMANTICS(TDiskUtil);
          public:

          /* TODO */
          TDiskUtil(Base::TScheduler *scheduler,
                    TDiskController *controller,
                    const Base::TOpt<std::string> &instance_filter,
                    bool do_fsync,
                    const TCacheCb &cache_cb,
                    bool do_corruption_check = true);

          /* TODO */
          void List(std::stringstream &ss) const;

          /* TODO */
          void CreateVolume(const std::string &instance_name,
                            size_t num_devices,
                            const std::set<std::string> &device_set,
                            const TVolume::TDesc::TKind kind,
                            const size_t replication_factor,
                            const size_t stripe_size_in_kb,
                            const TVolume::TDesc::TStorageSpeed storage_speed,
                            bool do_fsync);

          /* TODO */
          TVolumeManager *GetVolumeManager(const std::string &instance_name) const;

          /* TODO */
          inline const std::unordered_set<std::unique_ptr<TDevice>> &GetPersistentDeviceSet() const {
            return PersistentDeviceSet;
          }

          private:

          /* TODO */
          Base::TScheduler *Scheduler;

          /* TODO */
          TDiskController *Controller;

          /* TODO */
          std::unordered_map<std::string, std::unique_ptr<TVolumeManager>> VolumeManagerByInstance;

          /* TODO */
          std::unordered_map<TVolumeId, std::unique_ptr<TVolume>> VolumeById;
          std::unordered_set<std::unique_ptr<TDevice>> PersistentDeviceSet;

          /* TODO */
          std::unordered_set<std::string> AllDeviceSet;

          /* TODO */
          std::unordered_map<std::string, TDeviceUtil::TStigDevice> StigDeviceMap;

          /* TODO */
          const TCacheCb &CacheCb;

        };  // TDiskUtil

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig
