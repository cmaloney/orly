/* <base/var_int.cc>

   Implements <base/var_int.h>.

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

#include <base/var_int.h>

#include <cassert>

const char *Base::ReadVarInt(const char *buffer, uint8_t &value) {
  uint64_t temp;
  buffer = ReadVarInt(buffer, temp);
  value = temp;
  return buffer;
}

const char *Base::ReadVarInt(const char *buffer, uint16_t &value) {
  uint64_t temp;
  buffer = ReadVarInt(buffer, temp);
  value = temp;
  return buffer;
}

const char *Base::ReadVarInt(const char *buffer, uint32_t &value) {
  uint64_t temp;
  buffer = ReadVarInt(buffer, temp);
  value = temp;
  return buffer;
}

const char *Base::ReadVarInt(const char *buffer, uint64_t &value) {
  assert(buffer);
  assert(&value);
  value = 0;
  uint64_t place = 1;
  for (;;) {
    uint8_t byte = *buffer++;
    if (byte & 0x80U) {
      value += (byte & 0x7FU) * place;
    } else {
      value += byte * place;
      break;
    }
    place <<= 7;
  }
  return buffer;
}

char *Base::WriteVarInt(char *buffer, uint64_t value) {
  assert(buffer);
  while (value >= 128) {
    if (value < 128) {
      break;
    }
    *buffer++ = static_cast<char>(value | 0x80);
    value >>= 7;
  }
  *buffer++ = static_cast<char>(value);
  return buffer;
}
