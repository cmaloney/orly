/* <stig/rt/regex_splitter.cc>

   Implements <stig/rt/regex_splitter.h>.

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

#include <stig/rt/regex_splitter.h>

using namespace std;
using namespace Stig::Rt;

TRegexSplitter &TRegexSplitter::operator++() {
  assert(this);
  assert(Next);
  TPiece match_delim;
  if (Regex->TryGetMatch(Next, match_delim, Flags)) {
    get<0>(Item) = string(Next, match_delim.GetStart());
    get<1>(Item) = string(match_delim.GetStart(), match_delim.GetLimit());
    Flags |= REG_NOTBOL;
    Next = match_delim.GetLimit();
  } else {
    get<0>(Item) = string(Next);
    get<1>(Item).Reset();
    Next = nullptr;
  }
  return *this;
}

TRegexMatcher &TRegexMatcher::operator++() {
  assert(this);
  assert(Next);
  TPiece match_delim;
  if (Regex->TryGetMatch(Next, match_delim, Flags)) {
    Matched = string(match_delim.GetStart(), match_delim.GetLimit());
    Flags |= REG_NOTBOL;
    Next = match_delim.GetLimit();
  } else {
    Matched = "";
    Next = nullptr;
  }
  return *this;
}

