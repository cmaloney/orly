#pragma once

#include <exception>
#include <unordered_set>

#include <base/exception.h>
#include <base/json.h>
#include <base/split.h>
#include <base/thrower.h>

namespace Base {

template <typename TVal>
struct TJsonReader;

EXCEPTION(TInvalidValue, std::runtime_error, nullptr)

// TODO(cmaloney): Extract from r-value reference
// NOTE: name may contain '.' which specifies entry into sub key.
template <typename TVal>
TVal Extract(const Base::TJson &json, std::initializer_list<std::string> name={}) {
  try {
    return TJsonReader<TVal>::Read(json.Address(name));
  } catch(const TInvalidValue &ex) {
    THROWER(TInvalidValue) << "Invalid for \"" << Base::Join(name, '.') << "\". "
                               << ex.what();
  }
  // TODO: GCC BUG
  __builtin_unreachable();
}

template <typename TVal>
TVal ExtractOptional(const Base::TJson &json, std::initializer_list<std::string> name={}) {
  // Try reading a config value. If it exists, must be of the right type.
  const Base::TJson *entry = json.TryAddress(name);
  if (!entry) {
    return TVal();
  }
  try {
    return TJsonReader<TVal>::Read(*entry);
  } catch(const TInvalidValue &ex) {
    THROWER(TInvalidValue) << "Invalid for \"" << Base::Join(name, '.') << "\". "
                               << ex.what();
  }
  // TODO: GCC BUG
  __builtin_unreachable();
}

// Helper function
void ThrowIfWrongKind(const Base::TJson::TKind expected,
                      const Base::TJson &json,
                      const char *extra_msg = nullptr);

template <>
struct TJsonReader<bool> {
  static bool Read(const Base::TJson &entry);
};

template <>
struct TJsonReader<int64_t> {
  static int64_t Read(const Base::TJson &entry);
};

template <>
struct TJsonReader<std::string> {
  static std::string Read(const Base::TJson &entry);
};

template <typename TVal>
struct TJsonReader<std::vector<TVal>> {
  static std::vector<TVal> Read(const Base::TJson &entry) {
    ThrowIfWrongKind(Base::TJson::Array, entry);
    // Walk the array, pulling out each element
    std::vector<TVal> ret;
    ret.reserve(entry.GetSize());
    try {
      for (const Base::TJson &json: entry.GetArray()) {
        ret.push_back(TJsonReader<TVal>::Read(json));
      }
    } catch(const TInvalidValue &ex) {
      THROWER(TInvalidValue) << "Element in list. " << ex.what();
    }
    return ret;
  }
};

template <typename TVal>
struct TJsonReader<std::unordered_set<TVal>> {
  static std::unordered_set<TVal> Read(const Base::TJson &entry) {
    ThrowIfWrongKind(Base::TJson::Array, entry);
    // Walk the array, pulling out each element
    std::unordered_set<TVal> ret;
    ret.reserve(entry.GetSize());
    try {
      for (const Base::TJson &json: entry.GetArray()) {
        ret.insert(TJsonReader<TVal>::Read(json));
      }
    } catch(const TInvalidValue &ex) {
      THROWER(TInvalidValue) << "Element in list. " << ex.what();
    }
    return ret;
  }
};

template <typename TVal>
struct TJsonReader<std::map<std::string, TVal>> {
  static std::map<std::string, TVal> Read(const Base::TJson &entry) {
    ThrowIfWrongKind(Base::TJson::Object, entry);
    // Walk the array, pulling out each element
    std::map<std::string, TVal> ret;
    try {
      for (const auto &elem: entry.GetObject()) {
        ret.emplace(elem.first, TJsonReader<TVal>::Read(elem.second));
      }
    } catch(const TInvalidValue &ex) {
      THROWER(TInvalidValue) << "Element in object/map. " << ex.what();
    }
    return ret;
  }
};

} // namespace Base
