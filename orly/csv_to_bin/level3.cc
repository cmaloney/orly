/* <orly/csv_to_bin/level3.cc>

   Implements <orly/csv_to_bin/level3.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/csv_to_bin/level3.h>

using namespace std;
using namespace Orly::CsvToBin;

TLevel3 &TLevel3::operator>>(bool &that) {
  assert(this);
  assert(&that);
  return *this;
}

TLevel3 &TLevel3::operator>>(Base::TUuid &that) {
  assert(this);
  assert(&that);
  return *this;
}

TLevel3 &TLevel3::operator>>(int64_t &that) {
  assert(this);
  assert(&that);
  return *this;
}

TLevel3 &TLevel3::operator>>(double &that) {
  assert(this);
  assert(&that);
  return *this;
}

TLevel3 &TLevel3::operator>>(std::string &that) {
  assert(this);
  assert(&that);
  return *this;
}

TLevel3 &TLevel3::operator>>(Base::Chrono::TTimePnt &that) {
  assert(this);
  assert(&that);
  return *this;
}

void TLevel3::MatchState(TLevel2::TState state) {
  assert(this);
  if (Level2->State != state) {
    THROW_ERROR(TUnexpectedState)
        << "expected " << state
        << ", found " << Level2->State;
  }
  ++Level2;
}
