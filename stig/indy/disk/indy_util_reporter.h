/* <stig/indy/disk/indy_util_reporter.h>

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

#include <atomic>
#include <cassert>
#include <sstream>

#include <base/timer.h>
#include <stig/indy/disk/utilization_reporter.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      enum Source : uint8_t {
        BlockService,
        DataFileArena,
        DataFileHash,
        DataFileHashIndex,
        DataFileHistory,
        DataFileKey,
        DataFileMeta,
        DataFileNoteIndex,
        DataFileOther,
        DataFileRemapIndex,
        DataFileUpdate,
        DataFileUpdateIndex,
        DiskArena,
        DurableFetch,
        DurableMergeFileData,
        DurableMergeFileHash,
        DurableMergeFileHashIndex,
        DurableMergeFileOther,
        DurableMergeFileScan,
        DurableSortFileData,
        DurableSortFileHash,
        DurableSortFileHashIndex,
        DurableSortFileOther,
        FileRemoval,
        FileService,
        FileSync,
        MergeDataFileArena,
        MergeDataFileHash,
        MergeDataFileHashIndex,
        MergeDataFileHistory,
        MergeDataFileKey,
        MergeDataFileMeta,
        MergeDataFileOther,
        MergeDataFileRemapIndex,
        MergeDataFileScan,
        MergeDataFileTailIndex,
        MergeDataFileUpdate,
        MergeDataFileUpdateIndex,
        PresentWalk,
        RepoLoader,
        SlaveSlush,
        System,
        UpdateScoop,
        UpdateWalk,
      };

      /* TODO */
      class TIndyUtilReporter : public TUtilizationReporter {
        NO_COPY_SEMANTICS(TIndyUtilReporter);
        public:

        /* TODO */
        TIndyUtilReporter() {
          ReportTimer.Start();
        }

        /* TODO */
        virtual ~TIndyUtilReporter() {}

        /* TODO */
        virtual void Push(uint8_t /*source*/, TUtilizationReporter::TKind /*kind*/, size_t /*num_bytes*/, DiskPriority /*priority*/);

        /* TODO */
        virtual void Report(std::stringstream &ss);

        private:

        /* TODO */
        const char *GetName(uint8_t) const;

        /* TODO */
        Base::TTimer ReportTimer;

        /* TODO */
        static const size_t NumFields = UpdateWalk + 1;

        /* TODO */
        std::atomic<size_t> SyncReadNumByte[NumFields];
        std::atomic<size_t> AsyncReadNumByte[NumFields];
        std::atomic<size_t> WriteNumByte[NumFields];
        std::atomic<size_t> SyncReadOps[NumFields];
        std::atomic<size_t> AsyncReadOps[NumFields];
        std::atomic<size_t> WriteOps[NumFields];

      };  // TIndyUtilReporter

    }  // Disk

  }  // Indy

}  // Stig