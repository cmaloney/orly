/* <strm/idx_iter.h>

   Templates for working with sequences of tuple indices.

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

#include <cstddef>

namespace Strm {

  /* Take an instance of IdxIter<Idx...> when you want to iterate through the
     elements of a tuple from 0 to n.  You can then use something like
     'f(std::get<idx>(t))...' within your function body to apply function 'f'
     to each element of tuple 't' in turn. */
  template <size_t...>
  class IdxIter {};

  /* Use this template to construct an instance to pass to a function which
     takes a parameter of type IdxIter<>.  For example, if you're working
     with a parameter pack 'X', you can say 'MakeIdxIter<sizeof...(X)>()'
     to create the indices 0, 1, 2... up to the size of 'X. */
  template <size_t N, size_t... Idx>
  class MakeIdxIter
      : public MakeIdxIter<N - 1, N - 1, Idx...> {};

  /* The base case makes an instance compatible with IdxIter<>. */
  template <size_t... Idx>
  class MakeIdxIter<0, Idx...>
      : public IdxIter<Idx...> {};

}  // Strm
