/* <base/indent.test.cc>

   Unit test for <base/indent.h>.

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


#include <base/indent.h>

#include <sstream>

#include <test/kit.h>

using namespace Base;

FIXTURE(Test1) {
  std::string x_rated;
  TIndent ind1(x_rated, TIndent::StartAt::Indented, 3, 'x');
  EXPECT_EQ(ind1.Get(), "xxx");
  std::ostringstream oss1;
  oss1 << ind1;
  std::string s = oss1.str();
  EXPECT_EQ(s, "xxx");

  {
    TIndent ind2(ind1);
    EXPECT_EQ(ind2.Get(), "xxxxxx");

    {
      TIndent ind3(ind2, 4, 'y');
      EXPECT_EQ(ind3.Get(), "xxxxxxyyyy");

      {
        TIndent ind4(ind3);
        EXPECT_EQ(ind4.Get(), "xxxxxxyyyyyyyy");
        ind4.AddOnce("zz");
        EXPECT_EQ(ind4.Get(), "xxxxxxyyyyyyyyzz");

        {
          TIndent ind5(ind4);
          EXPECT_EQ(ind5.Get(), "xxxxxxyyyyyyyyzzyyyy");

          {
            TIndent ind6(ind5);
            EXPECT_EQ(ind6.Get(), "xxxxxxyyyyyyyyzzyyyyyyyy");
            ind6.AddOnce("splat");
            EXPECT_EQ(ind6.Get(), "xxxxxxyyyyyyyyzzyyyyyyyysplat");

            {
              TIndent ind7(ind6);
              EXPECT_EQ(ind6.Get(), "xxxxxxyyyyyyyyzzyyyyyyyysplatyyyy");
            }

            EXPECT_EQ(ind6.Get(), "xxxxxxyyyyyyyyzzyyyyyyyysplat");
          }

          EXPECT_EQ(ind5.Get(), "xxxxxxyyyyyyyyzzyyyy");
        }

        EXPECT_EQ(ind4.Get(), "xxxxxxyyyyyyyyzz");
      }

      EXPECT_EQ(ind3.Get(), "xxxxxxyyyy");
    }

    EXPECT_EQ(ind2.Get(), "xxxxxx");
  }

  EXPECT_EQ(ind1.Get(), "xxx");
}

FIXTURE(StartAtZeroTest) {
  std::string x_rated;
  TIndent ind1(x_rated, TIndent::StartAt::Zero, 3, 'x');
  EXPECT_EQ(ind1.Get(), "");
  std::ostringstream oss1;
  oss1 << ind1;
  std::string s = oss1.str();
  EXPECT_EQ(s, "");

  {
    TIndent ind2(ind1);
    EXPECT_EQ(ind2.Get(), "xxx");

    {
      TIndent ind3(ind2, 4, 'y');
      EXPECT_EQ(ind3.Get(), "xxxyyyy");
    }

    EXPECT_EQ(ind2.Get(), "xxx");
  }

  EXPECT_EQ(ind1.Get(), "");
}