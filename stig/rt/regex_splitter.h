/* <stig/rt/regex_splitter.h>

   Split a text into pieces based on regex delimiters.

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
#include <tuple>
#include <utility>

#include <base/no_copy_semantics.h>
#include <stig/rt/opt.h>
#include <utf8/piece.h>
#include <utf8/regex.h>

namespace Stig {

  namespace Rt {

    /* Split a text into pieces based on regex delimiters.. */
    class TRegexSplitter final {
      NO_COPY_SEMANTICS(TRegexSplitter);
      public:

      /* Conveniences. */
      using TPiece = Utf8::TPiece;
      using TRegex = Utf8::TRegex;

      /* This is the element-delimiter pair over which we iterate.
         The delimiter is optional because the last item will be
         terminated by the end of the text, not a delimiter. */
      using TItem = std::tuple<std::string, Rt::TOpt<std::string>>;

      /* Search the given text for matches for the given regex. */
      TRegexSplitter(const TRegex *regex, const char *text, int flags = 0)
          : Regex(regex), Flags(flags), Next(text) {
        assert(regex);
        assert(text);
        ++(*this);
      }

      /* Our current item. */
      const TItem &operator*() const {
        assert(this);
        return Item;
      }

      /* Our current item. */
      const TItem *operator->() const {
        assert(this);
        return &Item;
      }

      /* Advance to the next item, if any.
         It is an error to call this if the current item's second piece is unknown. */
      TRegexSplitter &operator++();

      private:

      /* The regex pattern we use to match. */
      const TRegex *Regex;

      /* The flags we use when matching. */
      int Flags;

      /* Our current item. */
      TItem Item;

      /* The pointer to the last char of the matched regex */
      const char *Next;

    };  // TRegexSplitter

    class TRegexMatcher final {
      NO_COPY_SEMANTICS(TRegexMatcher);
      public:

      using TRegex = Utf8::TRegex;
      using TPiece = Utf8::TPiece;

      /* Search the given text for matches for the given regex. */
      TRegexMatcher(const TRegex *regex, const char *text, int flags = 0)
        : Regex(regex), Flags(flags), Next(text) {
        assert(regex);
        assert(text);
        ++(*this);
      }

      /* Our current Piece. */
      const std::string &operator*() const {
        assert(this);
        return Matched;
      }

      /* Our current Piece. */
      const std::string *operator->() const {
        assert(this);
        return &Matched;
      }

      /* Advance to the next item, if any. */
      TRegexMatcher &operator++();

      virtual operator bool() const {
        assert(this);
        return !(Matched == "" && Next == nullptr);
      }

      private:

      /* The regex pattern we use to match. */
      const TRegex *Regex;

      /* The flags we use when matching. */
      int Flags;

      /* Our current matched Piece */
      std::string Matched;

      /* The pointer to the last char of the matched regex */
      const char *Next;

    };  // TRegexMatcher


  }  // Rt

}  // Stig
