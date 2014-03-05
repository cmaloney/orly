/* <stig/native/type.cc>

   Implements <stig/native/type.h>.

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

#include <stig/native/type.h>

using namespace Stig;

/*
#define FOR_NULLARY(val_t, name)  \
  const Native::Type::TAny &Native::Type::For<val_t>::Type = Sabot::Type::T##name::name;  \
  const Sabot::Type::T##name &Native::Type::For<val_t>::name##Type = Sabot::Type::T##name::name;
FOR_NULLARY(int8_t, Int8)
FOR_NULLARY(int16_t, Int16)
FOR_NULLARY(int32_t, Int32)
FOR_NULLARY(int64_t, Int64)
FOR_NULLARY(uint8_t, UInt8)
FOR_NULLARY(uint16_t, UInt16)
FOR_NULLARY(uint32_t, UInt32)
FOR_NULLARY(uint64_t, UInt64)
FOR_NULLARY(bool, Bool)
FOR_NULLARY(char, Char)
FOR_NULLARY(float, Float)
FOR_NULLARY(double, Double)
FOR_NULLARY(Sabot::TStdDuration, Duration)
FOR_NULLARY(Sabot::TStdTimePoint, TimePoint)
FOR_NULLARY(Base::TUuid, Uuid)
FOR_NULLARY(Native::TBlob, Blob)
FOR_NULLARY(std::string, Str)
FOR_NULLARY(const char *, Str)
FOR_NULLARY(Native::TTombstone, Tombstone)
FOR_NULLARY(void, Void)
#undef FOR_NULLARY
*/