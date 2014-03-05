/* <stig/method_result.h>

   The result returned by Stig when a client runs a method.

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
#include <memory>

#include <base/opt.h>
#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>
#include <stig/tracker.h>
#include <stig/atom/kit2.h>

namespace Stig {

  /* The result returned by Stig when a client runs a method. */
  class TMethodResult {
    public:

    /* Pure laziness. */
    using TArena = Atom::TCore::TArena;

    /* Default-constructs a void value and unknown tracker. */
    TMethodResult() {}

    /* Shares the given arena and shallow-copies the core.  Sets the tracker. */
    TMethodResult(
        const std::shared_ptr<TArena> &arena, const Atom::TCore &value, const Base::TOpt<TTracker> &tracker,
        bool is_error = false);

    /* Constructs a new arena and deep-copies the core into it.  Sets the tracker. */
    TMethodResult(TArena *arena, const Atom::TCore &value, const Base::TOpt<TTracker> &tracker, bool is_error = false);

    /* The arena in which our value lives. */
    const std::shared_ptr<TArena> &GetArena() const {
      assert(this);
      return Arena;
    }

    /* The tracker of this result.
       If the method had no side-effects and no time tracking was requested, this will be unknown. */
    const Base::TOpt<TTracker> &GetTracker() const {
      assert(this);
      return Tracker;
    }

    /* The value computed by the method. */
    const Atom::TCore &GetValue() const {
      assert(this);
      return Value;
    }

    /* If true, the value contained here is an error; otherwise, it is a normal return. */
    bool IsError() const {
      assert(this);
      return Error;
    }

    /* Stream in. */
    void Read(Io::TBinaryInputStream &strm);

    /* Reset to the default-constructed state. */
    void Reset();

    /* Stream out. */
    void Write(Io::TBinaryOutputStream &strm) const;

    private:

    /* See accessor. */
    std::shared_ptr<TArena> Arena;

    /* See accessor. */
    Atom::TCore Value;

    /* See accessor. */
    Base::TOpt<TTracker> Tracker;

    /* See accessor. */
    bool Error;

  };  // TMethodResult

  /* Binary stream extractor for Stig::TMethodResult. */
  inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TMethodResult &that) {
    assert(&that);
    that.Read(strm);
    return strm;
  }

  /* Binary stream inserter for Stig::TMethodResult. */
  inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TMethodResult &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

}  // Stig