/* <base/sigma_calc.h>

   Compute min, max, mean, and standard deviation of a series of values.

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
#include <cstddef>
#include <ostream>

namespace Base {

  /* Compute min, max, mean, and standard deviation of a series of values.
     This class uses Welford's method (as cited in Knuth) to perform the computations incrementally.
     It therefore doesn't store the individual values and is reasonably fast. */
  class TSigmaCalc {
    public:

    /* Start out empty. */
    TSigmaCalc()
        : Count(0) {}

    /* Push a value into the calculator. */
    void Push(double val);

    /* If Push() has been called at least once, return the stats via the out-params and return the
       number of pushes; otherwise, leave the out-params alone and return zero. */
    size_t Report(double &min, double &max, double &mean, double &sigma) const;

    /* TODO */
    inline size_t GetCount() const;

    /* TODO */
    inline double GetMin() const;

    /* TODO */
    inline double GetMax() const;

    /* TODO */
    inline double GetMean() const;

    /* TODO */
    inline double GetSigma() const;

    /* Go back to being empty. */
    void Reset() {
      assert(this);
      Count = 0;
    }

    private:

    /* The number of times Push() has been called. */
    size_t Count;

    /* These values are only valid when Count > 0. */
    double Min, Max, MovingMean, MovingSquare;

  };  // TSigmaCalc

  /***************
    *** Inline ***
    *************/

  inline size_t TSigmaCalc::GetCount() const {
    return Count;
  }

  inline double TSigmaCalc::GetMin() const {
    return Count ? Min : 0;
  }

  inline double TSigmaCalc::GetMax() const {
    return Count ? Max : 0;
  }

  inline double TSigmaCalc::GetMean() const {
    return Count ? MovingMean : 0;
  }

  inline double TSigmaCalc::GetSigma() const {
    return Count ? sqrt(MovingSquare / static_cast<double>(Count - 1)) : 0;
  }

  /* Write a string like this (count: a, min: b, max: c, mean: d +/- e). */
  std::ostream &operator<<(std::ostream &strm, const TSigmaCalc &that);

}  // Base