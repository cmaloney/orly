/* <stig/indy/disk/meta_rewriter.h>

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

#include <base/no_construction.h>
#include <stig/indy/disk/in_file.h>
#include <stig/indy/disk/out_stream.h>
#include <stig/indy/util/block_vec.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TMetaRewriter {
        NO_CONSTRUCTION(TMetaRewriter);
        public:

        /* TODO */
        typedef TStream<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage, 0UL> TInStream;
        typedef TOutStream<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage> TDataOutStream;

        /* TODO */
        static std::pair<size_t, size_t> RewriteMetaData(Util::TEngine *engine, const Indy::Util::TBlockVec &block_vec, size_t starting_block_offset);

      };  // TMetaRewriter

    }  // Disk

  }  // Indy

}  // Stig