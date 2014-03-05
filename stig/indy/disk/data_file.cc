/* <stig/indy/disk/data_file.cc>

   Implements <stig/indy/disk/data_file.h>.

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

#include <stig/indy/disk/data_file.h>

#include <stig/indy/disk/in_file.h>
#include <stig/indy/disk/indy_util_reporter.h>
#include <stig/indy/disk/read_file.h>
#include <stig/indy/disk/util/hash_util.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;

class TIndexFile
    : public TInFile {
  NO_COPY_SEMANTICS(TIndexFile);
  public:

  class TOrderedNote {
    public:

    TOrderedNote(Atom::TSuprena *arena, const Atom::TCore::TNote *note)
        : Arena(arena),
          Note(note) {}

    bool operator<(const TOrderedNote &that) const {
      assert(this);
      assert(&that);
      assert(Note);
      assert(that.Note);
      assert(Arena);
      assert(that.Arena);
      Atom::TCore
        lhs_core(reinterpret_cast<Atom::TCore::TOffset>(Note), Note),
        rhs_core(reinterpret_cast<Atom::TCore::TOffset>(that.Note), that.Note);
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      Sabot::State::TAny::TWrapper
        lhs(lhs_core.NewState(Arena, lhs_state_alloc)),
        rhs(rhs_core.NewState(that.Arena, rhs_state_alloc));
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      size_t
        lhs_depth = Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(lhs->GetType(lhs_type_alloc))),
        rhs_depth = Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(rhs->GetType(rhs_type_alloc)));
      return lhs_depth < rhs_depth || (lhs_depth == rhs_depth && Atom::IsLt(Sabot::CompareStates(*lhs, *rhs, lhs_type_alloc, rhs_type_alloc)));
    }

    bool operator==(const TOrderedNote &that) const {
      assert(this);
      assert(&that);
      assert(Note);
      assert(that.Note);
      assert(Arena);
      assert(that.Arena);
      Atom::TCore
        lhs_core(reinterpret_cast<Atom::TCore::TOffset>(Note), Note),
        rhs_core(reinterpret_cast<Atom::TCore::TOffset>(that.Note), that.Note);
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      Sabot::State::TAny::TWrapper
        lhs(lhs_core.NewState(Arena, lhs_state_alloc)),
        rhs(rhs_core.NewState(that.Arena, rhs_state_alloc));
      return Atom::IsEq(Sabot::CompareStates(*lhs, *rhs, lhs_type_alloc, rhs_type_alloc));
    }

    bool operator!=(const TOrderedNote &that) const {
      assert(this);
      assert(&that);
      assert(Note);
      assert(that.Note);
      assert(Arena);
      assert(that.Arena);
      Atom::TCore
        lhs_core(reinterpret_cast<Atom::TCore::TOffset>(Note), Note),
        rhs_core(reinterpret_cast<Atom::TCore::TOffset>(that.Note), that.Note);
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      Sabot::State::TAny::TWrapper
        lhs(lhs_core.NewState(Arena, lhs_state_alloc)),
        rhs(rhs_core.NewState(that.Arena, rhs_state_alloc));
      return Atom::IsNe(Sabot::CompareStates(*lhs, *rhs, lhs_type_alloc, rhs_type_alloc));
    }

    Atom::TSuprena *Arena;

    const Atom::TCore::TNote *Note;

  };  // TOrderedNote

  /* TODO */
  class THashObj {
    public:

    /* TODO */
    THashObj(const Atom::TCore &core, size_t hash, size_t offset)
        : Core(core), Hash(hash), Offset(offset) {}

    /* TODO */
    bool operator<(const THashObj &that) const {
      assert(this);
      assert(&that);
      return Hash < that.Hash;
    }

    /* TODO */
    Atom::TCore Core;

    /* TODO */
    size_t Hash;

    /* TODO */
    size_t Offset;

  };  // THashObj

  typedef Disk::Util::TIndexManager<THashObj, Stig::Indy::Disk::Util::SortBufSize, Stig::Indy::Disk::Util::SortBufMinParallelSize> THashCollector;
  typedef Disk::Util::TIndexManager<TOrderedNote, Stig::Indy::Disk::Util::SortBufSize, Stig::Indy::Disk::Util::SortBufMinParallelSize> TOrderedNoteIndex;

  static void EmplaceOrderedNotes(TOrderedNoteIndex &note_index, Atom::TSuprena *arena, size_t &total_bytes, Atom::TCore::TOffset offset);

  TIndexFile(const Base::TUuid &index_id,
             size_t gen_id,
             Disk::Util::TEngine *engine,
             Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
             DiskPriority priority,
             TDataFile::TRemapIndex &main_arena_remap_index,
             #ifndef NDEBUG
             std::unordered_set<size_t> &written_block_set,
             #endif
             size_t temp_file_consol_thresh,
             const TKey &example_key)
      : IndexId(index_id),
        GenId(gen_id),
        Engine(engine),
        StorageSpeed(storage_speed),
        Priority(priority),
        MaxArenaBytes(0UL),
        NumArenaNotes(0UL),
        NumArenaBytes(0UL),
        MainArenaRemapIndex(main_arena_remap_index),
        NumCurKeys(0UL),
        NumHistKeys(0UL),
        EndOfHistoryStream(0UL),
        FileSize(0UL),
        TempFileConsolThresh(temp_file_consol_thresh),
        ExampleKey(example_key),
        #ifndef NDEBUG
        WrittenBlockSet(written_block_set),
        #endif
        UpdateCollector(nullptr) {
    KeyRemapper = std::bind(&TIndexFile::RemapKey, this, std::placeholders::_1);
    ValRemapper = std::bind(&TIndexFile::RemapVal, this, std::placeholders::_1);
    NumHashTables = *(ExampleKey.GetCore().TryGetElemCount());
  }

  virtual ~TIndexFile() {
    assert(this);
    /* write out the index meta-data */ {
      TDataFile::TDataOutStream meta_stream(HERE,
                                            Source::DataFileOther,
                                            Engine->GetVolMan(),
                                            ByteOffsetOfIndexMeta,
                                            *BlockVec,
                                            KeyCollisionMap,
                                            KeyTrigger,
                                            Priority,
                                            true,
                                            #ifndef NDEBUG
                                            WrittenBlockSet,
                                            #endif
                                            [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});

      meta_stream << ArenaByteOffset;  // Offset of Arena
      meta_stream << NumArenaNotes;  // number of arena notes
      meta_stream << NumArenaBytes;  // number of arena bytes
      meta_stream << ArenaTypeBoundaryOffsetVec.size();  // number of arena type boundaries
      meta_stream << NumCurKeys;  // # Current Keys
      meta_stream << NumHistKeys;  // # History Keys
      meta_stream << ByteOffsetOfKeyIndex;  // Current Key Offset
      meta_stream << NumHashTables;  // # of hash indexes (n)

      #if 0
      stringstream ss;
      ss << "=== TIndexFile: index-id[" << IndexId << "] gen-id[" << GenId << "] [" << ByteOffsetOfIndexMeta << "] block [" << (*BlockVec)[ByteOffsetOfIndexMeta / Disk::Util::LogicalBlockSize] << "] ===" << std::endl;
      ss << "\tArenaByteOffset = " << ArenaByteOffset << std::endl;
      ss << "\tNumArenaNotes = " << NumArenaNotes << std::endl;
      ss << "\tNumArenaBytes = " << NumArenaBytes << std::endl;
      ss << "\tNumArenaTypeBoundaries = " << ArenaTypeBoundaryOffsetVec.size() << std::endl;
      ss << "\tNumCurKeys = " << NumCurKeys << std::endl;
      ss << "\tNumHistKeys = " << NumHistKeys << std::endl;
      ss << "\tByteOffsetOfKeyIndex = " << ByteOffsetOfKeyIndex << std::endl;
      ss << "\tNumHashTables = " << NumHashTables << std::endl;
      std::cout << ss.str();
      #endif

      for (const auto &hash_table : NumHashFieldsByOffset) {
        meta_stream << hash_table.first << hash_table.second;
      }
      for (const auto &offset : ArenaTypeBoundaryOffsetVec) {
        meta_stream << offset;
      }
    }
    /* flush collision blocks */ {
      for (auto iter : KeyCollisionMap) {
        assert(iter.first < BlockVec->Size());
        #ifndef NDEBUG
        WrittenBlockSet.insert((*BlockVec)[iter.first]);
        #endif
        Engine->GetVolMan()->WriteBlock(HERE,
                                        Disk::Util::PageCheckedBlock,
                                        Source::DataFileOther,
                                        iter.second->GetData(),
                                        (*BlockVec)[iter.first],
                                        Priority,
                                        Disk::Util::CacheAll,
                                        KeyTrigger);
      }
    }
    KeyStream.reset();
    KeyTrigger.Wait();
  }

  void ConstructArena(TDataFile::TBlockVec &block_vec);

  void PrepKeyRange(TDataFile::TUpdateCollector *update_collector, TDataFile::TBlockVec *block_vec);

  void PushKey(TUpdate::TEntry *entry);
  Base::TOpt<Indy::TKey> PrevKeyWritten;
  std::unordered_map<size_t, shared_ptr<const TBufBlock>> KeyCollisionMap;
  std::unique_ptr<TDataFile::TDataOutStream> KeyStream;
  size_t CurKeyOffset;
  TCompletionTrigger KeyTrigger;
  Atom::TCore::TArena *CurArena;
  size_t NumHistoryElem;
  size_t ByteOffsetOfHistory;
  std::vector<TUpdate::TEntry *> HistKeys;
  std::vector<std::unique_ptr<THashCollector>> HashCollectorVec;
  std::unordered_map<uint64_t, TKey> SeenPrefix;

  void FlushHistory();

  void FlushHashes();

  Atom::TCore::TOffset RemapKey(Atom::TCore::TOffset offset);
  std::function<Atom::TCore::TOffset(Atom::TCore::TOffset)> KeyRemapper;

  Atom::TCore::TOffset RemapVal(Atom::TCore::TOffset offset);
  std::function<Atom::TCore::TOffset(Atom::TCore::TOffset)> ValRemapper;

  Base::TUuid IndexId;

  size_t GenId;

  Disk::Util::TEngine *Engine;

  Disk::Util::TVolume::TDesc::TStorageSpeed StorageSpeed;

  DiskPriority Priority;

  std::unique_ptr<TOrderedNoteIndex> ArenaNoteIndex;

  size_t MaxArenaBytes;

  size_t MaxKeyCount;

  size_t ByteOffsetOfIndexMeta;

  size_t ArenaByteOffset;
  size_t NumArenaNotes;
  size_t NumArenaBytes;
  TDataFile::TRemapIndex &MainArenaRemapIndex;
  TDataFile::TTypeBoundaryOffsetVec ArenaTypeBoundaryOffsetVec;

  size_t ByteOffsetOfKeyIndex;

  size_t NumCurKeys;

  size_t NumHistKeys;

  size_t EndOfHistoryStream;

  TDataFile::TBlockVec *BlockVec;
  size_t FileSize;

  size_t TempFileConsolThresh;

  TDataFile::TRemapIndex ArenaRemapIndex;

  TKey ExampleKey;

  #ifndef NDEBUG
  std::unordered_set<size_t> &WrittenBlockSet;
  #endif

  size_t NumHashTables;

  std::vector<std::pair<size_t, size_t>> NumHashFieldsByOffset;

  TDataFile::TUpdateCollector *UpdateCollector;

  /*
     Offset of Arena
     # arena notes
     # arena bytes
     # arena type boundaries
     # Current Keys
     # History Keys
     Current Key Offset
     # of hash indexes (n)

     (n) (size_t) -> (size_t) hash index offset -> num hash fields pairings
  */
  static const size_t NumMetaFields = TData::NumIndexMetaFields;

  private:

  /* TODO */
  virtual size_t GetFileLength() const override {
    return FileSize;
    /* we can't do this because we may not have written this far yet. this would cause problems with prefetch or checkbuf()
    return BlockVec->size() * Disk::Util::LogicalBlockSize;
    */
  }

  /* TODO */
  virtual size_t GetStartingBlock() const override {
    throw std::logic_error("TODO: implement TIndexFile::GetStartingBlock");
  }

  /* TODO */
  virtual void ReadMeta(size_t /*offset*/, size_t &/*out*/) const override {
    throw std::logic_error("TODO: implement TIndexFile::ReadMeta");
  }

  /* TODO */
  virtual size_t FindPageIdOfByte(size_t offset) const override {
    assert(offset <= GetFileLength());
    //return (((*BlockVec)[offset / Disk::Util::LogicalBlockSize]) * Disk::Util::PagesPerBlock) + ((offset % Disk::Util::LogicalBlockSize) / Disk::Util::LogicalPageSize);
    return (*BlockVec)[offset / Disk::Util::LogicalBlockSize];
  }

};  // TIndexFile

