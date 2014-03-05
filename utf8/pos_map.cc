/* <utf8/pos_map.cc>

   Implements <utf8/pos_map.h>.

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

#include <utf8/pos_map.h>

#include <algorithm>

using namespace std;
using namespace Utf8;

TPosMap::TPosMap(const TPiece &text) {
  assert(&text);
  if (text) {
    Eols.push_back(text.GetStart() - 1);
    TPiece csr = text;
    while (csr) {
      const char *ptr = csr.GetStart();
      if (csr.Pop() == 10) {
        Eols.push_back(ptr);
      }
    }
    Eols.push_back(text.GetLimit());
  }
}

TPos TPosMap::GetPos(const char *ptr) const {
  assert(this);
  assert(ptr > Eols[0]);
  auto iter = lower_bound(Eols.begin() + 1, Eols.end(), ptr);
  assert(iter != Eols.end());
  int col = 1;
  for (TPiece line(*(iter - 1) + 1, *iter); line && !line.Split().Contains(ptr); ++col);
  return TPos(iter - Eols.begin(), col);
}
