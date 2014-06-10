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

#include <base/opt.h>
#include <base/json.h>

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
    public:

    DEFINE_ERROR(TInvalidValue, std::runtime_error, "invalid config value");

    TConfig(Base::TJson base_config);

    template<typename TVal>
    static TVal ReadJson(const std::string &name, const Base::TJson &entry);

    // TODO: All the partial  specializations
    //NOTE: name may contain '.' which specifies entry into sub key.
    template <typename TVal>
    TVal Read(const std::string &name) const {
      auto entry = GetEntry(name);
      return TJsonReader<TVal>::Read(name, entry);
    }

    template <typename TVal>
    bool TryRead(const std::string &name, TVal &out) const {
      // Try reading a config value. If it exists, must be of the right type.
      Base::TJson entry;
      if(!TryGetEntry(name, entry)) {
        return false;
      }
      out = TJsonReader<TVal>::Read(name, entry);
      return true;
    }

    void AddComputed(Base::TJson &&config);
    bool TryGetEntry(const std::string &name, Base::TJson &out) const;
    Base::TJson GetEntry(const std::string &name) const;

    /* Write out the computed configuration instructions */
    void WriteComputed(std::ostream &out) const;

    /* Read in computed configuration from the given file */
    void LoadComputed(const std::string &in);

    const std::vector<Base::TJson> GetComputed() const;
    void SetComputed(std::vector<Base::TJson> &&conf);

    private:
    mutable bool ConfigLocked = false; // This is purely an internal safety check / correctness check
    std::deque<Base::TJson> ConfigStack;
  };

  template <>
  struct TJsonReader<bool> {
    static bool Read(const std::string &name, const Base::TJson &entry) {
      if (entry.GetKind() != Base::TJson::Bool) {
        THROW_ERROR(TConfig::TInvalidValue) << "Invalid value for key " << std::quoted(name)
                                            << ". Expected a number (double)";
      }
      return entry.GetBool();
    }
  };

  template <>
  struct TJsonReader<double> {
    static double Read(const std::string &name, const Base::TJson &entry) {
      if (entry.GetKind() != Base::TJson::Number) {
        THROW_ERROR(TConfig::TInvalidValue) << "Invalid value for key " << std::quoted(name)
                                            << ". Expected a number (double)";
      }
      return entry.GetNumber();
    }
  };

  template <>
  struct TJsonReader<std::string> {
    static std::string Read(const std::string &name, const Base::TJson &entry) {
      if (entry.GetKind() != Base::TJson::String) {
        THROW_ERROR(TConfig::TInvalidValue) << "Invalid value for key " << std::quoted(name) << ". Expected a string";
      }
      return entry.GetString();
    }
  };

  template <typename TVal>
  struct TJsonReader<std::vector<TVal>> {
    static std::vector<TVal> Read(const std::string &name, const Base::TJson &entry) {
      if (entry.GetKind() != Base::TJson::Array) {
        THROW_ERROR(TConfig::TInvalidValue) << "Invalid value for key " << std::quoted(name) << ". Expected an array";
      }

      // Walk the array, pulling out each element
      std::vector<TVal> ret;
      ret.reserve(entry.GetSize());
      std::string elem_name = name + "[]";
      entry.ForEachElem([&ret, &elem_name](const Base::TJson &json) -> bool {
        ret.push_back(TJsonReader<TVal>::Read(elem_name, json));
        return true;
      });
      return ret;
    }
  };


  template <typename TVal>
  struct TJsonReader<std::map<std::string, TVal>> {
    static std::map<std::string, TVal> Read(const std::string &name, const Base::TJson &entry) {
      if (entry.GetKind() != Base::TJson::Object) {
        THROW_ERROR(TConfig::TInvalidValue) << "Invalid value for key " << std::quoted(name) << ". Expected a JSON object";
      }

      // Walk the array, pulling out each element
      std::map<std::string, TVal> ret;
      std::string elem_name = name + "[]";
      entry.ForEachElem([&ret, &elem_name](const std::string &key, const Base::TJson &json) -> bool {
        ret.emplace(key, TJsonReader<TVal>::Read(elem_name, json));
        return true;
      });
      return ret;
    }
  };
}