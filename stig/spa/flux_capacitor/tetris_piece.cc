/* <stig/spa/flux_capacitor/tetris_piece.cc>

   Implements <stig/spa/flux_capacitor/tetris_piece.h>.

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

#include <stig/spa/flux_capacitor/tetris_piece.h>

#include <algorithm>
#include <vector>

using namespace std;
using namespace Base;
using namespace Stig::Spa::FluxCapacitor;

void TTetrisPiece::PlayTetris(TContext &ctxt, const function<void (const function<void (TTetrisPiece *)> &)> &piece_generator) {
  assert(&piece_generator);
  /* Age the generated pieces and fail any that are now too old.  Collect the rest into a group we need to process.
     If, for some reason, the generator yields a null pointer, just skip it. */
  vector<TTetrisPiece *> pieces;
  piece_generator([&pieces](TTetrisPiece *piece) {
    if (piece) {
      if (piece->IncrAge()) {
        pieces.push_back(piece);
      } else {
        piece->Fail();
      }
    }
    return true;
  });
  /* Scan through the pieces, looking for ones we can promote.
     We do this in order, from oldest to youngest.  In the case of a tie in age, we go from largest number of keys to smallest. */
  sort(pieces.begin(), pieces.end(), [](const TTetrisPiece *lhs, const TTetrisPiece *rhs) {
    return (lhs->GetAge() > rhs->GetAge()) || (lhs->GetAge() == rhs->GetAge() && lhs->GetKeyCount() > rhs->GetKeyCount());
  });
  for (auto piece: pieces) {
    if (piece->Assert(ctxt)) {
      piece->Promote();
    }
  }
}

TTetrisPiece::~TTetrisPiece() {}
