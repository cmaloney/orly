/* <strm/out.cc>

   Implements <strm/out.h>.

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

#include <strm/out.h>

#include <algorithm>
#include <cstring>

using namespace std;
using namespace Strm::Out;

TCons::TCons() noexcept
    : Prod(nullptr) {}

TCons::~TCons() {
  assert(this);
  assert(!Prod);
}

void TProd::Abandon() noexcept {
  assert(this);
  if (Start) {
    Cons->Cycle(Start, nullptr, nullptr);
    Start  = nullptr;
    Cursor = nullptr;
    Limit  = nullptr;
  }
}

void TProd::Flush() {
  assert(this);
  if (Cursor > Start) {
    Cons->Cycle(Cursor, nullptr, nullptr);
    Start  = nullptr;
    Cursor = nullptr;
    Limit  = nullptr;
  }
}

TProd::TProd(TCons *cons) noexcept
    : Start(nullptr), Cursor(nullptr), Limit(nullptr) {
  assert(cons);
  assert(!cons->Prod);
  Cons = cons;
  cons->Prod = this;
}

TProd::~TProd() {
  assert(this);
  assert(Cons->Prod == this);
  if (Start) {
    Cons->Cycle(Cursor, nullptr, nullptr);
  }
  Cons->Prod = nullptr;
}

void TProd::Write(const void *data, size_t size) {
  assert(this);
  assert(data || !size);
  auto *cursor = static_cast<const char *>(data);
  while (size) {
    if (Cursor >= Limit) {
      Cons->Cycle(Cursor, &Start, &Limit);
      assert(Start);
      assert(Start < Limit);
      Cursor = Start;
    }
    auto actl = min<size_t>(size, Limit - Cursor);
    memcpy(Cursor, cursor, actl);
    Cursor += actl;
    cursor += actl;
    size   -= actl;
  }
}
