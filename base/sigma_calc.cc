/* <base/sigma_calc.cc>

   Implements <base/sigma_calc.h>.

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

#include <base/sigma_calc.h>

#include <algorithm>
#include <cmath>

using namespace std;
using namespace Base;

void TSigmaCalc::Push(double val) {
  assert(this);
  if (Count) {
    Min = min(Min, val);
    Max = max(Max, val);
    double
        old_moving_mean   = MovingMean,
        old_moving_square = MovingSquare;
    MovingMean   = old_moving_mean   + (val - old_moving_mean) / static_cast<double>(Count + 1);
    MovingSquare = old_moving_square + (val - old_moving_mean) * (val - MovingMean);
  } else {
    Min          = val;
    Max          = val;
    MovingMean   = val;
    MovingSquare = 0;
  }
  ++Count;
}

size_t TSigmaCalc::Report(double &min, double &max, double &mean, double &sigma) const {
  assert(this);
  assert(&min);
  assert(&max);
  assert(&mean);
  assert(&sigma);
  if (Count) {
    min   = Min;
    max   = Max;
    mean  = MovingMean;
    sigma = (Count > 1) ? sqrt(MovingSquare / static_cast<double>(Count - 1)) : 0;
  }
  return Count;
}

ostream &Base::operator<<(ostream &strm, const TSigmaCalc &that) {
  assert(&strm);
  assert(&that);
  double min, max, mean, sigma;
  size_t count = that.Report(min, max, mean, sigma);
  strm << "(count: " << count;
  if (count) {
    strm << ", min: " << min << ", max: " << max << ", mean: " << mean << " +/- " << sigma;
  }
  return strm << ')';
}
