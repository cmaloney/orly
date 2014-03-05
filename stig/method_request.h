/* <stig/method_request.h>

   The request sent to Stig when a client wishes to call a method.

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
#include <vector>

#include <base/opt.h>
#include <base/uuid.h>
#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>
#include <stig/closure.h>

namespace Stig {

  /* The request sent to Stig when a client wishes to call a method. */
  class TMethodRequest {
    public:

    /* We use 128-bit ids. */
    using TId = Base::TUuid;

    /* The fully-qualified name of a package.  The last element in the vector is the unqualified name of the package
       and the elements preceding it give the full path from the package root.  It is never correct for this vector
       to be empty. */
    using TPackage = std::vector<std::string>;

    /* We characterize time-to-live in milliseconds. */
    using TTimeToLive = std::chrono::milliseconds;

    /* TODO */
    TMethodRequest() = default;

    /* TODO */
    TMethodRequest(const TId &pov_id, const TPackage &package, const TClosure &closure);

    /* TODO */
    TMethodRequest(const TId &pov_id, const TPackage &package, const TClosure &closure, const TTimeToLive &time_to_live);

    /* TODO */
    TMethodRequest(const TId &pov_id, const TPackage &package, const TClosure &closure, const TId &tracking_id);

    /* A method name and set of arguments which the server is to execute. */
    const TClosure &GetClosure() const {
      assert(this);
      return Closure;
    }

    /* The full package to the package in which the requested method is to be found.
       This will always end with the name of the package itself and so will never be an empty vector. */
    const TPackage &GetPackage() const {
      assert(this);
      return Package;
    }

    /* The id of the private pov in which to run the method. */
    const TId &GetPovId() const {
      assert(this);
      return PovId;
    }

    /* If the method is to be evaluated as of a specific moment in the past, this will be the id of that moment.
       If the method is to be run in the 'now', this will be unknown.
       If the method is to be run in the past, it cannot generate an update; therefore, if this value is known,
       time-to-live must be zero. */
    const Base::TOpt<TId> &GetTrackingId() const {
      assert(this);
      return TrackingId;
    }

    /* This is the amount of time that the result of this method call will remain trackable.
       If the method has no side-effects but this value is non-zero, then the method will
       generate an empty update, just for the purpose of tracking.
       If the method is running in the past, then this must be zero. */
    const TTimeToLive &GetTimeToLive() const {
      assert(this);
      return TimeToLive;
    }

    /* Stream in. */
    void Read(Io::TBinaryInputStream &strm);

    /* Stream out. */
    void Write(Io::TBinaryOutputStream &strm) const;

    private:

    /* Are we in a good state? */
    bool IsValid() const;

    /* See accessor. */
    TId PovId;

    /* See accessor. */
    TTimeToLive TimeToLive;

    /* See accessor. */
    Base::TOpt<TId> TrackingId;

    /* See accessor. */
    TPackage Package;

    /* See accessor. */
    TClosure Closure;

  };  // TMethodRequest

  /* Binary stream extractor for Stig::TMethodRequest. */
  inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, Stig::TMethodRequest &that) {
    assert(&that);
    that.Read(strm);
    return strm;
  }

  /* Binary stream inserter for Stig::TMethodRequest. */
  inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const Stig::TMethodRequest &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

}  // Stig