#pragma once

#include <cassert>
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

}
