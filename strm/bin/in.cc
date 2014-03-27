/* <strm/bin/in.cc>

   Implements <strm/bin/in.h>.

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

#include <strm/bin/in.h>

#include <strm/syntax_error.h>
#include <strm/bin/var_int.h>

using namespace std;
using namespace Strm::Bin;

TIn &TIn::operator>>(bool &that) {
  assert(this);
  assert(&that);
  switch (Peek()) {
    case 'T': {
      that = true;
      break;
    }
    case 'F': {
      that = false;
      break;
    }
    default: {
      throw TSyntaxError();
    }
  }
  Skip();
  return *this;
}

TIn &TIn::operator>>(string &that) {
  assert(this);
  assert(&that);
  size_t size;
  *this >> size;
  string temp;
  temp.resize(size);
  Read(const_cast<char *>(temp.data()), size);
  that = move(temp);
  return *this;
}

uint64_t TIn::ReadVarInt() {
  assert(this);
  const uint8_t *start, *limit;
  TVarIntDecoder decoder;
  const uint64_t *val;
  do {
    Peek(start, limit);
    const uint8_t *cursor = decoder.Decode(start, limit);
    Skip(cursor - start);
    val = *decoder;
  } while (!val);
  return *val;
}