/* TODO */
size_t MakeArena(Disk::Util::TEngine *engine,
                 Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                 DiskPriority priority,
                 TIndexFile::TOrderedNoteIndex &note_index,
                 TDataFile::TRemapIndex &remap_index,
                 TDataFile::TBlockVec &block_vec,
                 #ifndef NDEBUG
                 std::unordered_set<size_t> &written_block_set,
                 #endif
                 TDataFile::TTypeBoundaryOffsetVec &type_boundary_vec,
                 size_t max_total_note_bytes,
                 size_t &num_note_out,
                 size_t &num_bytes_out);

void TIndexFile::EmplaceOrderedNotes(TOrderedNoteIndex &note_index, TSuprena *arena, size_t &total_bytes, TCore::TOffset offset) {
  const TCore::TNote *const note = reinterpret_cast<const TCore::TNote *>(offset);
  const size_t note_size = sizeof(TCore::TNote) + note->GetRawSize();
  note_index.Emplace(arena, note);
  total_bytes += note_size;
  note->ForOffset(std::bind(EmplaceOrderedNotes, ref(note_index), arena, ref(total_bytes), std::placeholders::_1));
}

void TIndexFile::PrepKeyRange(TDataFile::TUpdateCollector *update_collector, TDataFile::TBlockVec *block_vec) {
  assert(this);
  assert(MaxKeyCount);
  assert(!UpdateCollector);
  BlockVec = block_vec;
  UpdateCollector = update_collector;
  ByteOffsetOfHistory = 0UL;
  NumHistoryElem = 0UL;
  const size_t num_tuple_fields = *ExampleKey.GetCore().TryGetElemCount();
  for (size_t i = 0; i < num_tuple_fields; ++i) {
    HashCollectorVec.emplace_back(new THashCollector(HERE, Source::DataFileHashIndex, TempFileConsolThresh, StorageSpeed, Engine, true));
  }
  const size_t bytes_of_metadata = (TData::NumMetaFields * sizeof(size_t)) + (NumHashTables * sizeof(size_t) * 2UL) + (ArenaTypeBoundaryOffsetVec.size() * sizeof(size_t));
  if (bytes_of_metadata >= Disk::Util::LogicalBlockSize) {
    throw std::runtime_error("Index metadata >= 1 block");
  }
  size_t max_blocks_required = ceil(static_cast<double>((MaxKeyCount * TDataFile::KeyEntrySize) + bytes_of_metadata) / Disk::Util::LogicalBlockSize);
  ByteOffsetOfIndexMeta = BlockVec->Size() * Disk::Util::LogicalBlockSize;
  ByteOffsetOfKeyIndex = ByteOffsetOfIndexMeta + bytes_of_metadata;

  /* collision at beginning of this key range. We're going to leave some room to include this index's metadata */
  auto ret = KeyCollisionMap.insert(make_pair(ByteOffsetOfIndexMeta / Disk::Util::LogicalBlockSize, nullptr));
  if (ret.second) { // fresh insert
    ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
  }

  Engine->AppendReserveBlocks(StorageSpeed, max_blocks_required, *BlockVec);
  KeyStream = std::unique_ptr<TDataFile::TDataOutStream>(new TDataFile::TDataOutStream(HERE,
                                                                                       Source::DataFileKey,
                                                                                       Engine->GetVolMan(),
                                                                                       ByteOffsetOfKeyIndex,
                                                                                       *BlockVec,
                                                                                       KeyCollisionMap,
                                                                                       KeyTrigger,
                                                                                       Priority,
                                                                                       true,
                                                                                       #ifndef NDEBUG
                                                                                       WrittenBlockSet,
                                                                                       #endif
                                                                                       [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;}));
}

