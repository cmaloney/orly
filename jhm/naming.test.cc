/* <jhm/naming.test.cc>

   Unit test for <jhm/naming.h>

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

#include <jhm/naming.h>

#include <unistd.h>

#include <base/split.h>
#include <base/syntax_error.h>

#include <test/kit.h>

using namespace Base;
using namespace Jhm;
using namespace std;

namespace std {
  template <typename TVal>
  ostream &operator<<(ostream &strm, const vector<TVal> &vec) {
    assert(&strm);
    assert(&vec);

    strm << '{';
    Join(',', vec, strm) << '}';
    return strm;
  }
}
FIXTURE(Name) {
  //Check that general parsing and construction work.
  TName n1("a.txt");
  TName n1a("a", {"txt"});
  EXPECT_EQ(n1a.GetBase(), "a");
  EXPECT_EQ(n1a.GetExtensions(), TStrList({"txt"}));
  EXPECT_EQ(n1.GetBase(), "a");
  EXPECT_EQ(n1.GetExtensions(),  TStrList({"txt"}));
  EXPECT_EQ(n1, n1a);

  TName n2("a");
  TName n2a("a", {});
  EXPECT_EQ(n2, n2a);

  TName n3("a..b");
  TName n3a("a", {"","b"});
  EXPECT_EQ(n3, n3a);

  TName n4("a...b");
  TName n4a("a", {"","", "b"});
  EXPECT_EQ(n4, n4a);

  TName n5("a.b.c.d.e");
  TName n5a("a", {"b", "c", "d", "e"});
  EXPECT_EQ(n5, n5a);

  TName n6("a.b.c.d...");
  TName n6a("a", {"b", "c", "d", "", "", ""});
  EXPECT_EQ(n6, n6a);

  TName n7("a.");
  TName n7a("a", {""});
  EXPECT_EQ(n7, n7a);

  EXPECT_EQ(TName(".a"), TName("", {"a"}));
  EXPECT_EQ(TName("."), TName("", {""}));

  //Check non-equality
  EXPECT_NE(TName("a", {}), TName("b", {}));
  EXPECT_NE(TName("b.c"), TName("b.d"));

  //Swaping extensions
  TName n8("a.b");
  TName n8a("a.a");
  EXPECT_EQ(n8a.SwapLastExtension("b"), n8);
  EXPECT_EQ(n8a, TName("a.a"));
  EXPECT_EQ(n8a.SwapLastExtension("b").SwapLastExtension("a2"), TName("a.a2"));


  //Check the const ref constructor works the same as the move constructor
  TStr base = "a";
  TStrList extensions({"b", "c", "d", "", "", ""});
  TName n6b(base, extensions);
  EXPECT_EQ(n6a, n6b);

  //Check that copy and move construction work
  TName n1b(n1);
  EXPECT_EQ(n1a, n1b);
  TName n1c(move(n1a));
  EXPECT_EQ(n1b, n1c);

  //Make sure something invalid is invalid with an exception.
  EXPECT_THROW(TSyntaxError, [](){ TName n2("a/b.c"); });
  EXPECT_THROW(TSyntaxError, [](){ TName n2("a.b/c.d"); });
}

FIXTURE(Namespace) {
  TNamespace ns1("a/b/c/d");
  TNamespace ns1a({"a", "b", "c", "d"});
  EXPECT_EQ(ns1.Get (), TStrList({"a", "b", "c", "d"}));
  EXPECT_EQ(ns1, ns1a);

  EXPECT_THROW(TSyntaxError, [](){ TNamespace("/a"); });
  EXPECT_THROW(TSyntaxError, [](){ TNamespace("a/"); });
  EXPECT_THROW(TSyntaxError, [](){ TNamespace("/a/"); });

  TNamespace ns3("za");
  TNamespace ns3a(TStrList({"za"}));
  EXPECT_EQ(ns3, ns3a);

  TNamespace ns5("a.b/c.d/e");
  TNamespace ns5a(TStrList({"a.b", "c.d", "e"})); //Use a TStrList to prevent a GCC ICE.
  EXPECT_EQ(ns5, ns5a);


  //Check non-equality
  EXPECT_NE(TNamespace("a/b"), TNamespace("b"));
  EXPECT_NE(TNamespace("a/b"), TNamespace("a/c"));
  EXPECT_NE(TNamespace("a/b"), TNamespace("a/b/d"));
}

FIXTURE(RelPath) {
  TRelPath rp1("a/b/c/d.bar.baz");
  TNamespace ns1(TStrList({"a","b","c"}));
  TName n1("d", {"bar","baz"});
  TRelPath rp1a(ns1, n1);
  EXPECT_EQ(rp1.GetNamespace(), ns1);
  EXPECT_EQ(rp1.GetName(), n1);
  EXPECT_EQ(rp1, rp1a);

  TRelPath rp2("d.bar.baz");
  TRelPath rp3("a/d");
  TRelPath rp4(".a/.b");

  EXPECT_THROW(TSyntaxError, []() { TRelPath("a/b/../c"); });
  EXPECT_THROW(TSyntaxError, []() { TRelPath("a/./b"); });
  EXPECT_THROW(TSyntaxError, []() { TRelPath("/./"); });
  EXPECT_THROW(TSyntaxError, []() { TRelPath("b/../"); });
  EXPECT_THROW(TSyntaxError, []() { TRelPath("b/c/../d/../../e/f/./g/../h"); });
  EXPECT_THROW(TSyntaxError, []() { TRelPath("a/../../"); });
  EXPECT_THROW(TSyntaxError, []() { TRelPath("/../"); });
  EXPECT_THROW(TSyntaxError, []() { TRelPath("a/b/"); });

}
FIXTURE(AbsBase) {
  TAbsBase ab1("/a/b/c/");
  TAbsBase ab2("/b/d/");
  TAbsBase ab3("e");
  TAbsBase ab4("");
  TAbsBase ab5("/");
  EXPECT_EQ(ab1.Get(), TStr("a/b/c"));
  EXPECT_EQ(ab2.Get(), TStr("b/d"));

  std::unique_ptr<char> cwd_cstr(get_current_dir_name());
  string cwd(cwd_cstr.get()+1);

  EXPECT_EQ(ab3.Get(), cwd + "/e");
  EXPECT_EQ(ab4.Get(), cwd);
  EXPECT_EQ(ab5.Get(), TStr(""));
  EXPECT_NE(ab1, TAbsBase("a/b/c"));
  EXPECT_NE(ab2, TAbsBase("b/d"));
  EXPECT_NE(ab1, ab2);

  /* Programmer errors. These should be cleaned out before entering TAbsBase.
  EXPECT_THROW(TSyntaxError, []() { TAbsBase("//"); });
  EXPECT_THROW(TSyntaxError, []() { TAbsBase("/../"); });
  EXPECT_THROW(TSyntaxError, []() { TAbsBase("a/b//c/"); });
  EXPECT_THROW(TSyntaxError, []() { TAbsBase("a/b/./c/"); });
  EXPECT_THROW(TSyntaxError, []() { TAbsBase("a/b/../c/"); });
  TAbsBase("a/c/..d/");
  TAbsBase("a/c/.d/");
  */
}

FIXTURE(AbsPath) {
  TAbsPath ap1(TAbsBase("/a/b/c"), TRelPath("a/b.c"));
  TAbsPath ap1a(TAbsBase("/a/b/c"), TRelPath("a/b.c"));
  TAbsPath ap2(TAbsBase("a/b"), TRelPath("c/a/b.c"));

  EXPECT_EQ(ap1, ap1a);
  EXPECT_NE(ap1, ap2);

  std::unique_ptr<char> cwd_cstr(get_current_dir_name());
  string cwd(cwd_cstr.get());

  EXPECT_EQ(ap1.AsStr(), "/a/b/c/a/b.c");
  EXPECT_EQ(ap2.AsStr(), cwd + "/a/b/c/a/b.c");
}