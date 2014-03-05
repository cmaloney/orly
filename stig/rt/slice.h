/* <stig/rt/slice.h>

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

#include <vector>

#include <stig/rt/mutable.h>
#include <stig/var.h>

namespace Stig {

  namespace Rt {

    /* TODO */
    template <typename TVal>
    static TVal SliceSingle(const std::vector<TVal> &container, const int64_t &idx) {
      if (idx < 0 || idx >= static_cast<int64_t>(container.size())) {
        throw TSystemError(HERE, "Slice index out of bounds.");
      }
      return container[idx];
    }

    inline std::string SliceSingle(const std::string &container, const int64_t &idx) {
      if (idx < 0 || idx >= static_cast<int64_t>(container.size())) {
        throw TSystemError(HERE, "Slice index out of bounds.");
      }
      return std::string(container.substr(idx, 1));
    }

    /* TODO */
    template <typename TKey, typename TVal>
    static TVal SliceSingle(const TDict<TKey, TVal> &container, const TKey &idx) {
      auto pos = container.find(idx);
      if (pos == container.end()) {
        throw TSystemError(HERE, "No value in dict by this key.");
      }
      return pos->second;
    }

    template<typename TAddr, typename TVal>
    TMutable<TAddr, TVal> SliceSingle(const TMutable<TAddr, std::vector<TVal>> &container, const int64_t &idx) {
      return RewrapMutable(container, Var::TVar(idx), SliceSingle(container.GetVal(), idx));
    }

    template<typename TAddr, typename TKey, typename TVal, typename TIdx>
    TMutable<TAddr, TVal> SliceSingle(
          const TMutable<TAddr, TDict<TKey, TVal>> &container,
          const TIdx &idx) {
      return RewrapMutable(container, Var::TVar(idx), SliceSingle(container.GetVal(), idx));
    }

    /* TODO */
    template <typename TVal>
    std::vector<TVal> SliceRange(const std::vector<TVal> &container, bool start_range, const int64_t &idx) {
      if (idx < 0 || idx > static_cast<int>(container.size())) {
        throw TSystemError(HERE, "Slice index out of bounds.");
      }
      return start_range ? std::vector<TVal>(container.begin(), container.begin() + idx)
                         : std::vector<TVal>(container.begin() + idx, container.end());
    }

    /* TODO */
    inline std::string SliceRange(const std::string &container, bool start_range, const int64_t &idx) {
      if (idx < 0 || idx > static_cast<int>(container.size())) {
        throw TSystemError(HERE, "Slice index out of bounds.");
      }
      return start_range ? std::string(container.substr(0, idx))
                         : std::string(container.substr(idx));
    }

    /* TODO: SliceRange for a mutable
    template <typename TAddr, typename TVal, typename TIdx>
    TMutable<TAddr, std::vector<TVal>> SliceRange(
          const TMutable<TAddr, std::vector<TVal>> &container, bool start_range, const TIdx &idx) {
      return TMutable<TAddr, std::vector<TVal>>(container.GetOptAddr(),
          SliceRange(container.GetVal(), start_range, idx));
    } */

    /* TODO */
    template <typename TVal>
    std::vector<TVal> SliceRangeBoth(const std::vector<TVal> &container, const int64_t &lhs, const int64_t &rhs) {
      if (lhs < 0 || lhs > static_cast<int>(container.size()) ||
          rhs < 0 || rhs > static_cast<int>(container.size())) {
        throw TSystemError(HERE, "Slice index out of bounds.");
      }
      if (lhs > rhs) {
        throw TSystemError(HERE, "Reverse slices are not supported.");
      }
      return std::vector<TVal>(container.begin() + lhs, container.begin() + rhs);
    }

    /* Inline because it didn't like having both functions in here */
    inline std::string SliceRangeBoth(const std::string &container, const int64_t &lhs, const int64_t &rhs) {
      if (lhs < 0 || lhs > static_cast<int>(container.size()) ||
          rhs < 0 || rhs > static_cast<int>(container.size())) {
        throw TSystemError(HERE, "Slice index out of bounds.");
      }
      if (lhs > rhs) {
        throw TSystemError(HERE, "Reverse slices are not supported.");
      }
      return std::string(container.substr(lhs, rhs - lhs));
    }

    /* TODO
    template <typename TAddr, typename TVal, typename TIdx>
    TMutable<TAddr, std::vector<TVal>> SliceRangeBoth(
          const TMutable<TAddr, std::vector<TVal>> &container, const TIdx &lhs, const TIdx &rhs) {
      return TMutable<TAddr, std::vector<TVal>>(container.GetOptAddr(),
          SliceRangeBoth(container.GetVal(), lhs, rhs));
    }
    */

  }  // Rt

}  // Stig