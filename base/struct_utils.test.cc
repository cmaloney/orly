/* <base/struct_utils.test.cc>

   Unit test for <base/struct_utils.h>.

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

#include <base/struct_utils.h>

#include <test/kit.h>

using namespace Base;

/* The type of an ordinal. */
using TOrd = double;

/* A simple 2D point type with which we'll test. */
struct TPoint {

  /* A pair of ordinals. */
  TOrd X, Y;

};  // TPoint

FIXTURE(OffsetOf) {
  EXPECT_EQ(OFFSET_OF(TPoint, X), 0u);
  EXPECT_EQ(OFFSET_OF(TPoint, Y), sizeof(TOrd));
}

FIXTURE(ContainerOf) {
  // Make a point, p, and get the containers of its members.
  TPoint p = { 101, 202 };
  TPoint
      *cont_of_x = CONTAINER_OF(&(p.X), TPoint, X),
      *cont_of_y = CONTAINER_OF(&(p.Y), TPoint, Y);
  // The container of x is p.
  EXPECT_EQ(cont_of_x, &p);
  EXPECT_EQ(cont_of_x->X, p.X);
  EXPECT_EQ(cont_of_x->Y, p.Y);
  // The container of y is also p.
  EXPECT_EQ(cont_of_y, &p);
  EXPECT_EQ(cont_of_y->X, p.X);
  EXPECT_EQ(cont_of_y->Y, p.Y);
}
