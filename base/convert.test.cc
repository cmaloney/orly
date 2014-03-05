/* <base/convert.test.cc>

   Unit test for <base/convert.h>

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

#include <base/convert.h>

#include <test/kit.h>

#include <climits> //TODO
#include <iostream> //TODO

using namespace Base;

//TODO: Get to full coverage. Currently just a compile test.
FIXTURE(Int) {
  int i;
  TConverter(AsPiece("42")).ReadInt(i);
  EXPECT_EQ(i, 42);
}

FIXTURE(TypeLimits) {
  long i;
  TConverter(AsPiece("9223372036854775807")).ReadInt(i);
  EXPECT_EQ(i, 9223372036854775807l);
  TConverter(AsPiece("-9223372036854775808")).ReadInt(i);
  EXPECT_EQ(i, LONG_MIN);
}

FIXTURE(ProxyInt) {
  int i = TConvertProxy(AsPiece("123"));
  EXPECT_EQ(i, 123);
  size_t size = TConvertProxy(AsPiece("456"));
  EXPECT_EQ(size, 456ul);
}