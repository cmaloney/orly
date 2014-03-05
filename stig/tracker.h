/* <stig/tracker.h>

   A value returned by Stig which allows users to track thre result of a method call.

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
#include <chrono>

#include <base/uuid.h>
#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>

namespace Stig {

  /* A value returned by Stig which allows users to track thre result of a method call. */
  class TTracker {
    public:

    /* TODO */
    TTracker() {}

    /* TODO */
    TTracker(const Base::TUuid &id, std::chrono::seconds ttl) : Id(id), TimeToLive(ttl) {}

    /* The id used to track. */
    Base::TUuid Id;

    /* The minimum amount of time the id will be good. */
    std::chrono::seconds TimeToLive;

  };  // TTracker

  /* Binary stream extractor for Stig::TTracker. */
  inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TTracker &that) {
    assert(&strm);
    assert(&that);
    return strm >> that.Id >> that.TimeToLive;
  }

  /* Binary stream inserter for Stig::TTracker. */
  inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TTracker &that) {
    assert(&strm);
    assert(&that);
    return strm << that.Id << that.TimeToLive;
  }

}  // Stig