/* <base/uuid.h>

   A value-type wrapper for uuid_t defined in <uuid/uuid.h>.

   The text format used by TUuid looks like this: 1b4e28ba-2fa1-11d2-883f-b9a761bde3fb
   Which, as a printf pattern, would be: %08x-%04x-%04x-%04x-%012x

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

#include <algorithm>
#include <cassert>
#include <cstring>
#include <istream>
#include <random>
#include <ostream>
#include <stdexcept>
#include <uuid/uuid.h>

#include <base/no_default_case.h>
#include <base/spin_lock.h>
#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>

namespace Base {

  /* A value-type wrapper for uuid_t defined in <uuid/uuid.h>. */
  class TUuid {
    public:

    /* The size of a string representation of a uuid. */
    static const size_t StrSize = 36;

    /* The minimum size, in bytes, of the buffers used by Format() and Parse(). */
    static const size_t MinBufSize = StrSize + 1;

    /* TODO */
    static std::mt19937_64 TwisterEngine;
    static Base::TSpinLock TwisterLock;

    /* The algorithm to use when generating a new uuid. */
    enum TAlgo {

      /* Use /dev/random, if it exists; otherwise, behave as TimeAndMAC. */
      Best,

      /* Use /dev/random, if it exists; otheriwse, use a pseudo-random generator. */
      Random,

      /* Use the system time and MAC address of the ethernet card, if available.
         Try to avoid collisions by using global lock counter or UUID daemon, if the process has permission to do so.
         Collision is unlikely but possible. */
      TimeAndMAC,

      /* As TimeAndMAC, but do not allow collisions.
         If a collision-free guarantee is not possible, throw TUnsafeError. */
      TimeAndMACSafe,

      /* TWister */
      Twister

    };  // TAlgo

    /* The casing to use when formatting a uuid as text. */
    enum TCasing {

      /* Use the system default casing. */
      DefaultCasing,

      /* Use all upper-case letters. */
      Upper,

      /* Use all lower-case letters. */
      Lower

    };  // TCasing

    /* The exception thrown if text cannot be parsed as a uuid. */
    class TSyntaxError
        : public std::runtime_error {
      public:

      /* Do-little. */
      TSyntaxError()
          : std::runtime_error("cannot parse text as uuid") {}

    };  // TSyntaxError

    /* The exception thrown if the TimeAndMACSafe algorithm cannot run. */
    class TUnsafeError
        : public std::runtime_error {
      public:

      /* Do-little. */
      TUnsafeError()
          : std::runtime_error("cannot generate uuid safely") {}

    };  // TUnsafeError

    /* Default construct in the null state. */
    TUuid() {
      uuid_clear(Data);
    }

    /* Move constructor. */
    TUuid(TUuid &&that) {
      std::swap(Data, that.Data);
    }

    /* Copy constructor. */
    TUuid(const TUuid &that) {
      uuid_copy(Data, that.Data);
    }

    /* Copy constructor. */
    TUuid(const uuid_t &that) {
      uuid_copy(Data, that);
    }

    /* Construct with a unique state. */
    TUuid(TAlgo that) {
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

    /* Construct from a 36-character, null-terminated string.
       See the comment at the top of this file for more information about text formatting. */
    explicit TUuid(const char *that) {
      if (uuid_parse(that, Data) != 0) {
        throw TSyntaxError();
      }
    }

    /* Swaperator. */
    TUuid &operator=(TUuid &&that) {
      assert(this);
      assert(&that);
      if (this != &that) {
        std::swap(Data, that.Data);
      }
      return *this;
    }

    /* Copy assignment. */
    TUuid &operator=(const TUuid &that) {
      assert(this);
      assert(&that);
      if (this != &that) {
        uuid_copy(Data, that.Data);
      }
      return *this;
    }

    /* Copy assignment. */
    TUuid &operator=(const uuid_t &that) {
      assert(this);
      assert(&that);
      if (&Data != &that) {
        uuid_copy(Data, that);
      }
      return *this;
    }

    /* Unique assignment. */
    TUuid &operator=(TAlgo that) {
      assert(this);
      return *this = TUuid(that);
    }

    /* True iff. the uuid is not null. */
    operator bool() const {
      return uuid_is_null(Data) == 0;
    }

    /* Comparator. */
    bool operator==(const TUuid &that) const {
      assert(this);
      assert(&that);
      return uuid_compare(Data, that.Data) == 0;
    }

    /* Comparator. */
    bool operator!=(const TUuid &that) const {
      assert(this);
      assert(&that);
      return uuid_compare(Data, that.Data) != 0;
    }

    /* Comparator. */
    bool operator<(const TUuid &that) const {
      assert(this);
      assert(&that);
      return uuid_compare(Data, that.Data) < 0;
    }

    /* Comparator. */
    bool operator<=(const TUuid &that) const {
      assert(this);
      assert(&that);
      return uuid_compare(Data, that.Data) <= 0;
    }

    /* Comparator. */
    bool operator>(const TUuid &that) const {
      assert(this);
      assert(&that);
      return uuid_compare(Data, that.Data) > 0;
    }

    /* Comparator. */
    bool operator>=(const TUuid &that) const {
      assert(this);
      assert(&that);
      return uuid_compare(Data, that.Data) >= 0;
    }

    /* Comparator. */
    int Compare(const TUuid &that) const {
      assert(this);
      assert(&that);
      return uuid_compare(Data, that.Data);
    }

    /* Format the uuid as a 36-character string, followed by a null terminator.
       The buffer must be at least MinBufSize bytes long.
       See the comment at the top of this file for more information about text formatting. */
    void Format(char *buf, TCasing casing = DefaultCasing) const {
      assert(this);
      assert(buf);
      switch (casing) {
        case DefaultCasing: {
          uuid_unparse(Data, buf);
          break;
        }
        case Upper: {
          uuid_unparse_upper(Data, buf);
          break;
        }
        case Lower: {
          uuid_unparse_lower(Data, buf);
          break;
        }
        NO_DEFAULT_CASE;
      }
    }

    /* Format with an underscore instead of '-' */
    void FormatUnderscore(char *buf, TCasing casing = DefaultCasing) const {
      assert(this);
      Format(buf, casing);
      buf[8] = '_';
      buf[13] = '_';
      buf[18] = '_';
      buf[23] = '_';
    }

    /* Hasher. */
    size_t GetHash() const {
      assert(this);
      return *reinterpret_cast<size_t *>(const_cast<unsigned char *>(Data));
    }

    /* TODO */
    const uuid_t &GetRaw() const {
      assert(this);
      return Data;
    }

    /* Parse a 36-character, null-terminated string as a uuid.
       The buffer must be at least MinBufSize bytes long.
       See the comment at the top of this file for more information about text formatting. */
    TUuid &Parse(const char *buf) {
      assert(this);
      return *this = TUuid(buf);
    }

    /* Read from std stream. */
    void Read(std::istream &strm) {
      assert(this);
      assert(&strm);
      char buf[MinBufSize];
      strm.read(buf, StrSize);
      buf[StrSize] = '\0';
      *this = TUuid(buf);
    }

    /* Read from a binary stream. */
    void Read(Io::TBinaryInputStream &strm) {
      assert(this);
      assert(&strm);
      strm >> Data;
    }

    /* Return to the null state. */
    TUuid &Reset() {
      assert(this);
      uuid_clear(Data);
      return *this;
    }

    /* Write to a std stream. */
    void Write(std::ostream &strm) const {
      assert(this);
      assert(&strm);
      char buf[MinBufSize];
      Format(buf);
      strm << buf;
    }

    /* Write to a binary stream. */
    void Write(Io::TBinaryOutputStream &strm) const {
      assert(this);
      assert(&strm);
      strm << Data;
    }

    /* An instance of this class in the null state.
       Useful for providing default arguments, etc. */
    static const TUuid Null;

    private:

    /* The raw data of the uuid. */
    uuid_t Data;

  };  // TUuid

  /* A standard stream extractor for Base::TUuid. */
  inline std::istream &operator>>(std::istream &strm, TUuid &that) {
    assert(&that);
    that.Read(strm);
    return strm;
  }

  /* A standard stream inserter for Base::TUuid. */
  inline std::ostream &operator<<(std::ostream &strm, const TUuid &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

  /* Binary extractor for Base::TUuid. */
  inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TUuid &that) {
    assert(&that);
    that.Read(strm);
    return strm;
  }

  /* Binary inserter for Base::TUuid. */
  inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TUuid &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

}  // Base

namespace std {

  /* A standard hasher for Base::TUuid. */
  template <>
  struct hash<Base::TUuid> {
    typedef size_t result_type;
    typedef Base::TUuid argument_type;
    size_t operator()(const Base::TUuid &that) const {
      return that.GetHash();
    }
  };

}  // std