/* <base/split.cc>

   Implements <base/split.h>

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/split.h>

#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;
using namespace Base;

vector<string> Base::Split(const char *tok, const string &src) {
  assert(tok);
  assert(&src);

  vector<string> pieces;

  if(src.size() == 0) {
    return pieces;
  }

  size_t curpos = 0, endpos = 0;
  const size_t tok_len = strlen(tok);
  do {
    endpos = src.find(tok, curpos);

    if(endpos == string::npos) {
      string newValue;
      if (src.size() == curpos) {
        pieces.push_back("");
      } else {
        string new_value(src.substr(curpos, src.size() - curpos));
        pieces.push_back(new_value);
      }
    } else {
      string new_value(src.substr(curpos, endpos - curpos));
      // Pull out the substring
      pieces.push_back(new_value);

      // NOTE: This should always be <= src.size().
      curpos = endpos + tok_len;
    }
  } while(endpos != string::npos);

  return pieces;
}
