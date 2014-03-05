/* <starsha/note.test.cc>

   Unit test for <starsha/note.h>.

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

#include <starsha/note.h>

#include <iostream>
#include <sstream>

#include <test/kit.h>

using namespace std;
using namespace Starsha;

FIXTURE(ReadWriteRoundTrip) {
  string original;
  /* extra */ {
    TNote root("hal/9000");
    new TNote(&root, "hello", "I\nhave\nlines\n");
    new TNote(&root, "doctor", "I want \x01 hex.");
    new TNote(&root, "name");
    ostringstream strm;
    root.Write(strm);
    original = strm.str();
  }
  TNote *root;
  /* extra */ {
    istringstream strm(original);
    root = TNote::TryRead(strm);
  }
  string clone;
  if (EXPECT_TRUE(root)) {
    try {
      ostringstream strm;
      root->Write(strm);
      clone = strm.str();
    } catch (...) {
      delete root;
      throw;
    }
    delete root;
    root = 0;
  }
  EXPECT_EQ(original, clone);
}
