/* <util/stl.h>

   Some utilities for working with STL containers.

   Copyright 2015 Theoretical Chaos.

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

#include <cassert>
#include <limits>
#include <type_traits>
#include <vector>
#include <utility>

#include <base/class_traits.h>
#include <base/unreachable.h>

namespace Util {

/* Return true iff. a given value is in a container. */
template <typename TContainer>
bool Contains(const TContainer &container, const typename TContainer::value_type &val) {
  return container.find(val) != container.end();
}

/* Erase the given value from the container.  If the value is not in the container, fail an
 * assertion. */
template <typename TContainer>
void EraseOrFail(TContainer &container, const typename TContainer::key_type &key) {
  auto result = container.erase(key);
  if (result != 1) {
    // TODO(cmaloney): Shuold be able to capture the condition which failed in the unreachable /
    // attach a message.
    Base::Unreachable(HERE);
  }
}

/* Insert the given value into the container.  If the value is already in the container, fail an
 * assertion. */
template <typename TContainer, typename... TArgs>
void InsertOrFail(TContainer &container, TArgs &&... args) {
  if (!container.emplace(std::forward<TArgs>(args)...).second) {
    Base::Unreachable(HERE);
  }
}

/* Returns a pointer to the value mapped to the given key.  If the value doesn't appear in the
 * container, return a null pointer. */
template <typename TContainer>
const typename TContainer::mapped_type *TryFind(const TContainer &container,
                                                const typename TContainer::key_type &key) {
  auto iter = container.find(key);
  return (iter != container.end()) ? &(iter->second) : nullptr;
}

template <typename TContainer>
auto Pop(TContainer &container) {
  auto res = container.front();
  container.pop();
  return res;
}

template <typename TVal>
auto Append(std::vector<TVal> &base, const std::vector<TVal> &additional) {
  base.reserve(base.size() + additional.size());
  base.insert(base.end(), begin(additional), end(additional));
  return base;
}

}
