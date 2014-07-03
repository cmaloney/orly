/* <strm/utf8/in.h>

   A consumer of in-flowing data in UTF8 format.

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

#pragma once

#include <cassert>

#include <base/thrower.h>
#include <strm/in.h>

namespace Strm {

  namespace Utf8 {

    /* A consumer of in-flowing data in UTF8 format. */
    class TIn final
        : public In::TCons {
      public:

      /* Thrown when we find bytes that aren't valid UTF8. */
      DEFINE_ERROR(
          TBadEncoding, std::runtime_error,
          "UTF8 stream contains bad encoding");

      /* Thrown when the producer ends in the middle of a multi-byte UTF8
         character. */
      DEFINE_ERROR(
          TCutOff, std::runtime_error,
          "UTF8 stream cut off");

      /* Pass-thru to our base's constructor. */
      TIn(In::TProd *prod)
          : TCons(prod), IsCached(false) {}

      /* The current character. */
      char32_t operator*() const {
        assert(this);
        if (!IsCached) {
          Cache = ReadChar();
          IsCached = true;
        }
        return Cache;
      }

      /* Advance to the next character. */
      TIn &operator++() {
        assert(this);
        if (IsCached) {
          IsCached = false;
        } else {
          ReadChar();
        }
        return *this;
      }

      private:

      /* Read the next character from our producer. */
      char32_t ReadChar() const;

      /* True when Cache is valid. */
      mutable bool IsCached;

      /* The current character, or junk if Cache is false. */
      mutable char32_t Cache;

    };  // TIn

  }  // Utf8

}  // Strm
