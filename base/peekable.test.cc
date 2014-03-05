/* <base/peekable.test.cc>

   Unit test for <base/peekable.h>.

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

#include <base/peekable.h>

#include <test/kit.h>

using namespace std;
using namespace placeholders;
using namespace Base;

class TProducer {
  public:

  TProducer()
      : Val(0) {}

  bool Next(int &val) {
    bool success = (Val < 3);
    if (success) {
      val = Val++;
    }
    return success;
  }

  private:

  int Val;

};

FIXTURE(Typical) {
  TProducer producer;
  TPeekable<int> peekable(bind(&TProducer::Next, &producer, _1));
  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(peekable);
    for (int j = 0; j < 3; ++j) {
      EXPECT_EQ(*peekable, i);
    }
    ++peekable;
  }
  EXPECT_FALSE(peekable);
}
