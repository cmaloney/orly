/* <stig/indy/disk/present_walk_file.h>

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

#include <algorithm>
#include <cassert>

#include <base/no_copy_semantics.h>
#include <stig/indy/disk/read_file.h>
#include <stig/indy/present_walker.h>
#include <stig/sabot/all.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TWalkerKey {
        public:

        /* TODO */
        TWalkerKey(const Base::TUuid &file_id, size_t gen_id, const Base::TUuid &index_id) : FileId(file_id), GenId(gen_id), IndexId(index_id) {}

        /* TODO */
        TWalkerKey(Base::TUuid &&file_id, size_t gen_id, Base::TUuid &&index_id) : FileId(std::move(file_id)), GenId(gen_id), IndexId(std::move(index_id)) {}

        /* TODO */
        Base::TUuid FileId;

        /* TODO */
        size_t GenId;

        /* TODO */
        Base::TUuid IndexId;

        /* TODO */
        inline size_t GetHash() const {
          assert(this);
          return GenId ^ FileId.GetHash()^ IndexId.GetHash();
        }

        /* TODO */
        inline bool operator==(const TWalkerKey &that) const {
          assert(this);
          assert(&that);
          return FileId == that.FileId && GenId == that.GenId && IndexId == that.IndexId;
        }

        /* TODO */
        inline bool operator!=(const TWalkerKey &that) const {
          assert(this);
          assert(&that);
          return FileId != that.FileId || GenId != that.GenId || IndexId != that.IndexId;
        }

      };  // TWalkerKey

    }  // Disk

  }  // Indy

}  // Stig

namespace std {

