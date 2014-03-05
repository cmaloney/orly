/* <strm/in.cc>

   Implements <strm/in.h>.

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

#include <strm/in.h>

#include <algorithm>
#include <cstring>

using namespace std;
using namespace Strm::In;

TProd::TProd() noexcept
    : Cons(nullptr) {}

TProd::~TProd() {
  assert(this);
  assert(!Cons);
}

TCons::TCons(TProd *prod) noexcept
    : Start(nullptr), Cursor(nullptr), Limit(nullptr), AtEnd(false) {
  assert(prod);
  assert(!prod->Cons);
  Prod = prod;
  prod->Cons = this;
}

TCons::~TCons() {
  assert(this);
  assert(Prod->Cons == this);
  if (Start) {
    Prod->Cycle(1, nullptr, nullptr);
  }
  Prod->Cons = nullptr;
}

void TCons::Read(void *data, size_t size) {
  assert(this);
  assert(data || !size);
  auto *cursor = static_cast<char *>(data);
  while (size) {
    Refresh();
    auto actl = min<size_t>(size, Limit - Cursor);
    memcpy(cursor, Cursor, actl);
    Cursor += actl;
    cursor += actl;
    size   -= actl;
  }
}

void TCons::Skip(size_t size) {
  assert(this);
  while (size) {
    Refresh();
    auto actl = min<size_t>(size, Limit - Cursor);
    Cursor += actl;
    size   -= actl;
  }
}

void TCons::TryRefresh() const {
  assert(this);
  if (Cursor >= Limit) {
    if (Prod->Cycle(Start ? 1 : 0, &Start, &Limit)) {
      assert(Start < Limit);
    } else {
      Start = nullptr;
      Limit = nullptr;
      AtEnd = true;
    }
    Cursor = Start;
  }
}
