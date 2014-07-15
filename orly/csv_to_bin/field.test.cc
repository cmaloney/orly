/* <orly/csv_to_bin/field.test.cc>

   Unit test for <orly/csv_to_bin/field.h>.

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

#include <orly/csv_to_bin/field.h>

#include <iostream>  // TODO

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Base::Chrono;
using namespace Orly::CsvToBin;

/* A simple geo-location type.. */
class TGeo final
    : public TObj {
  NO_COPY(TGeo);
  public:

  /* Default to 0-0. */
  TGeo()
      : Lat(0), Lon(0) {}

  /* Some fields to play with. */
  double Lat, Lon;

  /* Required by TObj. */
  virtual const TAnyFields &GetFields() const override {
    static const TFields<TGeo> fields {
      NEW_FIELD(TGeo, Lat),
      NEW_FIELD(TGeo, Lon)
    };
    return fields;
  }

};  // TGeo

/* A structure to play with. */
class TFoo final
    : public TObj {
  NO_COPY(TFoo);
  public:

  /* Initialize the fields to known values, so we can tell when we've
     changed them. */
  TFoo()
      : A(false), B(0), C(0), D(), E() {}

  /* Some fields to play with. */
  bool A;
  int B;
  double C;
  string D;
  TUuid E;
  Chrono::TTimePnt F;
  TGeo G;
  vector<string> H;
  TOpt<int> P, Q;

  /* Required by TObj. */
  virtual const TAnyFields &GetFields() const override {
    static const TFields<TFoo> fields {
      NEW_FIELD(TFoo, A),
      NEW_FIELD(TFoo, B),
      NEW_FIELD(TFoo, C),
      NEW_FIELD(TFoo, D),
      NEW_FIELD(TFoo, E),
      NEW_FIELD(TFoo, F),
      NEW_FIELD(TFoo, G),
      NEW_FIELD(TFoo, H),
      NEW_FIELD(TFoo, P),
      NEW_FIELD(TFoo, Q)
    };
    return fields;
  }

};  // TFoo

//ostream &operator<<(ostream &strm, const TTimePnt &) { return strm; }

FIXTURE(Typical) {
  TFoo foo;
  TranslateJson(foo, TJson::TObject {
      { "A", true }, { "B", 101 }, { "C", 98.6 }, { "D", "hello"},
      { "E", "1b4e28ba-2fa1-11d2-883f-b9a761bde3fb" },
      { "F", "Mon Jul 14 15:30:10 +0000 2014" },
      { "G", TJson::TObject { { "Lat", 12.34 }, { "Lon", 56.78 } } },
      { "H", TJson::TArray { "continue", "yesterday", "tomorrow" } },
      { "P", TJson() },
      { "Q", 19380 } });
  EXPECT_TRUE(foo.A);
  EXPECT_EQ(foo.B, 101);
  EXPECT_EQ(foo.C, 98.6);
  EXPECT_EQ(foo.D, "hello");
  EXPECT_EQ(foo.E, TUuid("1b4e28ba-2fa1-11d2-883f-b9a761bde3fb"));
  EXPECT_TRUE(foo.F == CreateTimePnt(2014, 7, 14, 15, 30, 10, 0, 0));
  EXPECT_EQ(foo.G.Lat, 12.34);
  EXPECT_EQ(foo.G.Lon, 56.78);
  if (EXPECT_EQ(foo.H.size(), 3u)) {
    EXPECT_EQ(foo.H[0], "continue");
    EXPECT_EQ(foo.H[1], "yesterday");
    EXPECT_EQ(foo.H[2], "tomorrow");
  }
  EXPECT_FALSE(foo.P);
  if (EXPECT_TRUE(foo.Q)) {
    EXPECT_EQ(*foo.Q, 19380);
  }
}
