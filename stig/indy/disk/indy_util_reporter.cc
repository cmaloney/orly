/* <stig/indy/disk/indy_util_reporter.cc>

   Implements <stig/indy/disk/indy_util_reporter.h>.

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

#include <stig/indy/disk/indy_util_reporter.h>

#include <stdexcept>

using namespace std;
using namespace Base;
using namespace Stig::Indy::Disk;

void TIndyUtilReporter::Push(uint8_t source, TUtilizationReporter::TKind kind, size_t num_bytes, DiskPriority /*priority*/) {
  assert(this);
  assert(source < NumFields);
  switch (kind) {
    case TUtilizationReporter::TKind::SyncRead: {
      SyncReadNumByte[source] += num_bytes;
      ++SyncReadOps[source];
      break;
    }
    case TUtilizationReporter::TKind::AsyncRead: {
      AsyncReadNumByte[source] += num_bytes;
      ++AsyncReadOps[source];
      break;
    }
    case TUtilizationReporter::TKind::Write: {
      WriteNumByte[source] += num_bytes;
      ++WriteOps[source];
      break;
    }
  }
}

void TIndyUtilReporter::Report(std::stringstream &ss) {
  assert(this);
  ReportTimer.Stop();
  double elapsed_time = ReportTimer.Total();
  ReportTimer.Reset();
  ReportTimer.Start();
  size_t sync_read_bytes[NumFields];
  size_t async_read_bytes[NumFields];
  size_t write_bytes[NumFields];
  size_t sync_read_ops[NumFields];
  size_t async_read_ops[NumFields];
  size_t write_ops[NumFields];
  for (size_t i = 0; i < NumFields; ++i) {
    sync_read_bytes[i] = SyncReadNumByte[i].exchange(0UL);
    async_read_bytes[i] = AsyncReadNumByte[i].exchange(0UL);
    write_bytes[i] = WriteNumByte[i].exchange(0UL);
    sync_read_ops[i] = SyncReadOps[i].exchange(0UL);
    async_read_ops[i] = AsyncReadOps[i].exchange(0UL);
    write_ops[i] = WriteOps[i].exchange(0UL);
  }
  for (size_t i = 0; i < NumFields; ++i) {
    const size_t sync_bytes_read = sync_read_bytes[i];
    const size_t async_bytes_read = async_read_bytes[i];
    const size_t bytes_written = write_bytes[i];
    const size_t num_sync_read_op = sync_read_ops[i];
    const size_t num_async_read_op = async_read_ops[i];
    const size_t num_write_op = write_ops[i];
    const char *name = GetName(i);
    ss << name << " Sync Read Bytes / s = " << (sync_bytes_read ? (sync_bytes_read / elapsed_time) : 0) << endl;
    ss << name << " Async Read Bytes / s = " << (async_bytes_read ? (async_bytes_read / elapsed_time) : 0) << endl;
    ss << name << " Write Bytes / s = " << (bytes_written ? (bytes_written / elapsed_time) : 0) << endl;
    ss << name << " Sync Read Ops / s = " << (num_sync_read_op ? (num_sync_read_op / elapsed_time) : 0) << endl;
    ss << name << " Async Read Ops / s = " << (num_async_read_op ? (num_async_read_op / elapsed_time) : 0) << endl;
    ss << name << " Write Ops / s = " << (num_write_op ? (num_write_op / elapsed_time) : 0) << endl;
  }
}

const char *TIndyUtilReporter::GetName(uint8_t source) const {
  assert(this);
  assert(source < NumFields);
  switch (source) {
    case BlockService: {
      return "BlockService";
    }
    case DataFileArena: {
      return "DataFileArena";
    }
    case DataFileHash: {
      return "DataFileHash";
    }
    case DataFileHashIndex: {
      return "DataFileHashIndex";
    }
    case DataFileHistory: {
      return "DataFileHistory";
    }
    case DataFileKey: {
      return "DataFileKey";
    }
    case DataFileMeta: {
      return "DataFileMeta";
    }
    case DataFileNoteIndex: {
      return "DataFileNoteIndex";
    }
    case DataFileOther: {
      return "DataFileOther";
    }
    case DataFileRemapIndex: {
      return "DataFileRemapIndex";
    }
    case DataFileUpdate: {
      return "DataFileUpdate";
    }
    case DataFileUpdateIndex: {
      return "DataFileUpdateIndex";
    }
    case DiskArena: {
      return "DiskArena";
    }
    case DurableFetch: {
      return "DurableFetch";
    }
    case DurableMergeFileData: {
      return "DurableMergeFileData";
    }
    case DurableMergeFileHash: {
      return "DurableMergeFileHash";
    }
    case DurableMergeFileHashIndex: {
      return "DurableMergeFileHashIndex";
    }
    case DurableMergeFileOther: {
      return "DurableMergeFileOther";
    }
    case DurableMergeFileScan: {
      return "DurableMergeFileScan";
    }
    case DurableSortFileData: {
      return "DurableSortFileData";
    }
    case DurableSortFileHash: {
      return "DurableSortFileHash";
    }
    case DurableSortFileHashIndex: {
      return "DurableSortFileHashIndex";
    }
    case DurableSortFileOther: {
      return "DurableSortFileOther";
    }
    case FileRemoval: {
      return "FileRemoval";
    }
    case FileService: {
      return "FileService";
    }
    case FileSync: {
      return "FileSync";
    }
    case MergeDataFileArena: {
      return "MergeDataFileArena";
    }
    case MergeDataFileHash: {
      return "MergeDataFileHash";
    }
    case MergeDataFileHashIndex: {
      return "MergeDataFileHashIndex";
    }
    case MergeDataFileHistory: {
      return "MergeDataFileHistory";
    }
    case MergeDataFileKey: {
      return "MergeDataFileKey";
    }
    case MergeDataFileMeta: {
      return "MergeDataFileMeta";
    }
    case MergeDataFileOther: {
      return "MergeDataFileOther";
    }
    case MergeDataFileRemapIndex: {
      return "MergeDataFileRemapIndex";
    }
    case MergeDataFileScan: {
      return "MergeDataFileScan";
    }
    case MergeDataFileTailIndex: {
      return "MergeDataFileTailIndex";
    }
    case MergeDataFileUpdate: {
      return "MergeDataFileUpdate";
    }
    case MergeDataFileUpdateIndex: {
      return "MergeDataFileUpdateIndex";
    }
    case PresentWalk: {
      return "PresentWalk";
    }
    case RepoLoader: {
      return "RepoLoader";
    }
    case SlaveSlush: {
      return "SlaveSlush";
    }
    case System: {
      return "System";
    }
    case UpdateScoop: {
      return "UpdateScoop";
    }
    case UpdateWalk: {
      return "UpdateWalk";
    }
  }
  throw std::runtime_error("No name for source");
}