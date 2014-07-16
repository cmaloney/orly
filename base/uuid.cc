/* <base/uuid.cc>

   Implements <base/uuid.h>.

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

#include <base/uuid.h>

#include <chrono>

using namespace std;
using namespace Base;

const TUuid TUuid::Null;

bool TUuid::IsValidUuid(const char *s) {
  //TODO: Make more sophisticated.
  return strlen(s) == 36;
}

TUuid::TUuid(TAlgo that) {
  /* TODO */
  static std::mt19937_64 TwisterEngine(std::chrono::system_clock::now().time_since_epoch().count());
  static Base::TSpinLock TwisterLock;

  switch (that) {
    case Best: {
      uuid_generate(Data);
      break;
    }
    case Random: {
      uuid_generate_random(Data);
      break;
    }
    case TimeAndMAC: {
      uuid_generate_time(Data);
      break;
    }
    case TimeAndMACSafe: {
      /* TODO: We cannot currently support this mode because uuid_generate_time_safe() isn't in our OS build.
         When it becomes available, call it here and only throw TUnsafeError if the function returns an error. */
      throw TUnsafeError();
    }
    case Twister: {
      /* acquire Twister lock */ {
        Base::TSpinLock::TSoftLock lock(TwisterLock);
        *reinterpret_cast<uint_fast64_t *>(Data) = TwisterEngine();
        *(reinterpret_cast<uint_fast64_t *>(Data) + 1) = TwisterEngine();
      }  // release twister lock
      break;
    }
    NO_DEFAULT_CASE;
  }
}

