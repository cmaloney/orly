/* <utf8/regex.cc>

   Implements <utf8/regex.h>.

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

#include <utf8/regex.h>

using namespace Utf8;

TRegex::TRegex(const char *pattern, int flags)
    /* We don't allow REG_NOSUB because it hoses up TryGetMatch(). */
    : Handle(pattern, flags & ~REG_NOSUB) {
}

bool TRegex::IsMatch(const char *text, int flags) const {
  assert(this);
  assert(text);
  Base::TRegexMatcher::TMatches matches(1);
  return Base::TRegexMatcher().Match(Handle, text, matches, flags);
}

bool TRegex::TryGetMatch(const char *text, TPiece &piece, int flags) const {
  assert(this);
  assert(text);
  Base::TRegexMatcher::TMatches matches(1);
  bool success = Base::TRegexMatcher().Match(Handle, text, matches, flags);
  if (success) {
    /* We have a match.  Report its position. */
    piece = TPiece(text + matches->rm_so, text + matches->rm_eo);
  }
  return success;
}