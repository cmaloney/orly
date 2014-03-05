/* <stig/indy/disk/meta_rewriter.cc>

   Implements <stig/indy/disk/meta_rewriter.h>.

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

#include <stig/indy/disk/meta_rewriter.h>

#include <stig/indy/disk/out_stream.h>
#include <stig/indy/disk/indy_util_reporter.h>

using namespace std;
using namespace Stig::Indy::Disk;

class TMetaRewriteInFile
    : public TInFile {
  NO_COPY_SEMANTICS(TMetaRewriteInFile);
  public:

  TMetaRewriteInFile(const Stig::Indy::Util::TBlockVec &block_vec)
      : BlockVec(block_vec) {}

  virtual size_t GetFileLength() const override {
    assert(this);
    return BlockVec.Size() * Util::LogicalBlockSize;
  }

  virtual size_t GetStartingBlock() const override {
    assert(this);
    assert(BlockVec.Size() > 0);
    return BlockVec.Front();
  }

  virtual void ReadMeta(size_t /*offset*/, size_t &/*out*/) const override {
    throw std::logic_error("Logic error: TMetaRewriteInFile should not have ReadMeta");
  }

  virtual size_t FindPageIdOfByte(size_t offset) const override {
    assert(offset <= GetFileLength());
    return ((BlockVec[offset / Util::LogicalBlockSize]) * Util::PagesPerBlock) + ((offset % Util::LogicalBlockSize) / Util::LogicalPageSize);
  }

  private:

  const Stig::Indy::Util::TBlockVec &BlockVec;

};

std::pair<size_t, size_t> TMetaRewriter::RewriteMetaData(Disk::Util::TEngine *engine, const Indy::Util::TBlockVec &block_vec, size_t starting_block_offset) {
  assert(engine);
  TMetaRewriteInFile in_file(block_vec);
  TInStream in_stream(HERE, Source::FileSync, Disk::RealTime, &in_file, engine->GetPageCache(), starting_block_offset * Disk::Util::LogicalBlockSize);
  size_t block_vec_size;
  size_t old_num_meta_blocks;
  size_t old_num_sequential_block_pairings;
  size_t num_updates;
  size_t index_map_size;
  size_t num_main_arena_notes;
  size_t num_main_arena_bytes;
  size_t num_arena_type_boundaries;
  size_t main_arena_byte_offset;
  size_t byte_offset_of_update_entries;
  in_stream.Read(block_vec_size);  // # of blocks
  in_stream.Read(old_num_meta_blocks);  // # of meta-blocks
  in_stream.Read(old_num_sequential_block_pairings);  // # of #block / block_id pairings
  in_stream.Read(num_updates);  // # of updates
  in_stream.Read(index_map_size);  // # of index segments
  in_stream.Read(num_main_arena_notes);  // # of arena notes
  in_stream.Read(num_main_arena_bytes);  // # of arena bytes
  in_stream.Read(num_arena_type_boundaries);  // # of arena type boundaries
  in_stream.Read(main_arena_byte_offset);  // byte offset of main arena
  in_stream.Read(byte_offset_of_update_entries);  // offset of update index
  std::vector<size_t> main_arena_type_boundary_offset_vec;
  const size_t to_skip = (old_num_meta_blocks * sizeof(size_t)) + (old_num_sequential_block_pairings * 2UL * sizeof(size_t));
  in_stream.Skip(to_skip); /* meta blocks + sequential block pairings */
  const size_t num_bytes_to_copy = (index_map_size * (sizeof(Base::TUuid) + sizeof(size_t))) + (num_arena_type_boundaries * sizeof(size_t));
  char buf[num_bytes_to_copy];
  in_stream.Read(buf, num_bytes_to_copy);

  Indy::Util::TBlockVec block_vec_copy = block_vec;
  block_vec_copy.Trim(old_num_meta_blocks);
  const size_t num_sequential_block_pairings = block_vec_copy.Size();

  size_t bytes_required_for_meta_data = TData::NumMetaFields * sizeof(size_t);  // meta fields
  bytes_required_for_meta_data += num_sequential_block_pairings * sizeof(size_t) * 2UL;  // num_sequential_block_pairings
  bytes_required_for_meta_data += index_map_size * (sizeof(Base::TUuid) + sizeof(size_t));  // offsets to index segment(s)
  bytes_required_for_meta_data += num_arena_type_boundaries * sizeof(size_t);  // offsets to type boundaries in arena
  size_t num_bytes_for_meta = bytes_required_for_meta_data;
  size_t num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / Disk::Util::LogicalBlockSize);
  num_bytes_for_meta = bytes_required_for_meta_data + (num_meta_blocks * sizeof(size_t));
  num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / Disk::Util::LogicalBlockSize);
  num_bytes_for_meta = bytes_required_for_meta_data + (num_meta_blocks * sizeof(size_t));
  num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / Disk::Util::LogicalBlockSize);
  Indy::Util::TBlockVec meta_block_vec;
  engine->AppendReserveBlocks(Disk::Util::TVolume::TDesc::Fast, num_meta_blocks, meta_block_vec);
  #ifndef NDEBUG
  std::unordered_set<size_t> written_block_set;
  #endif
  TCompletionTrigger trigger;
  std::unordered_map<size_t, std::shared_ptr<const Disk::TBufBlock>> collision_map;
  /* scope the stream */ {
    TDataOutStream out(HERE,
                       Disk::Source::FileSync,
                       engine->GetVolMan(),
                       0UL,
                       meta_block_vec,
                       collision_map,
                       trigger,
                       Disk::Medium,
                       true,
                       #ifndef NDEBUG
                       written_block_set,
                       #endif
                       [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
    out << block_vec_size - old_num_meta_blocks + num_meta_blocks;  // # of blocks
    out << num_meta_blocks;  // # of meta-blocks
    out << num_sequential_block_pairings;  // # of #block / block_id pairings
    out << num_updates;  // # of updates
    out << index_map_size;  // # of index segments
    out << num_main_arena_notes;  // # of arena notes
    out << num_main_arena_bytes;  // # of arena bytes
    out << num_arena_type_boundaries;  // # of arena type boundaries
    out << main_arena_byte_offset;  // byte offset of main arena
    out << byte_offset_of_update_entries;  // offset of update index
    /* write out the meta-block ids */
    for (auto meta_block_id : meta_block_vec) {
      out << meta_block_id;
    }
    /* write out the sequential block pairings */
    static_assert(std::is_same<Indy::Util::TBlockVec::TBlockMap::mapped_type, std::pair<size_t, size_t>>::value, "BlockVec mapped type must be the same as expected types for streaming block meta info for MetaRewriter");
    for (const auto &iter : block_vec_copy.GetSeqBlockMap()) {
      out << iter.second.first << iter.second.second;
    }
    out.Write(buf, num_bytes_to_copy);
  }
  trigger.Wait();
  return make_pair(meta_block_vec.Front(), (block_vec_size - old_num_meta_blocks));
}