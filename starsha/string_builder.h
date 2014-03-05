/* <starsha/string_builder.h>

   TODO

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
#include <sstream>

#include <base/no_copy_semantics.h>

namespace Starsha {

  /* TODO */
  class TStringBuilder {
    NO_COPY_SEMANTICS(TStringBuilder);
    public:

    /* TODO */
    TStringBuilder(std::string &out)
        : Out(out) {}

    /* TODO */
    ~TStringBuilder() {
      assert(this);
      Out = Strm.str();
    }

    /* TODO */
    template <typename TVal>
    void Write(const TVal &val) {
      assert(this);
      Strm << val;
    }

    private:

    /* TODO */
    std::string &Out;

    /* TODO */
    std::ostringstream Strm;

  };  // TStringBuilder

  /* TODO */
  template <typename TVal>
  Starsha::TStringBuilder &&operator<<(Starsha::TStringBuilder &&string_builder, const TVal &val) {
    string_builder.Write(val);
    return std::move(string_builder);
  }

  /* TODO */
  template <typename TVal>
  Starsha::TStringBuilder &operator<<(Starsha::TStringBuilder &string_builder, const TVal &val) {
    string_builder.Write(val);
    return string_builder;
  }

}  // Starsha