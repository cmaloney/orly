/* <base/tmp_file.test.cc>

   Unit test for <base/tmp_file.h>.

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


#include <base/tmp_file.h>

#include <test/kit.h>

using namespace Base;

FIXTURE(Test1) {
  std::string tmpl("/tmp/stig_general_XXXXXX.tmp");
  TTmpFile tmp_file(tmpl.c_str(), true);
  std::string name(tmp_file.GetName());
  EXPECT_EQ(tmpl.size(), name.size());
  EXPECT_NE(tmpl, name);
  EXPECT_EQ(name.substr(0, 18), std::string("/tmp/stig_general_"));
  EXPECT_NE(name.substr(18, 6), std::string("XXXXXX"));
  EXPECT_EQ(name.substr(24, 4), std::string(".tmp"));
}

FIXTURE(Test2) {
  std::string tmpl("XXXXXXburp");
  TTmpFile tmp_file(tmpl.c_str(), true);
  std::string name(tmp_file.GetName());
  EXPECT_EQ(tmpl.size(), name.size());
  EXPECT_NE(tmpl, name);
  EXPECT_NE(name.substr(0, 6), std::string("XXXXXX"));
  EXPECT_EQ(name.substr(6, 4), std::string("burp"));
}

FIXTURE(Test3) {
  std::string tmpl("burpXXXXXX");
  TTmpFile tmp_file(tmpl.c_str(), true);
  std::string name(tmp_file.GetName());
  EXPECT_EQ(tmpl.size(), name.size());
  EXPECT_NE(tmpl, name);
  EXPECT_EQ(name.substr(0, 4), std::string("burp"));
  EXPECT_NE(name.substr(4, 6), std::string("XXXXXX"));
}

FIXTURE(Test4) {
  std::string tmpl("burpXXXXXXyXXXXXbarf");
  TTmpFile tmp_file(tmpl.c_str(), true);
  std::string name(tmp_file.GetName());
  EXPECT_EQ(tmpl.size(), name.size());
  EXPECT_NE(tmpl, name);
  EXPECT_EQ(name.substr(0, 4), std::string("burp"));
  EXPECT_NE(name.substr(4, 6), std::string("XXXXXX"));
  EXPECT_EQ(name.substr(10, 10), std::string("yXXXXXbarf"));
}