Atom::TCore::TOffset TIndexFile::RemapKey(Atom::TCore::TOffset offset) {
  assert(this);
  assert(CurArena);
  auto pos = ArenaRemapIndex.find(TRemapObj(CurArena, offset, 0UL /* not used */));
  assert(pos != ArenaRemapIndex.end());
  return pos->NewKey;
}

Atom::TCore::TOffset TIndexFile::RemapVal(Atom::TCore::TOffset offset) {
  assert(this);
  assert(CurArena);
  auto pos = MainArenaRemapIndex.find(TRemapObj(CurArena, offset, 0UL /* not used */));
  assert(pos != MainArenaRemapIndex.end());
  return pos->NewKey;
}

void TIndexFile::ConstructArena(TDataFile::TBlockVec &block_vec) {
  ArenaByteOffset = MakeArena(Engine,
                              StorageSpeed,
                              Priority,
                              *ArenaNoteIndex,
                              ArenaRemapIndex,
                              block_vec,
                              #ifndef NDEBUG
                              WrittenBlockSet,
                              #endif
                              ArenaTypeBoundaryOffsetVec,
                              MaxArenaBytes,
                              NumArenaNotes,
                              NumArenaBytes);
}

void TIndexFile::PushKey(TUpdate::TEntry *entry) {
  assert(this);
  assert(UpdateCollector);
  const TKey &key = entry->GetKey();
  #ifndef NDEBUG
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  Sabot::Type::TAny::TWrapper lhs_wrapper(key.GetCore().GetType(key.GetArena(), lhs_type_alloc));
  Sabot::Type::TAny::TWrapper rhs_wrapper(ExampleKey.GetCore().GetType(ExampleKey.GetArena(), rhs_type_alloc));
  /*
  if (Atom::IsNe(Sabot::CompareTypes(*lhs_wrapper,
                                     *rhs_wrapper))) {
    std::cout << "Types must match for same index id [";
    lhs_wrapper->Accept(Sabot::TTypeDumper(std::cout));
    std::cout << "] vs. [";
    rhs_wrapper->Accept(Sabot::TTypeDumper(std::cout));
    std::cout << "]" << std::endl;
  }
  */
  #endif
  assert(Atom::IsEq(Sabot::CompareTypes(*lhs_wrapper,
                                        *rhs_wrapper)));
  if (!PrevKeyWritten || key != *PrevKeyWritten) {  // Current Key
    ++NumCurKeys;
    TDataFile::TDataOutStream &stream = *KeyStream;
    if (PrevKeyWritten) {
      stream << NumHistoryElem << ByteOffsetOfHistory;
      ByteOffsetOfHistory += NumHistoryElem * TDataFile::KeyHistorySize;
      NumHistoryElem = 0UL;
    }
    const TSequenceNumber seq_num = entry->GetSequenceNumber();
    const TCore &val = entry->GetOp();
    PrevKeyWritten = key;
    CurKeyOffset = stream.GetOffset();
    stream << seq_num;
    TCore key_core = key.GetCore();
    TCore val_core = val;

    /* set the pre-computed hash */ {
      size_t key_hash = key.GetHash();
      key_core.TrySetStoredHash(key_hash);
    }

    TCore prefix_core = key_core;
    TCore::TArena *const prefix_arena = key.GetArena();

    CurArena = key.GetArena();
    key_core.Remap(KeyRemapper);
    val_core.Remap(ValRemapper);
    stream.Write(&key_core, sizeof(key_core));
    stream.Write(&val_core, sizeof(val_core));

    void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
    #ifndef NDEBUG
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    #endif
    do {
      const TKey prefix_key(prefix_core, prefix_arena);
      const uint64_t prefix_size = Sabot::GetTupleSize(*Sabot::Type::TAny::TWrapper(prefix_core.GetType(prefix_arena, type_alloc)));
      auto ret = SeenPrefix.insert(make_pair(prefix_size, prefix_key));
      bool is_new = ret.second;
      if (!is_new) {
        TKey &cur_prefix = ret.first->second;
        if (cur_prefix != prefix_key) {
          assert(Atom::IsLt(cur_prefix.Compare(prefix_key)));
          cur_prefix = prefix_key;
          is_new = true;
        }
      }
      if (is_new) {
        TCore remapped_prefix_core(prefix_core, KeyRemapper);

        assert(prefix_key.GetHash() == Sabot::GetHash(*Sabot::State::TAny::TWrapper(prefix_core.NewState(prefix_arena, state_alloc))));
        size_t prefix_hash = prefix_key.GetHash();
        remapped_prefix_core.TrySetStoredHash(prefix_hash);
        assert(HashCollectorVec.size() > prefix_size - 1);
        HashCollectorVec[prefix_size - 1]->Emplace(remapped_prefix_core, prefix_hash, CurKeyOffset);
      } else {
        break;
      }

    } while (prefix_core.TryTruncateTuple());
    UpdateCollector->Emplace(seq_num, entry->GetMetadata(), entry->GetId(), CurKeyOffset, true, IndexId);

  } else {  // History Key
    ++NumHistKeys;
    HistKeys.push_back(entry);
    UpdateCollector->Emplace(entry->GetSequenceNumber(), entry->GetMetadata(), entry->GetId(), ByteOffsetOfHistory + NumHistoryElem * TDataFile::KeyHistorySize, false, IndexId);
    ++NumHistoryElem;
  }
}

