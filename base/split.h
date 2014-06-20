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

  template <typename TContainer, typename TDelimiter, typename TFormat>
  struct TJoin {

    TJoin(const TContainer &container,
          const TDelimiter &delimiter,
          const TFormat &format)
        : Container(container),
          Delimiter(delimiter),
          Format(format) {}

    const TContainer &Container;

    const TDelimiter &Delimiter;

    const TFormat &Format;

  };  // TJoin

  template <typename TContainer, typename TDelimiter, typename TFormat>
  auto Join(const TContainer &container,
            const TDelimiter &delimiter,
            const TFormat &format) {
    return TJoin<TContainer, TDelimiter, TFormat>(container, delimiter, format);
  }

  template <typename TContainer, typename TDelimiter>
  auto Join(const TContainer &container, const TDelimiter &delimiter) {
    return Join(container,
                delimiter,
                [](auto &strm, const auto &elem) {
                  strm << elem;
                });
  }

  /* A generic function to stream out TJoin<> to some streamer.
     The streamer type is templated to handle other streamers such as TCppPrinter. */
  template <typename TStrm,
            typename TContainer,
            typename TDelimiter,
            typename TFormat>
  TStrm &WriteJoin(TStrm &strm,
                   const TJoin<TContainer, TDelimiter, TFormat> &that) {
    for (const auto &elem : that.Container) {
      if (&elem != &*std::begin(that.Container)) {
        strm << that.Delimiter;
      }  // if
      that.Format(strm, elem);
    }  // for
    return strm;
  }

  template <typename TStrm, typename TDelimiter, typename TFormat>
  struct TWriteTuple {

    explicit TWriteTuple(TStrm &strm,
                         const TDelimiter &delimiter,
                         const TFormat &format)
        : Strm(strm),
          Delimiter(delimiter),
          Format(format) {}

    template <std::size_t Idx, typename TElem>
    void operator()(const TElem &elem) const {
      if (Idx) {
        Strm << Delimiter;
      }  // if
      Format(Strm, elem);
    }

    private:

    TStrm &Strm;

    const TDelimiter &Delimiter;

    const TFormat &Format;

  };  // TWriteTuple

  /* Specialization for std::tuple. */
  template <typename TStrm,
            typename... TArgs,
            typename TDelimiter,
            typename TFormat>
  TStrm &WriteJoin(TStrm &strm,
                   const TJoin<std::tuple<TArgs...>, TDelimiter, TFormat> &that) {
    Util::ForEach(that.Container,
                  TWriteTuple<TStrm, TDelimiter, TFormat>(strm, that.Delimiter, that.Format));
    return strm;
  }

  /* Specialization of WriteJoin for std::ostream. */
  template <typename TContainer, typename TDelimiter, typename TFormat>
  std::ostream &operator<<(std::ostream &strm,
                           const TJoin<TContainer, TDelimiter, TFormat> &that) {
    return WriteJoin(strm, that);
  }

  void Split(const char *tok, const std::string &src, std::vector<std::string> &pieces);
}
