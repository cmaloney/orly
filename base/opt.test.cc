/* <base/opt.test.cc>

   Unit test for <base/opt.h>.

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

#include <base/opt.h>

#include <string>

#include <base/no_copy_semantics.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

class TToken {
  public:

  TToken()
      : Dummy(0) {
    Push('D');
  }

  TToken(TToken &&that)
      : Dummy(that.Dummy) {
    Push('M');
  }

  TToken(const TToken &that)
      : Dummy(that.Dummy) {
    Push('C');
  }

  ~TToken() {
    Push('X');
  }

  TToken &operator=(TToken &&that) {
    Push('S');
    swap(Dummy, that.Dummy);
    return *this;
  }

  TToken &operator=(const TToken &that) {
    Push('A');
    return *this = TToken(that);
  }

  static string LatchOps() {
    string temp = Ops;
    Ops.clear();
    return temp;
  }

  int Dummy;

  private:

  static void Push(char op) {
    Ops.push_back(op);
  }

  static string Ops;

};

string TToken::Ops;

ostream &operator<<(ostream &strm, const TToken &that) {
  return strm << "TToken(" << that.Dummy << ')';
}

FIXTURE(KnownVsUnknown) {
  TOpt<int> opt;
  EXPECT_FALSE(opt);
  EXPECT_FALSE(opt.IsKnown());
  EXPECT_TRUE(opt.IsUnknown());
  opt = 0;
  EXPECT_TRUE(opt);
  EXPECT_TRUE(opt.IsKnown());
  EXPECT_FALSE(opt.IsUnknown());
  opt.Reset();
  EXPECT_FALSE(opt);
  EXPECT_FALSE(opt.IsKnown());
  EXPECT_TRUE(opt.IsUnknown());
  EXPECT_FALSE(*opt.Unknown);
}

FIXTURE(TokenBasics) {
  /* extra */ {
    TToken a;
    EXPECT_EQ(TToken::LatchOps(), string("D"));
    TToken b = a;
    EXPECT_EQ(TToken::LatchOps(), string("C"));
    TToken c = move(b);
    EXPECT_EQ(TToken::LatchOps(), string("M"));
    a = move(b);
    EXPECT_EQ(TToken::LatchOps(), string("S"));
    a = c;
    EXPECT_EQ(TToken::LatchOps(), string("ACSX"));
  }
  EXPECT_EQ(TToken::LatchOps(), string("XXX"));
}

FIXTURE(MoveAndCopy) {
  TToken token;
  TOpt<TToken> a, b = token, c = move(b);
  EXPECT_EQ(TToken::LatchOps(), string("DCMX"));
  EXPECT_FALSE(a);
  EXPECT_FALSE(b);
  EXPECT_TRUE(c);
  a = move(c);
  EXPECT_EQ(TToken::LatchOps(), string("MX"));
  EXPECT_TRUE(a);
  EXPECT_FALSE(c);
  b = a;
  EXPECT_EQ(TToken::LatchOps(), string("CMX"));
  EXPECT_TRUE(a);
  EXPECT_TRUE(b);
  TOpt<TToken> d = b;
  EXPECT_EQ(TToken::LatchOps(), string("C"));
  EXPECT_TRUE(d);
  d.MakeKnown();
  EXPECT_EQ(TToken::LatchOps(), string(""));
  EXPECT_TRUE(d);
  c.MakeKnown();
  EXPECT_EQ(TToken::LatchOps(), string("D"));
  EXPECT_TRUE(c);
}
