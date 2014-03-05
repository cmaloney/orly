/* <stig/indy/memory_layer.test.cc>

   Unit test for <stig/indy/memory_layer.h>.

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

#include <stig/indy/memory_layer.h>
#include <stig/indy/update.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;


Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Stig::Indy::Util::TPool L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer");

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 4000004UL);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 4000004UL);

template <typename ...TArgs>
void Insert(TMemoryLayer &mem_layer, TSequenceNumber seq_num, const Base::TUuid &idx_id, int64_t val, const TArgs &...args) {
  Atom::TSuprena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  std::shared_ptr<TUpdate> update(TUpdate::NewUpdate(TUpdate::TOpByKey{
    { TIndexKey(idx_id, TKey(std::make_tuple(args...), &arena, state_alloc)), TKey(val, &arena, state_alloc)}
    }, TKey(&arena), TKey(Base::TUuid(Base::TUuid::Best), &arena, state_alloc)));
  update->SetSequenceNumber(seq_num);
  mem_layer.Insert(TUpdate::CopyUpdate(update.get(), state_alloc));
}

FIXTURE(Typical) {
  TMemoryLayer mem_layer(nullptr);
  Base::TUuid int_idx(Base::TUuid::Twister);
  TSuprena arena;
  TSequenceNumber seq_num = 0UL;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  /* insert data */ {
    for (int64_t i = 0; i < 11; i += 2) {
      Insert(mem_layer, ++seq_num, int_idx, static_cast<int64_t>(rand()),
               i);
    }
  }
  /* basic walk */ {
    for (int64_t i = 0; i < 11; i += 2) {
      auto walker_ptr = mem_layer.NewPresentWalker(TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(walker)) {
        const TPresentWalker::TItem &item = *walker;
        EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(i), &arena, state_alloc));
      }
    }
  }
  /* non-exist walk */ {
    for (int64_t i = 1; i < 11; i += 2) {
      auto walker_ptr = mem_layer.NewPresentWalker(TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      EXPECT_FALSE(walker);
    }
  }
}

FIXTURE(Free) {
  TMemoryLayer mem_layer(nullptr);
  Base::TUuid int_int_idx(Base::TUuid::Twister);
  TSuprena arena;
  TSequenceNumber seq_num = 0UL;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  /* insert data */ {
      Insert(mem_layer, ++seq_num, int_int_idx, static_cast<int64_t>(rand()),   1L, 7L);
      Insert(mem_layer, ++seq_num, int_int_idx, static_cast<int64_t>(rand()),   1L, 14L);
      Insert(mem_layer, ++seq_num, int_int_idx, static_cast<int64_t>(rand()),   3L, 21L);
      Insert(mem_layer, ++seq_num, int_int_idx, static_cast<int64_t>(rand()),   3L, 28L);
      Insert(mem_layer, ++seq_num, int_int_idx, static_cast<int64_t>(rand()),   8L, 35L);
  }
  /* basic walk */ {
    /* <[1L , 7L]> */ {
      auto walker_ptr = mem_layer.NewPresentWalker(TIndexKey(int_int_idx, TKey(make_tuple(1L, 7L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(walker)) {
        const TPresentWalker::TItem &item = *walker;
        EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(1L, 7L), &arena, state_alloc));
      }
    }
    /* <[1L , 14L]> */ {
      auto walker_ptr = mem_layer.NewPresentWalker(TIndexKey(int_int_idx, TKey(make_tuple(1L, 14L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(walker)) {
        const TPresentWalker::TItem &item = *walker;
        EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(1L, 14L), &arena, state_alloc));
      }
    }
    /* <[3L , 21L]> */ {
      auto walker_ptr = mem_layer.NewPresentWalker(TIndexKey(int_int_idx, TKey(make_tuple(3L, 21L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(walker)) {
        const TPresentWalker::TItem &item = *walker;
        EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(3L, 21L), &arena, state_alloc));
      }
    }
  }
  /* free walk */ {
    /* <[1L , free(int64_t)]> */ {
      TIndexKey search_key(int_int_idx, TKey(make_tuple(1L, Native::TFree<int64_t>()), &arena, state_alloc));
      auto walker_ptr = mem_layer.NewPresentWalker(search_key);
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(walker)) {
        const TPresentWalker::TItem &item = *walker;
        EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(1L, 7L), &arena, state_alloc));
        ++walker;
        if (EXPECT_TRUE(walker)) {
          const TPresentWalker::TItem &item = *walker;
          EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(1L, 14L), &arena, state_alloc));
          ++walker;
          EXPECT_FALSE(walker);
        }
      }
    }
    /* <[3L , free(int64_t)]> */ {
      TIndexKey search_key(int_int_idx, TKey(make_tuple(3L, Native::TFree<int64_t>()), &arena, state_alloc));
      auto walker_ptr = mem_layer.NewPresentWalker(search_key);
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(walker)) {
        const TPresentWalker::TItem &item = *walker;
        EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(3L, 21L), &arena, state_alloc));
        ++walker;
        if (EXPECT_TRUE(walker)) {
          const TPresentWalker::TItem &item = *walker;
          EXPECT_EQ(TKey(item.Key, item.KeyArena), TKey(make_tuple(3L, 28L), &arena, state_alloc));
          ++walker;
          EXPECT_FALSE(walker);
        }
      }
    }
  }
}

#if 0
FIXTURE(Range) {
  TMemoryLayer layer(nullptr);
  int64_t num_iter = 10L;
  TSequenceNumber seq_num = 1UL;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  for (int64_t i = 0; i < num_iter; ++i) {
    TSuprena arena;
    auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TKey(make_tuple(num_iter - 1L - i), &arena, state_alloc), TKey((num_iter - 1L - i) * 10L, &arena, state_alloc)} }, TKey(), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc));
    TUpdate *my_update = TUpdate::CopyUpdate(update.get());
    my_update->SetSequenceNumber(seq_num++);
    layer.Insert(my_update);
  }
  TSuprena arena;
  /* check that all the updates are there */ {
    auto walker_ptr = layer.NewPresentWalker(TKey(make_tuple(0L), &arena, state_alloc), TKey(make_tuple(num_iter), &arena, state_alloc));
    int64_t count = 0L;
    for (auto &walker = *walker_ptr; walker; ++walker) {
      EXPECT_EQ((*walker).SequenceNumber, static_cast<size_t>(num_iter) - count);
      EXPECT_EQ(TKey((*walker).Key, (*walker).Arena), TKey(make_tuple(count), &arena, state_alloc));
      EXPECT_EQ(TKey((*walker).Op, (*walker).Arena), TKey(count * 10L, &arena, state_alloc));
      ++count;
    }
    EXPECT_EQ(count, num_iter);
  }
}
#endif