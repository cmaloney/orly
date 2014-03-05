/* <rpc/unpack.test.cc>

   Unit test for <rpc/unpack.h>.

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

#include <rpc/unpack.h>

#include <string>

#include <test/kit.h>

using namespace std;
using namespace Rpc;

class TMyContext {
  public:

  TMyContext()
      : PingCount(0) {}

  int Add(int a, int b) {
    return a + b;
  }

  int AddGalois(int a, int b, int m) {
    return (a + b) % m;
  }

  void AdjPing1(int a) {
    PingCount += a;
  }

  void AdjPing2(int a, int b) {
    PingCount += a + b;
  }

  void AdjPing3(int a, int b, int c) {
    PingCount += a + b + c;
  }

  int Neg(int a) {
    return -a;
  }

  char GetNthChar(const string &text, int idx) {
    return text.at(idx);
  }

  int GetPingCount() {
    return PingCount;
  }

  void Ping() {
    ++PingCount;
  }

  private:

  int PingCount;

};

FIXTURE(Typical) {
  TMyContext context;
  EXPECT_EQ(Unpack(&TMyContext::GetPingCount, &context, make_tuple()), 0);            // 0, ret
  EXPECT_EQ(Unpack(&TMyContext::Neg, &context, make_tuple(101)), -101);               // 1, ret
  EXPECT_EQ(Unpack(&TMyContext::Add, &context, make_tuple(101, 202)), 303);           // 2, ret
  EXPECT_EQ(Unpack(&TMyContext::AddGalois, &context, make_tuple(101, 202, 300)), 3);  // 3, ret
  Unpack(&TMyContext::Ping, &context, make_tuple());                                  // 0, no ret
  Unpack(&TMyContext::AdjPing1, &context, make_tuple(5));                             // 1, no ret
  Unpack(&TMyContext::AdjPing2, &context, make_tuple(5, 10));                         // 2, no ret
  Unpack(&TMyContext::AdjPing3, &context, make_tuple(5, 10, 15));                     // 3, no ret
  EXPECT_EQ(Unpack(&TMyContext::GetPingCount, &context, make_tuple()), 51);
  EXPECT_EQ(Unpack(&TMyContext::GetNthChar, &context, make_tuple(string("mofo"), 2)), 'f');
}
