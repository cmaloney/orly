/* <base/regex_matcher.h>

   C++ wrappers for POSIX regex API (regcomp(), regexec(), regerror(),
   regfree()).

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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <regex.h>
#include <sys/types.h>

#include <base/no_copy_semantics.h>
#include <base/thrower.h>

namespace Base {

  class TRegexMatcher final {
    NO_COPY_SEMANTICS(TRegexMatcher);

    public:

    DEFINE_ERROR(TRegexError, std::runtime_error,
                 "POSIX regex API reported error");

    /* This represents a compiled pattern produced by regcomp(). */
    class TPattern final {
      NO_COPY_SEMANTICS(TPattern);

      friend class TRegexMatcher;  // for access to 'Regex' member

      public:

      TPattern(const char *expr, int cflags) {
        int err = regcomp(&Regex, expr, cflags);
        if (err) {
          THROW_ERROR(TRegexError) << MakeErrorString(err, Regex);
        }
      }

      ~TPattern() {
        assert(this);
        regfree(&Regex);
      }

      private:

      regex_t Regex;
    };  // TPattern

    /* This represents a sequence of substring matches (regmatch_t structures)
       from a call to regexec(). */
    class TMatches final {
      public:

      explicit TMatches(size_t nmatch)
          : Buf(nmatch) {
        Clear();
      }

      /* Rewind the object to the beginning.  After calling this, you can
         iterate through the matches again if desired. */
      void Rewind() {
        assert(this);
        Index = 0;
        InvalidateCachedIndex();
      }

      void Clear() noexcept {
        assert(this);
        Rewind();
        MakeEmpty();
      }

      /* Return true if the iterator references a valid position (has not yet
         reached the end). */
      operator bool() const {
        assert(this);
        return TryRefresh();
      }

      /* Access the value at the current iterator position.  On entry, the
         iterator must reference a valid position. */
      const regmatch_t &operator*() const {
        assert(this);
        Refresh();
        return Buf[CachedIndex];
      }

      /* Access the value at the current iterator position.  On entry, the
         iterator must reference a valid position. */
      const regmatch_t *operator->() const {
        assert(this);
        Refresh();
        return &Buf[CachedIndex];
      }

      /* Advance the iterator to the next position.  On entry, the iterator
         must be at a valid position. */
      TMatches &operator++() {
        assert(this);
        Refresh();
        InvalidateCachedIndex();
        return *this;
      }

      private:

      friend class TRegexMatcher;  // for access to 'Buf' member

      bool CachedIndexIsValid() const {
        assert(this);
        return (CachedIndex < Buf.size());
      }

      void InvalidateCachedIndex() {
        assert(this);
        CachedIndex = Buf.size();
      }

      void MakeEmpty() noexcept {
        assert(this);
        if (!Buf.empty()) {
          /* Mark the first regmatch_t as invalid so our sequence starts out
             empty. */
          Buf[0].rm_so = -1;
        }
      }

      void Refresh() const {
        assert(this);
        if (!TryRefresh()) {
          assert(false);
        }
      }

      bool TryAdvance() const;

      bool TryRefresh() const {
        assert(this);
        bool valid = CachedIndexIsValid();
        if (!valid) {
          valid = TryAdvance();
        }
        return valid;
      }

      std::vector<regmatch_t> Buf;
      mutable size_t Index;
      mutable size_t CachedIndex;
    };  // TMatches

    TRegexMatcher() = default;

    /* A true return value indicates that a match was found. */
    bool Match(const TPattern &pat, const char *str, TMatches &matches,
               int eflags);

    private:

    static std::string MakeErrorString(int errcode, const regex_t &regex);
  };  // TRegexMatcher

  static inline void SanityCheckStrAndMatch(const char *str,
      const regmatch_t& match) {
#ifndef NDEBUG
    assert(str);
    assert(match.rm_so >= 0);
    assert(match.rm_eo >= 0);
    size_t size = std::strlen(str);
    assert(static_cast<size_t>(match.rm_so) <= size);
    assert(static_cast<size_t>(match.rm_eo) <= size);
    assert(match.rm_eo >= match.rm_so);
#endif
  }

  static inline std::string
  RegexMatchToString(const char *str, const regmatch_t& match) {
    SanityCheckStrAndMatch(str, match);
    return std::string(&str[match.rm_so], &str[match.rm_eo]);
  }

  static inline void
  RegexMatchToString(const char *str, const regmatch_t& match,
      std::string& result) {
    SanityCheckStrAndMatch(str, match);
    result.assign(&str[match.rm_so], &str[match.rm_eo]);
  }

}  // Base