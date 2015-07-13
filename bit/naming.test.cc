#include <bit/naming.h>

#include <test/kit.h>

using namespace Bit;
using namespace std;

#define EXPECT_INVALID(path) \
  EXPECT_THROW_AT(HERE, TInvalidPath, []() { TTree(path); })

FIXTURE(TreeValidity) {
  EXPECT_INVALID("");
  EXPECT_INVALID("abc");
  EXPECT_INVALID("abc/");
  EXPECT_INVALID("a/b/c/");

  EXPECT_EQ(TTree("/").Path, "/");
  EXPECT_EQ(TTree("/a").Path, "/a/");
  EXPECT_EQ(TTree("/a").Path, TTree("/a/").Path);
  EXPECT_EQ(TTree("/a/b/c").Path, TTree("/a/b/c/").Path);
  TTree("/a/b/c/d/e/");
}
