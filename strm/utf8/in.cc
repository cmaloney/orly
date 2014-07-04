/* <strm/utf8/in.cc>

   Implements <strm/utf8/in.h>.

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

#include <strm/utf8/in.h>

#include <strm/syntax_error.h>

using namespace std;
using namespace Strm::Utf8;

char32_t TIn::ReadChar() const {
  assert(this);
  uint8_t c = Peek();
  char32_t result;
  ssize_t more;
  if (c <= 0x7F) {
    result = c;
    more = 0;
  } else if ((c & 0xE0) == 0xC0) {
    result = c & 31;
    more = 1;
  } else if ((c & 0xF0) == 0xE0) {
    result = c & 15;
    more = 2;
  } else if ((c & 0xF8) == 0xF0) {
    result = c & 7;
    more = 3;
  } else if ((c & 0xFC) == 0xF8) {
    more = -1;
  } else if ((c & 0xFE) == 0xFC) {
    more = -1;
  } else {
    more = -1;
  }
  if (more < 0) {
    THROW_ERROR(TBadEncoding);
  }
  for (;;) {
    const_cast<TIn *>(this)->Skip();
    if (!more) {
      break;
    }
    --more;
    const auto *ptr = TryPeek();
    if (!ptr) {
      THROW_ERROR(TCutOff);
    }
    c = *ptr;
    if ((c & 0xC0) != 0x80) {
      THROW_ERROR(TBadEncoding);
    }
    result = (result << 6) | (c & 0x3F);
  }
  return result;
}
