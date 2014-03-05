/* <stig/indy/disk/loop_block_dev.h>

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

#include <sstream>

#include <base/error_utils.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      class TBlockDev {
        NO_COPY_SEMANTICS(TBlockDev);
        public:

        TBlockDev(const char *file_name, size_t file_size_mb, const char *loop_name)
            : FileName(file_name),
              LoopName(loop_name) {
          std::stringstream cmd1;
          cmd1 << "dd if=/dev/zero of=/tmp/" << FileName << " bs=512 count=" << (file_size_mb * 256UL);
          std::stringstream cmd2;
          cmd2 << "chmod a+rw /tmp/" << FileName;
          std::stringstream cmd3;
          cmd3 << "sudo /sbin/losetup /dev/" << LoopName << " /tmp/" << FileName;
          std::stringstream cmd4;
          cmd4 << "sudo /bin/chown $USER /dev/" << LoopName;
          Base::IfLt0(system(cmd1.str().c_str()));
          Base::IfLt0(system(cmd2.str().c_str()));
          Base::IfLt0(system(cmd3.str().c_str()));
          Base::IfLt0(system(cmd4.str().c_str()));
        }

        ~TBlockDev() {
          std::stringstream cmd1;
          cmd1 << "sudo /sbin/losetup -d /dev/" << LoopName;
          std::stringstream cmd2;
          cmd2 << "rm -f /tmp/" << FileName;
          Base::IfLt0(system(cmd1.str().c_str()));
          Base::IfLt0(system(cmd2.str().c_str()));
        }

        private:

        const char *FileName;
        const char *LoopName;

      };

    }  // Disk

  }  // Indy

}  // Stig