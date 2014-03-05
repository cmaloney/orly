/* <stig/native/point.h>

   A simple record type we use for testing.

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

/* A simple record type we use for testing.
   It's in the global namespace on purpose. */
class TPoint {
  public:

  /* Default-construct at the origin. */
  TPoint()
      : X(0), Y(0) {}

  /* Copy the discrete coordinates. */
  TPoint(double x, double y)
      : X(x), Y(y) {}

  /* TODO */
  double GetX() const {
    return X;
  }

  /* TODO */
  double GetY() const {
    return Y;
  }

  private:

  /* Coordinates, maybe? */
  double X, Y;

};  // TPoint
