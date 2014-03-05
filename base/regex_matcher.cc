/* <base/regex_matcher.cc>

   Implements <base/regex_matcher.h>.

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

#include <base/regex_matcher.h>

namespace Base {

bool TRegexMatcher::TMatches::TryAdvance() const {
  assert(this);
  /* Note: A regmatch_t with a rm_so field of -1 indicates that there are no
     more substring matches. */
  bool result = (Index < Buf.size()) && (Buf[Index].rm_so >= 0);
  if (result) {
    CachedIndex = Index++;
  }
  return result;
}

bool TRegexMatcher::Match(const TPattern &pat, const char *str,
    TMatches &matches, int eflags) {
  assert(this);
  assert(str);
  /* First clear the matches so that they will be guaranteed empty if no match
     was found or the pattern was compiled with REG_NOSUB. */
  matches.Clear();
  int err = regexec(&pat.Regex, str, matches.Buf.size(), &matches.Buf[0],
                    eflags);
  assert((err == 0) || (err == REG_NOMATCH));
  return (err == 0);
}

std::string TRegexMatcher::MakeErrorString(int errcode, const regex_t &regex) {
  std::vector<char> buf(1024);
  regerror(errcode, &regex, &buf[0], buf.size());
  return std::string(&buf[0]);
}

}  // Base