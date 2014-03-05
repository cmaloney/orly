/* <utf8/regex.h>

   Regular expression matching.

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

#include <regex.h>

#include <base/no_copy_semantics.h>
#include <base/regex_matcher.h>
#include <base/thrower.h>
#include <utf8/piece.h>

namespace Utf8 {

  /* A compiled regular expression. */
  class TRegex final {
    NO_COPY_SEMANTICS(TRegex);
    public:

    /* The error we throw. */
    DEFINE_ERROR(TError, std::runtime_error, "regex error");

    /* A cursor for walking regex pattern matches. */
    class TCursor final {
      NO_COPY_SEMANTICS(TCursor);
      public:

      /* Search the given text for matches for the given regex. */
      TCursor(const TRegex *regex, const char *text, int flags = 0)
          : Regex(regex), Text(text), Flags(flags), Status(Unknown) {
        assert(regex);
        assert(text);
      }

      /* True iff. we have a match. */
      operator bool() const {
        assert(this);
        TryRefresh();
        return Status == Match;
      }

      /* Our current match. */
      const TPiece &operator*() const {
        assert(this);
        Refresh();
        return Piece;
      }

      /* Our current match. */
      const TPiece *operator->() const {
        assert(this);
        Refresh();
        return &Piece;
      }

      /* Advance to the next match, if any. */
      TCursor &operator++() {
        assert(this);
        Refresh();
        Text = Piece.GetStart() + 1;
        Flags |= REG_NOTBOL;
        Status = Unknown;
        return *this;
      }

      private:

      /* Make sure we have a match. */
      void Refresh() const {
        assert(this);
        TryRefresh();
        assert(Status == Match);
      }

      /* Try to make sure we have a match. */
      void TryRefresh() const {
        assert(this);
        if (Status == Unknown) {
          Status = Regex->TryGetMatch(Text, Piece, Flags) ? Match : NoMoreMatches;
        }
      }

      /* The regex pattern we use to match. */
      const TRegex *Regex;

      /* The start of the next regex match attempt. */
      const char *Text;

      /* The flags we use when matching. */
      int Flags;

      /* Is this cursor currently positioned at a match? */
      mutable enum TStatus {

        /* We don't know.  We'll have to refresh to find out. */
        Unknown,

        /* Yes, we have a match.  'Piece' is pointing at it. */
        Match,

        /* No, there are no more matches.  The cursor is exhausted. */
        NoMoreMatches

      } Status;

      /* Points at our current match, if we have one; otherwise, undefined. */
      mutable TPiece Piece;

    };  // TCursor

    /* Compile the given pattern. */
    TRegex(const char *pattern, int flags = REG_EXTENDED);

    /* True iff. the given text matches this pattern. */
    bool IsMatch(const char *text, int flags = 0) const;

    /* If there is a match for this pattern in the given text,
       set the piece to point to it and return true;
       otherwise, leave the piece alone and return false. */
    bool TryGetMatch(const char *text, TPiece &piece, int flags = 0) const;

    private:

    /* Contains OS regex object. */
    Base::TRegexMatcher::TPattern Handle;

  };  // TRegex

}  // Utf8