void TIndexFile::FlushHistory() {
  assert(this);
  TDataFile::TDataOutStream &stream = *KeyStream;
  if (PrevKeyWritten) {
    stream << NumHistoryElem << ByteOffsetOfHistory;
  }
  ByteOffsetOfHistory = stream.GetOffset();
  for (const TUpdate::TEntry *entry : HistKeys) {
    const TSequenceNumber seq_num = entry->GetSequenceNumber();
    const TKey &key = entry->GetKey();
    const TCore &val = entry->GetOp();
    stream << seq_num;
    TCore key_core = key.GetCore();
    TCore val_core = val;
    CurArena = key.GetArena();
    key_core.Remap(KeyRemapper);
    val_core.Remap(ValRemapper);
    stream.Write(&key_core, sizeof(key_core));
    stream.Write(&val_core, sizeof(val_core));
  }
  EndOfHistoryStream = stream.GetOffset();
}

void TIndexFile::FlushHashes() {
  assert(this);
  size_t total_bytes_required = 0UL;
  unordered_map<size_t, shared_ptr<const TBufBlock>> collision_map {};
  size_t hash_index_byte_offset = BlockVec->Size() * Disk::Util::LogicalBlockSize;
  for (const auto &collection : HashCollectorVec) {
    /* collision at beginning of this hash index */
    auto ret = collision_map.insert(make_pair((hash_index_byte_offset + total_bytes_required) / Disk::Util::LogicalBlockSize, nullptr));
    if (ret.second) { // fresh insert
      ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
    }
    size_t num_hash_fields = Disk::Util::SuggestHashSize(collection->GetSize());
    NumHashFieldsByOffset.emplace_back(hash_index_byte_offset + total_bytes_required, num_hash_fields);
    total_bytes_required += num_hash_fields * TDataFile::HashEntrySize;
    /* collision at end of this hash index (if the block doesn't get filled completely) */
    if ((hash_index_byte_offset + total_bytes_required) % Disk::Util::LogicalBlockSize != 0) {
      ret = collision_map.insert(make_pair((hash_index_byte_offset + total_bytes_required) / Disk::Util::LogicalBlockSize, nullptr));
      if (ret.second) { // fresh insert
        ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
      }
    }
  }
  assert(NumHashFieldsByOffset.size() == NumHashTables);
  size_t max_blocks_required = ceil(static_cast<double>(total_bytes_required) / Disk::Util::LogicalBlockSize);

  Engine->AppendReserveBlocks(StorageSpeed, max_blocks_required, *BlockVec);
  TCompletionTrigger completion_trigger;
  for (const auto &collection : HashCollectorVec) {
    size_t num_hash_fields = Disk::Util::SuggestHashSize(collection->GetSize());
    /* make a new index with the hashes modded by the hash field size */
    THashCollector modded_hash_collector(HERE, Source::DataFileHashIndex, TempFileConsolThresh, StorageSpeed, Engine, true);
    for (THashCollector::TCursor orig_csr(collection.get(), 32UL); orig_csr; ++orig_csr) {
      const THashObj &obj = *orig_csr;
      modded_hash_collector.Emplace(obj.Core, obj.Hash % num_hash_fields, obj.Offset);
    }
    THashCollector::TCursor hash_csr(&modded_hash_collector, 32UL);
    /* do the first pass over the hash map */ {
      assert(hash_index_byte_offset + num_hash_fields * TDataFile::HashEntrySize <= BlockVec->Size() * Disk::Util::LogicalBlockSize);
      TDataFile::TDataOutStream stream(HERE,
                                       Source::DataFileHashIndex,
                                       Engine->GetVolMan(),
                                       hash_index_byte_offset,
                                       *BlockVec,
                                       collision_map,
                                       completion_trigger,
                                       Priority,
                                       true,
                                       #ifndef NDEBUG
                                       WrittenBlockSet,
                                       #endif
                                       [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
      for (size_t i = 0; i < num_hash_fields; ++i) {
        if (hash_csr && (*hash_csr).Hash <= i) {
          const THashObj &obj = *hash_csr;
          assert(obj.Core.IsTuple());
          #ifndef NDEBUG
          obj.Core.ForceGetStoredHash();
          #endif
          stream.Write(&obj.Core, sizeof(TCore));
          stream << obj.Offset;
          ++hash_csr;
        } else {
          stream.Write(&TData::NullCore, sizeof(TCore));
          stream << 0UL;
        }
      }
      FileSize = stream.GetOffset();
    }
    /* do the second pass if it is required */ {
      if (hash_csr) {
        /* flush the collision blocks surrounding the hash index */ {
          const size_t collision_block_pos = hash_index_byte_offset / Disk::Util::LogicalBlockSize;
          assert(collision_block_pos < BlockVec->Size());
          assert(collision_map.find(collision_block_pos) != collision_map.end());
          #ifndef NDEBUG
          WrittenBlockSet.insert((*BlockVec)[collision_block_pos]);
          #endif
          Engine->GetVolMan()->WriteBlock(HERE,
                                          Disk::Util::PageCheckedBlock,
                                          Source::DataFileHash,
                                          collision_map[collision_block_pos]->GetData(),
                                          (*BlockVec)[collision_block_pos],
                                          Priority,
                                          Disk::Util::CacheAll,
                                          completion_trigger);
          const size_t byte_offset_of_end = hash_index_byte_offset + num_hash_fields * TDataFile::HashEntrySize;
          const size_t end_block_pos = byte_offset_of_end / Disk::Util::LogicalBlockSize;
          if ((collision_block_pos != end_block_pos) && (end_block_pos < BlockVec->Size())) {
            assert(end_block_pos < BlockVec->Size());
            assert(collision_map.find(end_block_pos) != collision_map.end());
            #ifndef NDEBUG
            WrittenBlockSet.insert((*BlockVec)[end_block_pos]);
            #endif
            Engine->GetVolMan()->WriteBlock(HERE,
                                            Disk::Util::PageCheckedBlock,
                                            Source::DataFileHash,
                                            collision_map[end_block_pos]->GetData(),
                                            (*BlockVec)[end_block_pos],
                                            Priority,
                                            Disk::Util::CacheAll,
                                            completion_trigger);
          }
        }
        /* wait for the first pass to finish flushing before doing the wrap around pass */ {
          completion_trigger.Wait();
        }
        TDataFile::TDataOutStream out_stream(HERE,
                                             Source::DataFileHashIndex,
                                             Engine->GetVolMan(),
                                             hash_index_byte_offset,
                                             *BlockVec,
                                             collision_map,
                                             completion_trigger,
                                             Priority,
                                             true,
                                             #ifndef NDEBUG
                                             WrittenBlockSet,
                                             #endif
                                             [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
        TDataFile::TDataInStream<0UL> in_stream(HERE, Source::DataFileHash, Priority, this, Engine->GetCache<TDataFile::TDataInStream<0UL>::PhysicalCachePageSize>(), hash_index_byte_offset);
        TCore core;
        size_t key_ptr = 0;
        size_t i = 0;
        for (; i < num_hash_fields; ++i) {
          in_stream.Read(&core, sizeof(TCore));
          in_stream.Read(key_ptr);
          if (memcmp(&core, &TData::NullCore, sizeof(TCore)) == 0) {
            assert(hash_csr);
            const THashObj &obj = *hash_csr;
            assert(obj.Core.IsTuple());
            #ifndef NDEBUG
            obj.Core.ForceGetStoredHash();
            #endif
            out_stream.Write(&obj.Core, sizeof(TCore));
            out_stream << obj.Offset;
            ++hash_csr;
          } else {
            out_stream.Write(&core, sizeof(TCore));
            out_stream << key_ptr;
          }
          if (!hash_csr) {
            break;
          }
        }
        size_t left = std::min(Disk::Util::LogicalBlockSize - in_stream.GetOffset() % Disk::Util::LogicalBlockSize, FileSize - in_stream.GetOffset());
        char buf[left];
        in_stream.Read(buf, left);
        out_stream.Write(buf, left);
        assert(!hash_csr);
      }
    }
    hash_index_byte_offset += num_hash_fields * TDataFile::HashEntrySize;
  }
  /* flush collision blocks */ {
    for (auto iter : collision_map) {
      assert(iter.first < BlockVec->Size());
      #ifndef NDEBUG
      WrittenBlockSet.insert((*BlockVec)[iter.first]);
      #endif
      Engine->GetVolMan()->WriteBlock(HERE,
                                      Disk::Util::PageCheckedBlock,
                                      Source::DataFileOther,
                                      iter.second->GetData(),
                                      (*BlockVec)[iter.first],
                                      Priority,
                                      Disk::Util::CacheAll,
                                      completion_trigger);
    }
    completion_trigger.Wait();
  }
  HashCollectorVec.clear();
}

TDataFile::TDataFile(Util::TEngine *engine,
                     Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                     TMemoryLayer *memory_layer,
                     const Base::TUuid &file_uid,
                     size_t gen_id,
                     size_t temp_file_consol_thresh,
                     TSequenceNumber /*release_up_to*/,
                     DiskPriority priority)
    : Engine(engine),
      StorageSpeed(storage_speed),
      Priority(priority),
      NumUpdates(0UL),
      MainArenaByteOffset(0UL),
      NumKeys(0UL),
      TempFileConsolThresh(temp_file_consol_thresh),
      UpdateCollector(HERE, Source::DataFileUpdateIndex, TempFileConsolThresh, StorageSpeed, Engine, true) {
  assert(this);
  try {
    std::unique_ptr<TIndexFile::TOrderedNoteIndex> main_arena_note_index(new TIndexFile::TOrderedNoteIndex(HERE,
                                                                                                           Source::DataFileNoteIndex,
                                                                                                           TempFileConsolThresh,
                                                                                                           StorageSpeed,
                                                                                                           Engine,
                                                                                                           true));
    std::unordered_map<Base::TUuid, std::unique_ptr<TIndexFile>> index_map;
    size_t main_arena_max_bytes = 0UL;
    /* compute the number of updates */ {
      for (TMemoryLayer::TUpdateCollection::TCursor csr(memory_layer->GetUpdateCollection()); csr; ++csr) {
        ++NumUpdates;
      }
    }
    /* generate the arena index for each key index */ {
      Base::TOpt<Base::TUuid> prev_index_id;
      TUpdate::TEntry *prev_entry = nullptr;
      std::unique_ptr<TIndexFile::TOrderedNoteIndex> ordered_note_index;
      size_t cur_index_bytes = 0UL;
      size_t max_key_count = 0UL;
      for (TMemoryLayer::TEntryCollection::TCursor csr(memory_layer->GetEntryCollection()); csr; ++csr) {
        const Base::TUuid &cur_idx_id = csr->GetIndexKey().GetIndexId();
        if (!prev_index_id || cur_idx_id != *prev_index_id) {
          if (prev_index_id) {
            assert(prev_entry);
            auto ret = index_map.emplace(*prev_index_id, std::unique_ptr<TIndexFile>(new TIndexFile(*prev_index_id,
                                                                                                    gen_id,
                                                                                                    Engine,
                                                                                                    StorageSpeed,
                                                                                                    Priority,
                                                                                                    MainArenaRemapIndex,
                                                                                                    #ifndef NDEBUG
                                                                                                    WrittenBlockSet,
                                                                                                    #endif
                                                                                                    TempFileConsolThresh,
                                                                                                    prev_entry->GetKey())));
            TIndexFile &index_file = *ret.first->second;
            std::swap(index_file.ArenaNoteIndex, ordered_note_index);
            index_file.MaxArenaBytes = cur_index_bytes;
            index_file.MaxKeyCount = max_key_count;
          }
          cur_index_bytes = 0UL;
          max_key_count = 0UL;
          prev_index_id = cur_idx_id;
          ordered_note_index = std::unique_ptr<TIndexFile::TOrderedNoteIndex>(new TIndexFile::TOrderedNoteIndex(HERE,
                                                                                                                Source::DataFileNoteIndex,
                                                                                                                TempFileConsolThresh,
                                                                                                                StorageSpeed,
                                                                                                                Engine,
                                                                                                                true));
        }
        TSuprena &cur_arena = csr->GetSuprena();
        /* push the key's arena */ {
          const TCore &key_core = csr->GetKey().GetCore();
          const TCore::TOffset *offset = key_core.TryGetOffset();
          if (offset) {
            TIndexFile::EmplaceOrderedNotes(*ordered_note_index, &cur_arena, cur_index_bytes, *offset);
          }
        }
        /* push the main arena : value, meta, id */ {
          const TCore &val_core = csr->GetOp();
          const TCore &meta_core = csr->GetMetadata();
          const TCore &id_core = csr->GetId();
          const TCore::TOffset *offset = val_core.TryGetOffset();
          if (offset) {
            TIndexFile::EmplaceOrderedNotes(*main_arena_note_index, &cur_arena, main_arena_max_bytes, *offset);
          }
          offset = meta_core.TryGetOffset();
          if (offset) {
            TIndexFile::EmplaceOrderedNotes(*main_arena_note_index, &cur_arena, main_arena_max_bytes, *offset);
          }
          offset = id_core.TryGetOffset();
          if (offset) {
            TIndexFile::EmplaceOrderedNotes(*main_arena_note_index, &cur_arena, main_arena_max_bytes, *offset);
          }
        }
        ++max_key_count;
        prev_entry = &*csr;
      }
      if (prev_index_id) {
        assert(prev_entry);
        auto ret = index_map.emplace(*prev_index_id, std::unique_ptr<TIndexFile>(new TIndexFile(*prev_index_id,
                                                                                                gen_id,
                                                                                                Engine,
                                                                                                StorageSpeed,
                                                                                                Priority,
                                                                                                MainArenaRemapIndex,
                                                                                                #ifndef NDEBUG
                                                                                                WrittenBlockSet,
                                                                                                #endif
                                                                                                TempFileConsolThresh,
                                                                                                prev_entry->GetKey())));
        TIndexFile &index_file = *ret.first->second;
        std::swap(index_file.ArenaNoteIndex, ordered_note_index);
        index_file.MaxArenaBytes = cur_index_bytes;
        index_file.MaxKeyCount = max_key_count;
      }
    }  // done generating the arena index
    for (const auto &iter : index_map) {
      TIndexFile &index_file = *iter.second;
      if (index_file.MaxArenaBytes) {
        index_file.ConstructArena(BlockVec);
      }
    }
    /* write out the main arena */
    size_t num_main_arena_notes = 0UL;
    size_t num_main_arena_bytes = 0UL;
    if (main_arena_max_bytes) {
      MainArenaByteOffset = MakeArena(Engine,
                                      StorageSpeed,
                                      Priority,
                                      *main_arena_note_index,
                                      MainArenaRemapIndex,
                                      BlockVec,
                                      #ifndef NDEBUG
                                      WrittenBlockSet,
                                      #endif
                                      MainArenaTypeBoundaryOffsetVec,
                                      main_arena_max_bytes,
                                      num_main_arena_notes,
                                      num_main_arena_bytes);
    }
    /* write the in-order key indexes */ {
      Base::TOpt<Base::TUuid> prev_index_id;
      TIndexFile *cur_index_file = nullptr;
      for (TMemoryLayer::TEntryCollection::TCursor csr(memory_layer->GetEntryCollection()); csr; ++csr) {
        const Base::TUuid &cur_idx_id = csr->GetIndexKey().GetIndexId();
        if (!prev_index_id || cur_idx_id != *prev_index_id) {

          /* if we just finished the last key for an index, flush the history... */
          if (cur_index_file) {
            /* flush history section */
            cur_index_file->FlushHistory();
            /* now release the unused blocks at the end. */
            const size_t end_of_stream = cur_index_file->EndOfHistoryStream;
            const size_t actual_blocks_required = ((end_of_stream - 1UL) / Disk::Util::LogicalBlockSize) + 1UL;
            const size_t num_to_remove = BlockVec.Size() - actual_blocks_required;
            BlockVec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
              Engine->FreeSeqBlocks(block_id, num_blocks);
              return true;
            }, actual_blocks_required, BlockVec.Size());
            BlockVec.Trim(num_to_remove);
            assert(BlockVec.Size() == actual_blocks_required);
          }

          auto ret = index_map.find(cur_idx_id);
          assert(ret != index_map.end());
          cur_index_file = ret->second.get();
          cur_index_file->PrepKeyRange(&UpdateCollector, &BlockVec);
          prev_index_id = cur_idx_id;
        }
        cur_index_file->CurArena = &csr->GetSuprena();
        cur_index_file->PushKey(&*csr);
      }
      assert(cur_index_file);
      /* flush the last index's history */ {
        /* flush history section */
        cur_index_file->FlushHistory();
        /* now release the unused blocks at the end. */
        const size_t end_of_stream = cur_index_file->EndOfHistoryStream;
        const size_t actual_blocks_required = ((end_of_stream - 1UL) / Disk::Util::LogicalBlockSize) + 1UL;
        const size_t num_to_remove = BlockVec.Size() - actual_blocks_required;
        BlockVec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
          Engine->FreeSeqBlocks(block_id, num_blocks);
          return true;
        }, actual_blocks_required, BlockVec.Size());
        BlockVec.Trim(num_to_remove);
        assert(BlockVec.Size() == actual_blocks_required);
      }
      /* flush the hash sections */ {
        for (const auto &index : index_map) {
          TIndexFile &index_file = *index.second;
          index_file.FlushHashes();
        }
      }
    }

    TCompletionTrigger completion_trigger;
    const size_t num_bytes_required_for_update_idx = (NumUpdates * UpdateBucketEntrySize) + (UpdateCollector.GetSize() * UpdateKeyPtrSize);
    const size_t byte_offset_of_update_entries = BlockVec.Size() * Disk::Util::LogicalBlockSize;
    const size_t byte_offset_of_bucket_entries = byte_offset_of_update_entries + (NumUpdates * UpdateBucketEntrySize);
    /* write out the update index */ {
      size_t blocks_required = ceil(static_cast<double>(num_bytes_required_for_update_idx) / Disk::Util::LogicalBlockSize);
      engine->AppendReserveBlocks(StorageSpeed, blocks_required, BlockVec);
      unordered_map<size_t, shared_ptr<const TBufBlock>> collision_map {};
      auto ret = collision_map.insert(make_pair(byte_offset_of_bucket_entries / Disk::Util::LogicalBlockSize, nullptr));
      if (ret.second) { // fresh insert
        ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
      }
      /* update idx stream life-span */ {
        TDataOutStream seq_stream(HERE,
                                  Source::DataFileUpdate,
                                  Engine->GetVolMan(),
                                  byte_offset_of_update_entries,
                                  BlockVec,
                                  collision_map,
                                  completion_trigger,
                                  Priority,
                                  true,
                                  #ifndef NDEBUG
                                  WrittenBlockSet,
                                  #endif
                                  [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
        TDataOutStream ptr_stream(HERE,
                                  Source::DataFileUpdate,
                                  Engine->GetVolMan(),
                                  byte_offset_of_bucket_entries,
                                  BlockVec,
                                  collision_map,
                                  completion_trigger,
                                  Priority,
                                  true,
                                  #ifndef NDEBUG
                                  WrittenBlockSet,
                                  #endif
                                  [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
        TUpdateCollector::TCursor update_csr(&UpdateCollector, 32UL);
        assert(update_csr);
        TSequenceNumber cur_seq_num = (*update_csr).SequenceNumber;
        TCore cur_meta, cur_id;
        size_t bucket_ptr = byte_offset_of_bucket_entries;
        size_t num_accum = 0U;
        LowestSeq = (*update_csr).SequenceNumber;
        while (update_csr) {
          const TUpdateObj &obj = *update_csr;
          if (obj.SequenceNumber != cur_seq_num) {
            seq_stream << cur_seq_num;  // Sequence Number
            seq_stream.Write(&cur_meta, sizeof(cur_meta));  // Metadata
            seq_stream.Write(&cur_id, sizeof(cur_id));  // Id
            seq_stream << bucket_ptr;  // Byte offset of bucket
            seq_stream << num_accum;  // number of key ptrs in bucket


            cur_seq_num = obj.SequenceNumber;
            cur_meta = obj.Metadata;
            cur_id = obj.Id;
            bucket_ptr += num_accum * TData::UpdateKeyPtrSize;
            num_accum = 0U;
          }
          if (obj.IsCurrent) {  /* current key, NO need to re-adjust offset */
            ptr_stream << obj.Offset;  // key_ptr
          } else {  /* history key, need to re-adjust offset */
            ptr_stream << static_cast<size_t>(index_map[obj.IndexId]->ByteOffsetOfHistory + obj.Offset);
          }
          ++num_accum;
          ++update_csr;
        }
        HighestSeq = cur_seq_num;
        assert(HighestSeq >= LowestSeq);
        seq_stream << cur_seq_num;  // Sequence Number
        seq_stream.Write(&cur_meta, sizeof(cur_meta));  // Metadata
        seq_stream.Write(&cur_id, sizeof(cur_id));  // Id
        seq_stream << bucket_ptr;  // Byte offset of bucket
        seq_stream << num_accum;  // number of key ptrs in bucket
      }
      /* flush collision blocks */ {
        for (auto iter : collision_map) {
          assert(iter.first < BlockVec.Size());
          #ifndef NDEBUG
          WrittenBlockSet.insert(BlockVec[iter.first]);
          #endif
          Engine->GetVolMan()->WriteBlock(HERE,
                                          Disk::Util::PageCheckedBlock,
                                          Source::DataFileOther,
                                          iter.second->GetData(),
                                          BlockVec[iter.first],
                                          Priority,
                                          Disk::Util::CacheAll,
                                          completion_trigger);
        }
        /* wait for the io to finish since the collision map is about to go out of scope. */
        completion_trigger.Wait();
      }
    }
    size_t num_blocks = 0UL;
    size_t num_meta_blocks = 0UL;
    /* compute / write out the meta-data */ {
      /*
        # of blocks
        # of meta-blocks (n)
        # of #block / block_id pairings (number of sequential blocks starting at) (m)
        # updates
        # index segments (p)
        # of arena notes
        # of arena bytes
        offset of main arena
        offset of update index

        n (size_t) metablock block_id(s)
        m (size_t) -> (size_t) #block -> starting_block_id pairings
        p (Base::TUuid) -> (size_t) offsets to index segment(s)
      */

      const TBlockVec::TBlockMap seq_block_map_copy = BlockVec.GetSeqBlockMap();
      const size_t num_sequential_block_pairings = seq_block_map_copy.size();

      const size_t start_of_meta_data = BlockVec.Size() * Disk::Util::LogicalBlockSize;

      size_t bytes_required_for_meta_data = TData::NumMetaFields * sizeof(size_t);  // meta fields
      bytes_required_for_meta_data += num_sequential_block_pairings * sizeof(size_t) * 2UL;  // num_sequential_block_pairings
      bytes_required_for_meta_data += index_map.size() * (sizeof(Base::TUuid) + sizeof(size_t));  // offsets to index segment(s)
      bytes_required_for_meta_data += MainArenaTypeBoundaryOffsetVec.size() * sizeof(size_t);  // offsets to type boundaries in arena

      num_blocks = BlockVec.Size();
      size_t num_bytes_for_meta = bytes_required_for_meta_data;
      num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / Disk::Util::LogicalBlockSize);
      num_bytes_for_meta = bytes_required_for_meta_data + (num_meta_blocks * sizeof(size_t));
      num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / Disk::Util::LogicalBlockSize);
      num_bytes_for_meta = bytes_required_for_meta_data + (num_meta_blocks * sizeof(size_t));
      num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / Disk::Util::LogicalBlockSize);

      num_blocks += num_meta_blocks;

      engine->AppendReserveBlocks(StorageSpeed, num_meta_blocks, BlockVec);

      unordered_map<size_t, shared_ptr<const TBufBlock>> collision_map {};
      TDataOutStream stream(HERE,
                            Source::DataFileMeta,
                            Engine->GetVolMan(),
                            start_of_meta_data,
                            BlockVec,
                            collision_map,
                            completion_trigger,
                            Priority,
                            true,
                            #ifndef NDEBUG
                            WrittenBlockSet,
                            #endif
                            [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
      stream << BlockVec.Size();  // # of blocks
      stream << num_meta_blocks;  // # of meta-blocks
      stream << num_sequential_block_pairings;  // # of #block / block_id pairings
      stream << NumUpdates;  // # of updates
      stream << index_map.size();  // # of index segments
      stream << num_main_arena_notes;  // # of arena notes
      stream << num_main_arena_bytes;  // # of arena bytes
      stream << MainArenaTypeBoundaryOffsetVec.size();  // # of arena type boundaries
      stream << MainArenaByteOffset;  // byte offset of main arena
      stream << byte_offset_of_update_entries;  // offset of update index
      assert((stream.GetOffset() - start_of_meta_data) / sizeof(size_t) == TData::NumMetaFields);

      assert(stream.GetOffset() == start_of_meta_data + (TData::NumMetaFields * sizeof(size_t)));
      /* write out the meta-block ids */
      for (size_t i = 0; i < num_meta_blocks; ++i) {
        stream << BlockVec[BlockVec.Size() - num_meta_blocks + i];
      }
      /* write out the sequential block pairings */
      static_assert(std::is_same<TBlockVec::TBlockMap::mapped_type, std::pair<size_t, size_t>>::value, "BlockVec mapped type must be the same as expected types for streaming block meta info for DataFile");
      for (const auto &iter : seq_block_map_copy) {
        stream << iter.second.first << iter.second.second;
      }
      /* write out the index offsets */
      for (const auto &index : index_map) {
        stream.Write(index.first);
        stream << index.second->ByteOffsetOfIndexMeta;
      }
      for (const auto &offset : MainArenaTypeBoundaryOffsetVec) {
        stream << offset;
      }

    }
    /* compute the total number of current keys */ {
      for (const auto &idx : index_map) {
        NumKeys += idx.second->NumCurKeys;
      }
    }
    completion_trigger.Wait();
    StartingBlockId = BlockVec[BlockVec.Size() - num_meta_blocks];
    StartingBlockOffset = BlockVec.Size() - num_meta_blocks;
    FileLength = num_blocks * Disk::Util::LogicalBlockSize;
    size_t total_num_keys = 0UL;
    for (const auto &index : index_map) {
      total_num_keys += index.second->NumCurKeys;
    }
    index_map.clear();
    /* sync this file to disk */ {
      std::vector<std::pair<size_t, size_t>> block_id_to_num_seq_blocks;
      /* compute the sequential block map */ {
        std::pair<size_t, size_t> seq_group = make_pair(BlockVec.Front(), 0UL);
        for (auto iter: BlockVec) {
          if (iter == (seq_group.first + seq_group.second)) {
            ++seq_group.second;
          } else {
            block_id_to_num_seq_blocks.push_back(seq_group);
            seq_group.first = iter;
            seq_group.second = 1UL;
          }
        }
        block_id_to_num_seq_blocks.push_back(seq_group);
      }
      Engine->GetVolMan()->SyncToDisk(block_id_to_num_seq_blocks);
    } /* done sync file to disk */
    /* wait for file entry to flush */ {
      Engine->InsertFile(file_uid, TFileObj::TKind::DataFile, gen_id, StartingBlockId, StartingBlockOffset, FileLength, total_num_keys, LowestSeq, HighestSeq, completion_trigger);
      completion_trigger.Wait();
    }
    for (TMemoryLayer::TUpdateCollection::TCursor csr(memory_layer->GetUpdateCollection()); csr; ++csr) {
      const auto &obj = csr->GetPersistenceNotification();
      if (obj) {
        obj->Call(TUpdate::TPersistenceNotification::Completed);
      }
    }
  } catch (const std::exception &err) {
    std::cout << "Exception: " << err.what() << std::endl;
    for (TMemoryLayer::TUpdateCollection::TCursor csr(memory_layer->GetUpdateCollection()); csr; ++csr) {
      const auto &obj = csr->GetPersistenceNotification();
      if (obj) {
        obj->Call(TUpdate::TPersistenceNotification::Failed);
      }
    }
    throw;
  }
  /* Let's make sure that all the blocks we have allocated were written to */
  #ifndef NDEBUG
  for (size_t block_id : BlockVec) {
    if (WrittenBlockSet.find(block_id) == WrittenBlockSet.end()) {
      syslog(LOG_ERR, "TDataFile allocated blocks that did not get written to [%ld]", block_id);
      throw std::logic_error("TDataFile allocated blocks that did not get written to");
    }
  }
  #endif
}

size_t MakeArena(Disk::Util::TEngine *engine,
                 Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                 DiskPriority priority,
                 TIndexFile::TOrderedNoteIndex &note_index,
                 TDataFile::TRemapIndex &remap_index,
                 TDataFile::TBlockVec &block_vec,
                 #ifndef NDEBUG
                 std::unordered_set<size_t> &written_block_set,
                 #endif
                 TDataFile::TTypeBoundaryOffsetVec &type_boundary_vec,
                 size_t max_total_note_bytes,
                 size_t &num_notes_out,
                 size_t &num_bytes_out) {
  assert(remap_index.empty());
  Atom::TCore::TArena *cur_arena = nullptr;
  auto remapper = [&remap_index, &cur_arena](TCore::TOffset off) {
    assert(cur_arena);
    auto pos = remap_index.find(TRemapObj(cur_arena, off, 0UL /* not used */));
    assert(pos != remap_index.end());
    return pos->NewKey;
  };

  TIndexFile::TOrderedNoteIndex::TCursor ordered_csr(&note_index, 32UL);
  const size_t arena_byte_offset = block_vec.Size() * Disk::Util::LogicalBlockSize;
  TCore::TOffset cur_disk_offset = 0UL;
  TCore::TOffset prev_disk_offset = cur_disk_offset;

  size_t max_blocks_required = ceil(static_cast<double>(max_total_note_bytes) / Disk::Util::LogicalBlockSize);

  engine->AppendReserveBlocks(storage_speed, max_blocks_required, block_vec);
  #ifndef NDEBUG
  /* TEMP DEBUG: validate that no one is looking at these cache pages */ {
    for (size_t i = block_vec.Size() - max_blocks_required; i < block_vec.Size(); ++i) {
      assert(engine->GetPageCache()->AssertNoRefCount(block_vec[i] * 16));
      assert(engine->GetBlockCache()->AssertNoRefCount(block_vec[i]));
    }
  }
  #endif

  TCompletionTrigger completion_trigger;
  unordered_map<size_t, shared_ptr<const TBufBlock>> arena_collision_map {};
  /* arena stream life time */ {
    TDataFile::TDataOutStream arena_stream(HERE,
                                           Source::DataFileArena,
                                           engine->GetVolMan(),
                                           arena_byte_offset,
                                           block_vec,
                                           arena_collision_map,
                                           completion_trigger,
                                           priority,
                                           true,
                                           #ifndef NDEBUG
                                           written_block_set,
                                           #endif
                                           [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
    type_boundary_vec.push_back(cur_disk_offset);

    void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
    void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
    TCore::TNote *temp_note = nullptr;
    size_t max_temp_note_size = getpagesize();
    temp_note = reinterpret_cast<TCore::TNote *>(malloc(max_temp_note_size));
    if (!temp_note) {
      throw std::bad_alloc();
    }
    try {
      while (ordered_csr) {
        /* insert the ordered notes first */ {
          Base::TOpt<TIndexFile::TOrderedNote> prev_note;
          for (; ordered_csr; ++ordered_csr) {
            const TIndexFile::TOrderedNote &ordered_note = *ordered_csr;
            const TCore::TNote *const note = ordered_note.Note;
            const size_t note_size = sizeof(TCore::TNote) + note->GetRawSize();
            TSuprena *const arena = reinterpret_cast<TSuprena *const>(ordered_note.Arena);
            if (!prev_note || ordered_note != *prev_note) {  // we haven't seen this one yet
              if (prev_note) {
                Atom::TCore
                  lhs_core(reinterpret_cast<Atom::TCore::TOffset>(prev_note->Note), prev_note->Note),
                  rhs_core(reinterpret_cast<Atom::TCore::TOffset>(ordered_note.Note), ordered_note.Note);
                Sabot::Type::TAny::TWrapper lhs_type_wrapper(lhs_core.GetType(prev_note->Arena, lhs_type_alloc));
                Sabot::Type::TAny::TWrapper rhs_type_wrapper(rhs_core.GetType(ordered_note.Arena, rhs_type_alloc));
                if (Atom::IsNe(Sabot::CompareTypes(*lhs_type_wrapper, *rhs_type_wrapper))) {
                  type_boundary_vec.push_back(cur_disk_offset);
                }
              }
              if (note_size > max_temp_note_size) {
                if ((temp_note = reinterpret_cast<TCore::TNote *>(realloc(temp_note, note_size))) == 0) {
                  free(temp_note);
                  temp_note = nullptr;
                  syslog(LOG_EMERG, "bad alloc in DataFile MakeArena note realloc [%ld]", note_size);
                  throw std::bad_alloc();
                }
                max_temp_note_size = note_size;
              }
              memcpy(temp_note, note, note_size);
              cur_arena = ordered_note.Arena;
              temp_note->Remap(remapper);
              arena_stream.Write(temp_note, note_size);
              prev_note = ordered_note;
              remap_index.emplace(arena, reinterpret_cast<TCore::TOffset>(note), cur_disk_offset);
              ++num_notes_out;
              prev_disk_offset = cur_disk_offset;
              cur_disk_offset += note_size;
            } else { // we've seen this one before
              remap_index.emplace(arena, reinterpret_cast<TCore::TOffset>(note), prev_disk_offset);
            }
          }
        }  // done inserting the ordered notes
      }
      free(temp_note);
    } catch (...) {
      free(temp_note);
      temp_note = nullptr;
      throw;
    }
  }  // done arena stream
  num_bytes_out = cur_disk_offset;
  /* wait for the arena to flush */ {
    completion_trigger.Wait();
  }
  /* now that we know exactly how many bytes we actually used, we can shrink the block vec to free the unused blocks. */
  const size_t end_of_stream = arena_byte_offset + num_bytes_out;
  const size_t actual_blocks_required = ((end_of_stream - 1UL) / Disk::Util::LogicalBlockSize) + 1UL;

  const size_t num_to_remove = block_vec.Size() - actual_blocks_required;
  block_vec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
    engine->FreeSeqBlocks(block_id, num_blocks);
    return true;
  }, actual_blocks_required, block_vec.Size());
  block_vec.Trim(num_to_remove);
  assert(block_vec.Size() == actual_blocks_required);

  return arena_byte_offset;
}