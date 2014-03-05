/* <tools/nycr/symbol/name.h>

   A symbolic name, broken into parts, which can be represented in any of
   three forms:

     lower-case style:  strm << TLower(name);  ==> "my_named_thing"
     upper-case style:  strm << TUpper(name);  ==> "MyNamedThing"
     type style:        strm << TType(name);   ==> "TMyNamedThing"

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
#include <cstring>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TName {
        public:

        /* TODO */
        TName() {}

        /* TODO */
        TName(TName &&that) {
          std::swap(Parts, that.Parts);
        }

        /* TODO */
        TName(const TName &that)
            : Parts(that.Parts) {}

        /* TODO */
        TName(const char *that) {
          Init(that, that + strlen(that));
        }

        /* TODO */
        TName(const std::string &that) {
          Init(that.data(), that.data() + that.size());
        }

        /* TODO */
        TName &operator=(TName &&that) {
          assert(this);
          std::swap(Parts, that.Parts);
          return *this;
        }

        /* TODO */
        TName &operator=(const TName &that) {
          assert(this);
          return *this = TName(that);
        }

        /* TODO */
        TName &operator=(const char *that) {
          assert(this);
          return *this = TName(that);
        }

        /* TODO */
        TName &operator=(const std::string &that) {
          assert(this);
          return *this = TName(that);
        }

        /* TODO */
        bool operator<(const TName &that) const;

        /* TODO */
        void WriteLower(std::ostream &strm) const;

        /* TODO */
        void WriteUpper(std::ostream &strm) const;

	/* TODO */
	void WriteXml(std::ostream &strm) const;

        private:

        /* TODO */
        void Init(const char *start, const char *limit);

        /* TODO */
        std::vector<std::string> Parts;

      };  // TName

      /* TODO */
      class TLower {
        public:

        /* TODO */
        TLower(const TName &name)
            : Name(name) {}

        /* TODO */
        const TName &Name;

      };  // TLower

      /* TODO */
      class TUpper {
        public:

        /* TODO */
        TUpper(const TName &name)
            : Name(name) {}

        /* TODO */
        const TName &Name;

      };  // TUpper

      /* TODO */
      class TType {
        public:

        /* TODO */
        TType(const TName &name)
            : Name(name) {}

        /* TODO */
        const TName &Name;

      };  // TType

      /* TODO */
      class TXml {
      public:

	/* TODO */
      TXml(const TName &name)
	: Name(name) {}

	/* TODO */
	const TName &Name;

      }; // TXml

/* TODO */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TLower &that) {
  assert(&that);
  that.Name.WriteLower(strm);
  return strm;
}

/* TODO */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TUpper &that) {
  assert(&that);
  that.Name.WriteUpper(strm);
  return strm;
}

/* TODO */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TType &that) {
  assert(&strm);
  strm << 'T';
  that.Name.WriteUpper(strm);
  return strm;
}

/* TODO */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TXml &that) {
  assert(&strm);
  that.Name.WriteXml(strm);
  return strm;
}

    }  // Symbol

  }  // Nycr

}  // Tools