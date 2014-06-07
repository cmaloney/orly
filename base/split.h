/* <base/split.h>

   Defines a function for spliting a string into pieces, as well as one for joining back together.

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

#include <functional>
#include <ostream>
#include <string>
#include <vector>

namespace Base {

  // TODO: Add a variant of the stream insertion that allows passing an element callback.
  template <typename TSepType, typename TContainer>
  class TJoin {
    public:
    TJoin(const TSepType &sep, const TContainer &container) : Sep(sep), Container(container) {}

    const TSepType &Sep;
    const TContainer &Container;
  };

  template <typename TSepType, typename TContainer>
  std::ostream &operator<<(std::ostream &out, const TJoin<TSepType, TContainer> &join) {
    bool first = true;
    for(const auto &elem: join.Container) {
      if (first) {
        first = false;
      } else {
        out << join.Sep;
      }

      out << elem;
    }

    return out;
  }

  // TODO: Migrate other uses of Join to this one which is more fluid to use in stream insertions.
  template <typename TSepType, typename TContainer>
  auto Join(const TSepType &sep, const TContainer &container) {
    return TJoin<TSepType, TContainer>(sep, container);
  }

  //NOTE: pieces_container must be able to have foreach run over it, and what it contains must
  template<typename TSepType, typename TVal, typename TStrm>
  TStrm &Join(const TSepType &sep, const TVal &pieces_container, TStrm &out) {
    bool first = true;
    for (auto it: pieces_container) {
      if (first) {
        first = false;
      } else {
        out << sep;
      }
      out << it;
    }

    return out;
  }

  //TODO: Tighter specification of TCb
  template<typename TSepType, typename TVal, typename TCb, typename TStrm>
  TStrm &Join(const TSepType &sep, const TVal &pieces_container, const TCb &cb,  TStrm &out) {
    bool first = true;
    for (const auto &it: pieces_container) {
      if (first) {
        first = false;
      } else {
        out << sep;
      }
      cb(it, out);
    }

    return out;
  }


  void Split(const char *tok, const std::string &src, std::vector<std::string> &pieces);
}