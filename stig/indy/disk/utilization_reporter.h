/* <stig/indy/disk/utilization_reporter.h>

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

#include <base/no_copy_semantics.h>
#include <stig/indy/disk/priority.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TUtilizationReporter {
        NO_COPY_SEMANTICS(TUtilizationReporter);
        public:

        enum TKind {
          SyncRead,
          AsyncRead,
          Write
        };

        /* TODO */
        virtual void Push(uint8_t source, TKind kind, size_t num_bytes, DiskPriority priority) = 0;

        /* TODO */
        virtual void Report(std::stringstream &ss) = 0;

        protected:

        /* TODO */
        TUtilizationReporter() {}

        /* TODO */
        virtual ~TUtilizationReporter() {}

      };  // TUtilizationReporter

    }  // Disk

  }  // Indy

}  // Stig