  /* A standard hasher for Stig::Indy::Disk::TWalkerKey. */
  template <>
  struct hash<Stig::Indy::Disk::TWalkerKey> {
    typedef size_t result_type;
    typedef Stig::Indy::Disk::TWalkerKey argument_type;
    size_t operator()(const Stig::Indy::Disk::TWalkerKey &that) const {
      return that.GetHash();
    }
  };

}  // std

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TLocalWalkerCache {
        NO_COPY_SEMANTICS(TLocalWalkerCache);
        public:

        /* TODO */
        TLocalWalkerCache() : LoaderCollection(this) {}

        /* Forward Declaration */
        class TLoaderObj;

        /* TODO */
        class TPresentWalkFile
            : public TPresentWalker {
          NO_COPY_SEMANTICS(TPresentWalkFile);
          public:

          /* TODO */
          static constexpr size_t PhysicalCachePageSize = Util::PhysicalBlockSize / (Util::LogicalBlockSize / Util::LogicalPageSize);

          /* TODO */
          using TArena = TDiskArena<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage, DiskArenaMaxCacheSize, true>;

          /* TODO */
          using TInStream = TStream<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage, 0UL>;
          using TMyReadFile = Stig::Indy::Disk::TReadFile<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage>;

          TPresentWalkFile(Util::TEngine *engine,
                           const Base::TUuid &file_id,
                           size_t gen_id,
                           const Base::TUuid &index_id,
                           TLoaderObj *loader_obj)
              : Stig::Indy::TPresentWalker(Match),
                IndexId(index_id),
                MyReadFile(TLocalReadFileCache<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage>::Cache->Get(engine, file_id, gen_id)),
                IndexFile(nullptr),
                MainArena(MyReadFile, engine->GetCache<PhysicalCachePageSize>(), RealTime),
                Stream(HERE, Source::PresentWalk, RealTime, MyReadFile, engine->GetPageCache(), 0),
                IndexStream(HERE, Source::PresentWalk, RealTime, MyReadFile, engine->GetPageCache(), 0),
                Valid(true), Cached(false), LoaderObj(loader_obj), NextWalker(nullptr) {
            auto pos = MyReadFile->GetIndexByIdMap().find(IndexId);
            if (pos != MyReadFile->GetIndexByIdMap().end()) {
              IndexFile = pos->second.get();
            }
            /* The unique ptr to index_file is a temporary solution. */
            if (IndexFile) {
              IndexArena = std::unique_ptr<TArena>(new TArena(IndexFile, engine->GetCache<PhysicalCachePageSize>(), RealTime));
              Item.KeyArena = IndexArena.get();
              Item.OpArena = &MainArena;
            }
          }

          /* TODO */
          struct TDeleter {

            /* TODO */
            void operator ()(TPresentWalkFile *p) const {
              p->LoaderObj->Free(p);
            };

          };  // TDeleter

          /* Look for this specific key. If it's free we'll return the range. This will never fall back to a binary search. If we don't have the prefix
             hash for your key, then there's no way we'll find it in a binary search. */
          void Init(const TKey &from) {
            SearchKind = Match;
            if (IndexFile) {
              From = from;
              Cached = false;
              if ((Valid = IndexFile->FindInHash(From, Offset, IndexStream, IndexArena.get()))) {
                Stream.GoTo(Offset);
              }
              if (Valid) {
                Refresh();
              }
            } else {
              Valid = false;
              Cached = true;
            }
          }

          /* Look for your key range. This can fall back to a binary search. This means we're NOT free. We've been given 2 fully defined keys. In case
             where the start of the range exists as a key in this particular file, we'll be able to hash to it and iterate from there. If it does not
             exist, we'll find the first key larger than the given start key using a binary search and iterate from there. */
          void Init(const TKey &from, const TKey &to) {
            SearchKind = Range;
            if (IndexFile) {
              From = from;
              To = to;
              Cached = false;
              size_t offset = 0U;
              bool success = IndexFile->FindInHash(From, Offset, IndexStream, IndexArena.get());
              if (success) {
                Valid = true;
                Stream.GoTo(Offset);
              } else if (From != To && IndexFile->BinaryLowerBoundOnKey(From, offset, IndexStream, IndexArena.get())) {
                Stream.GoTo(offset);
                Valid = true;
              }
              if (Valid) {
                Refresh();
              }
            } else {
              Valid = false;
              Cached = true;
            }
          }

          /* TODO */
          virtual ~TPresentWalkFile() {}

          /* True iff. we have an item. */
          inline virtual operator bool() const {
            assert(this);
            //Refresh();
            return Valid;
          }

          /* The current item. */
          inline virtual const TItem &operator*() const {
            assert(this);
            //Refresh();
            assert(Cached);
            assert(Valid);
            //std::cout << "Returning [" << Indy::TKey(Item.Key, Item.KeyArena) << "] = [" << Indy::TKey(Item.Op, Item.OpArena) << "]" << std::endl;
            return Item;
          }

          /* Walk to the next item, if any. */
          inline virtual TPresentWalker &operator++() {
            assert(this);
            Cached = false;
            Refresh();
            return *this;
          }

          private:

          /* TODO */
          void Refresh() const {
            assert(this);
            assert (!Cached);
            assert(Valid);
            #ifndef NDEBUG
            void *key_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
            void *search_state_alloc = reinterpret_cast<uint8_t *>(key_state_alloc) + Sabot::State::GetMaxStateSize();
            #endif
            for (;Stream.GetOffset() < IndexFile->GetByteOffsetOfHistoryIndex();) {
              assert(Valid);
              Stream.Read(&Item.SequenceNumber, sizeof(TSequenceNumber) + sizeof(Atom::TCore) * 2);
              Stream.Skip(sizeof(size_t) * 2U);
              Cached = true;
              switch (SearchKind) {
                case Match: {
                  #ifndef NDEBUG
                  Sabot::State::TAny::TWrapper key_state(From.GetCore().NewState(From.GetArena(), key_state_alloc));
                  Sabot::State::TAny::TWrapper cur_state(Item.Key.NewState(Item.KeyArena, search_state_alloc));
                  #endif
                  Sabot::TMatchResult res = From.GetCore().PrefixMatch(From.GetArena(), Item.Key, Item.KeyArena);
                  assert(res == MatchPrefixState(*key_state, *cur_state));
                  switch (res) {
                    case Sabot::TMatchResult::Unifies: {
                      /* we found a match. */
                      return;
                      break;
                    }
                    case Sabot::TMatchResult::NoMatch: {
                      /* we're past our specific value, or in the case of a free the possible range of entries that we can unify with */
                      Valid = false;
                      return;
                      break;
                    }
                    case Sabot::TMatchResult::PrefixMatch: {
                      /* keep iterating till we find a unify. */
                      if (TKey(From.GetCore(), From.GetArena()) < TKey(Item.Key, Item.KeyArena)) {
                        Valid = false;
                        return;
                      }
                      break;
                    }
                  }
                  break;
                }
                case Range: {
                  if (TKey(Item.Key, Item.KeyArena) > To) {
                    /* we're past the end of the range. */
                    Valid = false;
                    return;
                  } else {
                    /* we're still in the valid range. */
                    return;
                  }
                  break;
                }
              }
            }
            Valid = false;
          }

          /* TODO */
          Base::TUuid IndexId;

          /* TODO */
          TMyReadFile *MyReadFile;

          /* TODO */
          typename TMyReadFile::TIndexFile *IndexFile;

          /* TODO */
          TArena MainArena;
          std::unique_ptr<TArena> IndexArena;

          /* TODO */
          TKey From;

          /* TODO */
          TKey To;

          /* TODO */
          mutable TInStream Stream;

          /* TODO */
          mutable TInStream IndexStream;

          /* TODO */
          mutable bool Valid;

          /* TODO */
          mutable bool Cached;

          /* TODO */
          mutable TItem Item;

          /* TODO */
          mutable size_t Offset;

          /* TODO */
          TLoaderObj *LoaderObj;

          /* TODO */
          TPresentWalkFile *NextWalker;

          friend class TLoaderObj;
          friend class TPresentWalkFileWrapper;

        };  // TPresentWalkFile

        /* TODO */
        TPresentWalkFile *Get(Util::TEngine *engine,
                              const Base::TUuid &file_id,
                              size_t gen_id,
                              const Base::TUuid &index_id) {
          assert(this);
          TLoaderObj *loader = LoaderCollection.TryGetFirstMember(TWalkerKey(file_id, gen_id, index_id));
          if (!loader) {
            loader = new TLoaderObj(this, file_id, gen_id, index_id);
          }
          return loader->GetNewFile(engine, file_id, gen_id, index_id);
        }

        /* TODO */
        void Clear(const Base::TUuid &file_id, size_t gen_id, const Base::TUuid &index_id) {
          assert(this);
          TLoaderObj *loader = LoaderCollection.TryGetFirstMember(TWalkerKey(file_id, gen_id, index_id));
          if (loader) {
            delete loader;
          }
        }

        /* TODO */
        static __thread TLocalWalkerCache *Cache;

        /* TODO */
        class TLoaderObj {
          NO_COPY_SEMANTICS(TLoaderObj);
          public:

          /* TODO */
          typedef InvCon::UnorderedMultimap::TMembership<TLoaderObj, TLocalWalkerCache, TWalkerKey> TCacheMembership;

          /* TODO */
          TLoaderObj(TLocalWalkerCache *cache, const Base::TUuid &file_id, size_t gen_id, const Base::TUuid &index_id)
              : FreeQueue(nullptr),
                CacheMembership(this, TWalkerKey(file_id, gen_id, index_id), &cache->LoaderCollection) {}

          /* TODO */
          ~TLoaderObj() {
            assert(this);
            for (TPresentWalkFile *file = FreeQueue; file;) {
              TPresentWalkFile *to_delete = file;
              file = file->NextWalker;
              delete to_delete;
            }
          }

          /* TODO */
          TPresentWalkFile *GetNewFile(Util::TEngine *engine, const Base::TUuid &file_id, size_t gen_id, const Base::TUuid &index_id) {
            assert(this);
            if (!FreeQueue) {
              return new TPresentWalkFile(engine, file_id, gen_id, index_id, this);
            } else {
              TPresentWalkFile *ret = FreeQueue;
              FreeQueue = ret->NextWalker;
              return ret;
            }
          }

          /* TODO */
          void Free(TPresentWalkFile *walker) {
            assert(this);
            walker->NextWalker = FreeQueue;
            FreeQueue = walker;
          }

          private:

          /* TODO */
          TPresentWalkFile *FreeQueue;

          /* TODO */
          typename TCacheMembership::TImpl CacheMembership;

        };  // TLoaderObj

        private:

        /* TODO */
        typedef InvCon::UnorderedMultimap::TCollection<TLocalWalkerCache, TLoaderObj, TWalkerKey> TLoaderCollection;

        /* TODO */
        mutable typename TLoaderCollection::TImpl LoaderCollection;

      };  // TLocalWalkerCache

      /* TODO */
      class TPresentWalkFileWrapper
          : public TPresentWalker {
        NO_COPY_SEMANTICS(TPresentWalkFileWrapper);
        public:

        /* TODO */
        TPresentWalkFileWrapper(Util::TEngine *engine,
                                const Base::TUuid &file_id,
                                size_t gen_id,
                                const Base::TUuid &index_id,
                                const TKey &from,
                                const TKey &to)
            : TPresentWalker(Match),
          File(TLocalWalkerCache::Cache->Get(engine, file_id, gen_id, index_id)) {
          File->Init(from, to);
        }

        /* TODO */
        TPresentWalkFileWrapper(Util::TEngine *engine,
                                const Base::TUuid &file_id,
                                size_t gen_id,
                                const Base::TUuid &index_id,
                                const TKey &key)
            : TPresentWalker(Match),
          File(TLocalWalkerCache::Cache->Get(engine, file_id, gen_id, index_id)) {
          File->Init(key);
        }

        /* TODO */
        virtual ~TPresentWalkFileWrapper() {
          assert(this);
          assert(File);
          assert(File->LoaderObj);
          File->LoaderObj->Free(File);
        }

        /* True iff. we have an item. */
        inline virtual operator bool() const {
          assert(this);
          return static_cast<bool>(*File);
        }

        /* The current item. */
        inline virtual const TItem &operator*() const {
          assert(this);
          return **File;
        }

        /* Walk to the next item, if any. */
        inline virtual TPresentWalker &operator++() {
          assert(this);
          ++(*File);
          return *this;
        }

        private:

        /* TODO */
        TLocalWalkerCache::TPresentWalkFile *File;

      };  // TPresentWalkFileWrapper

    }  // Disk

  }  // Indy

}  // Stig