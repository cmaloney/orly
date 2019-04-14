/* Classes for Interning / when you need a single pointer for objects with the same key.

There are two varietes provided
1. Basic std::unordered_map backed interner, for non-thread based access
2. Fixed size lock-free (Although not wait free) map for high contention use cases.

TODO:
 - Expanding size
*/

#pragma once

#include <array>
#include <atomic>
#include <base/as_str.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>

namespace Base {

// Simple memory management by making one object (The registry) own all the pointers/memory.
template <typename TKey, typename TValue, typename Hash = std::hash<TKey>>
class TInterner {
  public:

    TValue *Add(TKey &&key, std::unique_ptr<TValue> &&item) {
    // TODO: Add a new InsertOrFail which can handle this
    auto result = ManagedThings.emplace(std::move(key), std::move(item));
    assert(result.second);

    return result.first->second.get();
  }

  TValue *TryGet(const TKey &key) {
    auto it = ManagedThings.find(key);
    if(it != ManagedThings.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  private:
  std::unordered_map<TKey, std::unique_ptr<TValue>, Hash> ManagedThings;
};

/* Thread-safe / concurrent safe interner. Get returns a a pointer or nullptr if no
   entry exists.

   Be certain _all_ access is done before deletion.

Implementation details:
 - Thread safety is done through atomic pointer swapping
 - Does a single hash then linear probing
*/
template <size_t N, typename TKey, typename TValue, typename Hash = std::hash<TKey>>
class TThreadSafeInterner {
public:
  TThreadSafeInterner(const TThreadSafeInterner &) = delete;
  TThreadSafeInterner(TThreadSafeInterner &&) = delete;

  TThreadSafeInterner() : warnedScan(false) {
    for(auto &elem: Storage) {
      elem.Value = nullptr;
      elem.Filled = false;
    }
  }
  ~TThreadSafeInterner() {
    // Empty out Storage
    for(auto &elem: Storage) {
      if (elem.Value) {
        delete elem.Value;
      }
    }
  }

  /* Add the given item to the interner. If an entry with the given key already exists
     use that, discard/free the new value and return the old value. This basically
     "ensures" the key has a value in the map.

     Setting a value happens first. Getting the value set indicates that thread has "won"
     the race for that slot, and all other contestants should spin until filled is set
     to indicate the key value is set. */
  TValue *Ensure(TKey &&key, std::unique_ptr<TValue> &&item) {
    // Keep track of linear probe depth. If it gets more than half the map size, print
    // a message that the map needs to be made significantly bigger.
    size_t scanLength = 0;

    TValue *newValue = item.get();

    // Cycle until a spot is found, linear probing from the initial hash target.
    for(auto index = Hash()(key) % N;; index = index + 1) {
      // Compete for the spot
      auto &slot = Storage[index];
      if (!slot.Value) {
        // Compete for the slot
        TValue* emptyValue = nullptr;
        if (slot.Value.compare_exchange_strong(emptyValue, newValue)) {
          // Won the set, fill the rest
          // NOTE: This needs to be as small as possible to keep spinlocking down.
          slot.Key = std::move(key);
          slot.Filled = true;
          // Take ownership from the unique_ptr as the interner now owns the item.
          item.release();
          return newValue;
        }

        // Lost the set, spinlock for it's key to be filled
        while(!slot.Filled) {}

        // Fall through to key checking the entry to see if we should take the newly
        // set value.
      }

      // Compare with the key to validate match
      if (slot.Key == key) {
        // Don't take ownership from the unique_ptr since the item needs to be deleted.
        return slot.Value;
      }

      scanLength += 1;
      if (scanLength > N / 2) {
        if (warnedScan.test_and_set(std::memory_order_acquire)) {
          std::cout << "WARNING: More than half of Interner static size walked trying to place value. Files map constant size should be expanded\n";
        }
      }
    }
  }

  // TODO(cmaloney): Could further optimize later setting by returning the last attempted key in the TryGet
  // and using that as the later start point for setting on miss.
  TValue *TryGet(const TKey &key) {
    for (auto index = Hash()(key) % N; true; ++index) {
      auto &slot = Storage[index];
      if (!slot.Filled) {
        return nullptr;
      }
      if (slot.Key == key) {
        return slot.Value;
      }

    }
  }


private:
  struct TEntry {
    // TODO(cmaloney): Make the key a pointer, have value pointer being set imply filled.
    TKey Key;
    std::atomic<TValue*> Value;
    std::atomic<bool> Filled;

    static_assert(std::atomic<TValue*>::is_always_lock_free, "The value atomic should always be lock free for speed.");
  };

  std::array<TEntry, N> Storage;
  std::atomic_flag warnedScan;
};

} // Base
