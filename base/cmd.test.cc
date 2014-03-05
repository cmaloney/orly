/* <base/cmd.test.cc>

   Unit test for <base/cmd.h>.

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

#include <base/cmd.h>

#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

#include <base/no_copy_semantics.h>

#include <test/kit.h>

using namespace std;

class TShape
    : public Base::TCmd {
  public:

  bool IsFilled, IsOutlined;

  double LineWeight;

  protected:

  TShape()
      : IsFilled(false), IsOutlined(false), LineWeight(1.2) {}

  class TMeta
      : public TCmd::TMeta {
    protected:

    TMeta(const char *desc)
        : TCmd::TMeta(desc) {
      Param(&TShape::IsFilled, "is_filled", Optional, "filled\0", "The shape should be drawn filled-in.");
      Param(&TShape::IsOutlined, "is_outlined", Optional, "outlined\0", "The shape should be drawn with an outline.");
      Param(&TShape::LineWeight, "line_weight", Optional, "line_weight\0lw\0", "The weight of the line used to outline, in points.");
    }

  };

};

class TCircle
    : public TShape {
  public:

  TCircle(int argc, char *argv[])
      : Radius(1.0) {
    Parse(argc, argv, TMeta());
  }

  double Radius;

  private:

  class TMeta
      : public TShape::TMeta {
    public:

    TMeta()
        : TShape::TMeta("Draw a circle.") {
      Param(&TCircle::Radius, "radius", Required, "The radius of the circle.");
    }

  };

};

class TArgs {
  NO_COPY_SEMANTICS(TArgs);
  public:

  TArgs(initializer_list<const char *> strs) {
    try {
      for (const char *str: strs) {
        char *dup = strdup(str);
        try {
          Strs.push_back(dup);
        } catch (...) {
          free(dup);
          throw;
        }
      }
      Strs.push_back(nullptr);
    } catch (...) {
      Cleanup();
      throw;
    }
  }

  ~TArgs() {
    Cleanup();
  }

  int GetArgc() const {
    return Strs.size() - 1;
  }

  char **GetArgv() const {
    return const_cast<char **>(&Strs[0]);
  }

  private:

  void Cleanup() {
    for (char *str: Strs) {
      free(str);
    }
  }

  vector<char *> Strs;

};

FIXTURE(Typical) {
  TArgs args({ "a/b/c/prog", "2.5" });
  TCircle circle(args.GetArgc(), args.GetArgv());
  EXPECT_EQ(string(circle.GetDir()), string("a/b/c"));
  EXPECT_EQ(string(circle.GetProg()), string("prog"));
  EXPECT_FALSE(circle.IsFilled);
  EXPECT_EQ(circle.Radius, 2.5);
}

FIXTURE(ArgsFile) {
  TArgs args({ "a/b/c/prog", "--args=" SRC_ROOT "base/cmd.test.args", "2.5" });
  TCircle circle(args.GetArgc(), args.GetArgv());
  EXPECT_TRUE(circle.IsFilled);
  EXPECT_TRUE(circle.IsOutlined);
  EXPECT_EQ(circle.LineWeight, 3.0);
  EXPECT_EQ(circle.Radius, 2.5);
}
