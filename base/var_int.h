/* <base/var_int.h>

   Serialize integers such that smaller values take up a smaller number of bytes.

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

#pragma once

#include <cstddef>
#include <cstdint>

#include <base/no_construction.h>

namespace Base {

  /* Specializations of this template define 'MaxSize', which gives the maximum number of bytes needed to encode an integer like TVar. */
  template <typename TVal>
  class VarInt;

  /* Specialization for 8-bit integers. */
  template <>
  class VarInt<uint8_t> {
    NO_CONSTRUCTION(VarInt);
    public:

    /* The maximum number of bytes needed.*/
    static const size_t MaxSize = 2;

  };  // VarInt<uint8_t>

  /* Specialization for 16-bit integers. */
  template <>
  class VarInt<uint16_t> {
    NO_CONSTRUCTION(VarInt);
    public:

    /* The maximum number of bytes needed.*/
    static const size_t MaxSize = 3;

  };  // VarInt<uint16_t>

  /* Specialization for 32-bit integers. */
  template <>
  class VarInt<uint32_t> {
    NO_CONSTRUCTION(VarInt);
    public:

    /* The maximum number of bytes needed.*/
    static const size_t MaxSize = 5;

  };  // VarInt<uint32_t>

  /* Specialization for 64-bit integers. */
  template <>
  class VarInt<uint64_t> {
    NO_CONSTRUCTION(VarInt);
    public:

    /* The maximum number of bytes needed.*/
    static const size_t MaxSize = 10;

  };  // VarInt<uint64_t>

  /* Read the value from the buffer and return a pointer just past what was read. */
  const char *ReadVarInt(const char *buffer, uint8_t &value);

  /* Read the value from the buffer and return a pointer just past what was read. */
  const char *ReadVarInt(const char *buffer, uint16_t &value);

  /* Read the value from the buffer and return a pointer just past what was read. */
  const char *ReadVarInt(const char *buffer, uint32_t &value);

  /* Read the value from the buffer and return a pointer just past what was read. */
  const char *ReadVarInt(const char *buffer, uint64_t &value);

  /* Write the value into the buffer and return a pointer just past what was written. */
  char *WriteVarInt(char *buffer, uint64_t value);

}  // Base
