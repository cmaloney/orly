/* <stig/indy/file_sync.cc>

   Implements <stig/indy/file_sync.h>.

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

#include <stig/indy/file_sync.h>

#include <io/recorder_and_player.h>
#include <stig/indy/disk/indy_util_reporter.h>
#include <stig/indy/disk/meta_rewriter.h>
#include <stig/indy/disk/out_stream.h>
#include <stig/indy/disk/read_file.h>
#include <stig/indy/disk/util/snappy.h>

using namespace Stig::Indy;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;

class TFileSyncReadFile
    : public TReadFile<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> {
  NO_COPY_SEMANTICS(TFileSyncReadFile);
  public:

  typedef TStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage, 0UL> TInStream;
  typedef Snappy::TStreamSource<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> TSnappyInStream;
  typedef TOutStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> TDataOutStream;
  typedef Snappy::TBlockSink<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> TSnappyOutStream;
  typedef Stig::Indy::Disk::TReadFile<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> TMyReadFile;

  TFileSyncReadFile(Disk::Util::TEngine *engine, const Base::TUuid &file_id, size_t gen_id)
    : TMyReadFile(HERE, Source::FileSync, engine, file_id, Low, gen_id) {}

  virtual ~TFileSyncReadFile() {}

  using TReadFile::GetStartingBlockOffset;

};

const size_t TFileSync::CopyBufSize = LogicalBlockSize;

void TFileSync::Write(Io::TBinaryOutputStream &stream) const {
  assert(this);
  assert(Type == TType::Source);
  TFileSyncReadFile sync_file(Engine, FileId, GenId);
  const size_t file_length = sync_file.GetFileLength();
  stream << Context;
  stream << file_length;
  stream << sync_file.GetStartingBlockOffset();
  TFileSyncReadFile::TInStream in_stream(HERE, Disk::Source::FileSync, Low, &sync_file, Engine->GetPageCache(), 0UL);
  const size_t max_compressed = snappy::MaxCompressedLength(CopyBufSize);
  char CopyBuf[max_compressed];
  for (size_t i = 0; i < file_length; i+= CopyBufSize) {
    assert(in_stream.GetOffset() == i);
    const size_t to_copy = std::min(TFileSync::CopyBufSize, file_length - i);
    TFileSyncReadFile::TSnappyInStream snappy_in_source(in_stream, to_copy);
    Snappy::TRawSink raw_sink(CopyBuf, max_compressed);
    size_t compressed_size = snappy::Compress(&snappy_in_source, &raw_sink);
    stream << compressed_size;
    stream.WriteExactly(CopyBuf, compressed_size);
  }
}

void TFileSync::Read(Io::TBinaryInputStream &stream) {
  assert(this);
  assert(Type == TType::Destination);
  size_t file_length;
  size_t starting_block_offset;
  stream >> Context;
  stream >> file_length;
  stream >> starting_block_offset;
  Engine = reinterpret_cast<Disk::Util::TEngine *>(Context);
  const size_t num_blocks = ceil(static_cast<double>(file_length) / Disk::Util::LogicalBlockSize);
  syslog(LOG_INFO, "TFileSync::Read [%p]", Engine);
  Engine->AppendReserveBlocks(Disk::Util::TVolume::TDesc::Fast, num_blocks, BlockVec);
  #ifndef NDEBUG
  std::unordered_set<size_t> written_block_set;
  #endif
  TCompletionTrigger trigger;
  std::unordered_map<size_t, std::shared_ptr<const Disk::TBufBlock>> collision_map;
  /* scope the stream */ {
    TFileSyncReadFile::TDataOutStream out(HERE,
                                          Disk::Source::FileSync,
                                          Engine->GetVolMan(),
                                          0UL,
                                          BlockVec,
                                          collision_map,
                                          trigger,
                                          Disk::Low,
                                          true,
                                          #ifndef NDEBUG
                                          written_block_set,
                                          #endif
                                          [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
    size_t compressed_size = 0UL;
    std::unique_ptr<TBufBlock> buf_block(new TBufBlock());
    for (size_t amt_decompressed = 0UL; amt_decompressed < file_length;) {
      stream >> compressed_size;
      Snappy::TIoStreamSource stream_source(stream, compressed_size);
      bool res = snappy::RawUncompress(&stream_source, buf_block->GetData());
      if (unlikely(!res)) {
        throw std::runtime_error("Corrupted compressed data");
      }
      out.Write(buf_block->GetData(), TFileSync::CopyBufSize);
      amt_decompressed += TFileSync::CopyBufSize;
    }
  }
  trigger.Wait();
  #ifndef NDEBUG
  for (size_t block_id : BlockVec) {
    if (written_block_set.find(block_id) == written_block_set.end()) {
      syslog(LOG_ERR, "TFileSync::Read allocated blocks that did not get written to [%ld]", block_id);
      throw std::logic_error("TFileSync::Read allocated blocks that did not get written to");
    }
  }
  #endif
  auto ret = TMetaRewriter::RewriteMetaData(Engine, BlockVec, starting_block_offset);
  StartingBlockId = ret.first;
  StartingBlockOffset = ret.second;
}
