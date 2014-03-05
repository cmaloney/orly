/* <stig/uuid.h>

   A Stig uuid, which is a <uuid/uuid.h>.

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

#include <cassert>

#include <string>
#include <sstream>

#include <uuid/uuid.h>

namespace Stig {

  /* TODO */
  class TUUID {
    public:

    /* TODO */
    TUUID();

    /* TODO */
    TUUID(const TUUID &that);

    /* TODO */
    TUUID(const uuid_t &id);

    /* TODO
       NOTE: May throw a TSyntaxError. */
    TUUID(const char *in);

    /* TODO */
    TUUID &operator=(const TUUID &that);

    /* TODO */
    bool operator==(const TUUID &that) const;

    /* TODO */
    bool operator!=(const TUUID &that) const;

    /* TODO */
    bool operator<(const TUUID &that) const;

    /* TODO */
    bool operator<=(const TUUID &that) const;

    /* TODO */
    bool operator>(const TUUID &that) const;

    /* TODO */
    bool operator>=(const TUUID &that) const;

    /* TODO */
    std::string AsString() const;

    /* TODO */
    int Compare(const TUUID &that) const;

    /* TODO */
    size_t GetHash() const;

    /* TODO */
    void Write(std::ostream &stream) const;

    static bool IsValidUuid(const char *s);

    /* Access to the native type, so we can marshall to Base::TUuid. */
    const uuid_t &GetRaw() const {
      assert(this);
      return UUID;
    }

    private:

    /* TODO */
    uuid_t UUID;

  };  // TUUID

}  // Stig

namespace std {

  /* A standard hasher for Stig::TUUID. */
  template <>
  struct hash<Stig::TUUID> {

    typedef size_t result_type;
    typedef Stig::TUUID argument_type;

    result_type operator()(const argument_type &that) const {
      return that.GetHash();
    }

  };  // hash<Stig::TUUID>

  /* TODO */
  inline ostream &operator<<(ostream &stream, const Stig::TUUID &uuid) {
    uuid.Write(stream);
    return stream;
  }

}  // std
