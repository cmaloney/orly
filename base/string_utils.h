/* <base/string_utils.h>

   Utilities for working with strings.

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

#include <cassert>
#include <functional>
#include <initializer_list>
#include <ostream>
#include <string>
#include <sstream>

namespace Base {

  /* The string ", ".  We use this commonly as a separator sequence in a string join. */
  extern const char *CommaSep;

  /* A function which uses c-strings returned by a generator. */
  using TCStrUser = std::function<bool (const char *)>;

  /* A function which generates c-strings on behalf of a user. */
  using TCStrGen = std::function<bool (const TCStrUser &)>;

  /* A function which writes to std out-stream. */
  using TWriter = std::function<void (std::ostream &)>;

  /* Concatenate the array of c-strings.  The array must be null-terminated. */
  std::string ConcatCStrArray(const char *c_strs[]);

  /* Concatenate the given list of c-strings.  The argument sequence must be null-terminated. */
  std::string ConcatCStrList(std::initializer_list<const char *> c_strs);

  /* Generate the c-strings given by the pair of iterators. */
  template <typename TIter>
  std::string ConcatCStrs(TIter begin, TIter end) {
    std::ostringstream strm;
    for (; begin != end; ++begin) {
      strm << *begin;
    }
    return strm.str();
  }

  /* Generate the array of c-strings.  The array must be null-terminated. */
  bool GenCStrArray(const TCStrUser &c_str_user, const char *c_strs[]);

  /* Generate the c-strings given by the initializer list. */
  bool GenCStrList(const TCStrUser &c_str_user, std::initializer_list<const char *> c_strs);

  /* Generate the c-strings given by the pair of iterators. */
  template <typename TIter>
  bool GenCStrs(const TCStrUser &c_str_user, TIter begin, TIter end) {
    assert(c_str_user);
    for (; begin != end; ++begin) {
      if (!c_str_user(*begin)) {
        return false;
      }
    }
    return true;
  }

  /* Collect the results of the writing function as a std string. */
  std::string ToString(const TWriter &writer);

  /* Write the generated c-strings to the given stream, separated with the given separator string
     If you generate any null pointers, we skip them.
     and enclosed with the given brackets.  Automatically injects a space before and after the sequence
     of strings (inside the brackets), but only if the sequence is non-empty.
     For example: "[ a, b, c ]" or "[]". */
  void WriteBracketedJoin(std::ostream &strm, const TCStrGen &c_str_gen, char open_c = '[', char close_c = ']', const char *sep = CommaSep);

  /* Write the generated c-strings to the given stream, separated with the given separator string.
     If you generate any null pointers, we skip them.
     For example: "a, b, c". */
  void WriteJoin(std::ostream &strm, const TCStrGen &c_str_gen, const char *sep = CommaSep);

}  // Base
