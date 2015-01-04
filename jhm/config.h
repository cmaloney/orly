/* <jhm/config.h>

   Configuration file loading / reading.

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <deque>

#include <base/class_traits.h>
#include <base/json.h>
#include <base/opt.h>
#include <base/split.h>
#include <util/time.h>

namespace Jhm {
  /* Configuration system. Handles stacked configuration files ('parent', 'include' statements)

     Wraps <base/json.h> as it's core config file format.

     Key prefixed with '+' means add/append '-' means remove, '=' means replace. Default behavior is '+'.

     Allows configs to enter the stack either through explicitly including eachother (Useful for platform includes), by
     being handwritten for a file which is an input or need of a job (Useful for bad headers with c style casts and the
     like), or by having a job which is known to need special flags to process it's output (Useful for flex/bison and
     the horrible C++ they output)

     NOTE: Config cannot be machine generated __EVER__. Attached/used from producer and the like yes. But never a
     generated file.

     TODO: parent/include statements? */
  template<typename TVal>
  struct TJsonReader;

  // Combines multiple configs into one coherent config
  // Also resolves the config delta notation.
  class TConfig final {
    NO_COPY(TConfig);
    NO_MOVE(TConfig);
    public:

    DEFINE_ERROR(TInvalidValue, std::runtime_error, nullptr);
    DEFINE_ERROR(TNotFound, std::runtime_error, nullptr);
    DEFINE_ERROR(TInvalidConfig, std::runtime_error, nullptr);

    explicit TConfig();
    TConfig(const std::string &filename);
    TConfig(const std::vector<std::string> &files);

    bool HasConfig() const {
      return !Config.empty();
    }

    //NOTE: name may contain '.' which specifies entry into sub key.
    template <typename TVal>
    TVal Read(const std::initializer_list<std::string> &name) const {
      auto entry = GetEntry(name);
      try {
        return TJsonReader<TVal>::Read(entry);
      } catch (const TInvalidValue &ex) {
        THROW_ERROR(TInvalidValue) << "Invalid for \"" << Base::Join(name, '.') << "\". " << ex.what();
      }
      // TODO: GCC BUG
      __builtin_unreachable();
    }

    template <typename TVal>
    bool TryRead(const std::initializer_list<std::string> &name, TVal &out) const {
      // Try reading a config value. If it exists, must be of the right type.
      Base::TJson entry;
      if(!TryGetEntry(name, entry)) {
        return false;
      }
      out = TJsonReader<TVal>::Read(entry);
      return true;
    }

    bool TryGetEntry(const std::initializer_list<std::string> &name, Base::TJson &out) const;
    Base::TJson GetEntry(const std::initializer_list<std::string> &name) const;

    /* Add a config that has been computed. */
    void AddComputed(Base::TJson &&config);

    /* Call back the given function for each computed config (in order). */
    bool ForEachComputed(const std::function<bool (const Base::TJson &conf)> &cb) const;

    const Util::TOptTimestamp &GetTimestamp() const {
      assert(this);
      return Timestamp;
    }

    /* Write out the computed configuration instructions */
    void WriteComputed(std::ostream &out) const;

    /* Read in computed configuration from the given file */
    void LoadComputed(const std::string &in);

    const std::vector<Base::TJson> GetComputed() const;
    void SetComputed(std::vector<Base::TJson> &&conf);

    private:
    void AddConfig(Base::TJson &&config, bool top=true);
    void AddFile(const std::string &filename);

    Util::TOptTimestamp Timestamp;
    Base::TOpt<uint32_t> ComputedStart;
    mutable bool ComputedLocked = false; // This is purely an internal safety check / correctness check
    std::deque<Base::TJson> Config;
  };

  // Helper function
  inline void ThrowIfWrongKind(const Base::TJson::TKind expected, const Base::TJson &json) {
    if (json.GetKind() != expected) {
      THROW_ERROR(TConfig::TInvalidValue) << "Expected a " << expected << "but found a " << json.GetKind();
    }
  }

  template <>
  struct TJsonReader<bool> {
    static bool Read(const Base::TJson &entry) {
      ThrowIfWrongKind(Base::TJson::Bool, entry);
      return entry.GetBool();
    }
  };

  template <>
  struct TJsonReader<uint64_t> {
    static uint64_t Read(const Base::TJson &entry) {
      ThrowIfWrongKind(Base::TJson::Number, entry);
      return entry.GetNumber();
    }
  };

  template <>
  struct TJsonReader<std::string> {
    static std::string Read(const Base::TJson &entry) {
      ThrowIfWrongKind(Base::TJson::String, entry);
      return entry.GetString();
    }
  };

  template <typename TVal>
  struct TJsonReader<std::vector<TVal>> {
    static std::vector<TVal> Read(const Base::TJson &entry) {
      ThrowIfWrongKind(Base::TJson::Array, entry);
      // Walk the array, pulling out each element
      std::vector<TVal> ret;
      ret.reserve(entry.GetSize());
      try {
        entry.ForEachElem([&ret](const Base::TJson &json) -> bool {
          ret.push_back(TJsonReader<TVal>::Read(json));
          return true;
        });
      } catch (const TConfig::TInvalidValue &ex) {
        THROW_ERROR(TConfig::TInvalidValue) << "Element in list. " << ex.what();
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
        entry.ForEachElem([&ret](const std::string &key, const Base::TJson &json) -> bool {
          ret.emplace(key, TJsonReader<TVal>::Read(json));
          return true;
        });
      } catch (const TConfig::TInvalidValue &ex) {
        THROW_ERROR(TConfig::TInvalidValue) << "Element in object/map. " << ex.what();
      }
      return ret;
    }
  };
}
