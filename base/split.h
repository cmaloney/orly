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

#include <util/tuple.h>

namespace Base {

template <typename TContainer, typename TDelimit, typename TFormat>
struct TJoin {
  const TContainer &Container;

  TDelimit Delimit;

  TFormat Format;

};  // TJoin

template <typename TContainer, typename TDelimit, typename TFormat>
[[gnu::warn_unused_result]] auto MakeJoin(const TContainer &container,
                                          TDelimit &&delimit,
                                          TFormat &&format) {
  return TJoin<TContainer, std::decay_t<TDelimit>, std::decay_t<TFormat>>{
      container, std::forward<TDelimit>(delimit), std::forward<TFormat>(format)};
}

struct TNoDelimit {
  template <typename TStrm>
  void operator()(TStrm &) const {}

};  // TNoDelimit

template <typename TDelimiter>
struct TDefaultDelimit {
  template <typename TStrm>
  void operator()(TStrm &strm) const {
    strm << Delimiter;
  }

  const TDelimiter &Delimiter;

};  // TDefaultDelimit

struct TDefaultFormat {
  template <typename TStrm, typename TElem>
  void operator()(TStrm &strm, const TElem &elem) const {
    strm << elem;
  }

};  // TDefaultFormat

template <typename TContainer, typename TDelimiter, typename TFormat>
auto Join(const TContainer &container, const TDelimiter &delimiter, TFormat &&format) {
  return MakeJoin(container, TDefaultDelimit<TDelimiter>{delimiter}, std::forward<TFormat>(format));
}

template <typename TContainer, typename TDelimiter>
auto Join(const TContainer &container, const TDelimiter &delimiter) {
  return Join(container, delimiter, TDefaultFormat());
}

template <typename TContainer>
auto Concat(const TContainer &container) {
  return MakeJoin(container, TNoDelimit(), TDefaultFormat());
}

/* A generic function to stream out TJoin<> to some streamer.
   The streamer type is templated to handle other streamers such as TCppPrinter. */
template <typename TStrm, typename TContainer, typename TDelimit, typename TFormat>
TStrm &WriteJoin(TStrm &strm, const TJoin<TContainer, TDelimit, TFormat> &that) {
  bool first = true;
  for(const auto &elem : that.Container) {
    if(first) {
      first = false;
    } else {
      that.Delimit(strm);
    }  // if
    that.Format(strm, elem);
  }  // for
  return strm;
}

template <typename TStrm, typename TDelimit, typename TFormat>
struct TWriteTuple {
  template <std::size_t Idx, typename TElem>
  void operator()(const TElem &elem) const {
    if(Idx) {
      Delimit(Strm);
    }  // if
    Format(Strm, elem);
  }

  TStrm &Strm;

  const TDelimit &Delimit;

  const TFormat &Format;

};  // TWriteTuple

/* Specialization for std::pair. */
template <typename TStrm, typename TLhs, typename TRhs, typename TDelimit, typename TFormat>
TStrm &WriteJoin(TStrm &strm, const TJoin<std::pair<TLhs, TRhs>, TDelimit, TFormat> &that) {
  Util::ForEach(that.Container,
                TWriteTuple<TStrm, TDelimit, TFormat>{strm, that.Delimit, that.Format});
  return strm;
}

/* Specialization for std::tuple. */
template <typename TStrm, typename... TArgs, typename TDelimit, typename TFormat>
TStrm &WriteJoin(TStrm &strm, const TJoin<std::tuple<TArgs...>, TDelimit, TFormat> &that) {
  Util::ForEach(that.Container,
                TWriteTuple<TStrm, TDelimit, TFormat>{strm, that.Delimit, that.Format});
  return strm;
}

/* Specialization of WriteJoin for std::ostream. */
template <typename TContainer, typename TDelimit, typename TFormat>
std::ostream &operator<<(std::ostream &strm, const TJoin<TContainer, TDelimit, TFormat> &that) {
  return WriteJoin(strm, that);
}

/* split the src string every time tok apperas, up to count instances if count is specified */
void Split(const char *tok, const std::string &src, std::vector<std::string> &pieces);
}
