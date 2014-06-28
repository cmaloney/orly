/* <orly/package/name.test.cc>

   Unit test for <orly/package/name.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/package/name.h>

#include <test/kit.h>

#include <base/as_str.h>

using namespace std;
using namespace Base;
using namespace Orly::Package;

/* Expected kinds of input:
   "scope/A.1"
   "scope/A"
   "A.1"
   "A"
*/

//TODO: Nowhere near all of package_name.cc is tested here.
FIXTURE(VersionedNameWithScope) {
  const TVersionedName package = TVersionedName::Parse(AsPiece("scope/sample.1"));
  EXPECT_EQ(package.Name, (TName{{"scope", "sample"}}));
  EXPECT_EQ(package.Version, 1u);
  EXPECT_EQ(AsStr(package.GetSoRelPath()), "scope/sample.1.so");
}

FIXTURE(NameWithScope) {
  EXPECT_EQ(TName::Parse("scope/sample"), (TName{{"scope", "sample"}}));
}

FIXTURE(VersionedNameWithoutScope) {
  const TVersionedName package = TVersionedName::Parse(AsPiece("sample.1000"));
  EXPECT_TRUE(package.Name == TName{{"sample"}});
  EXPECT_EQ(package.Version, 1000u);
  EXPECT_EQ(AsStr(package.GetSoRelPath()), "sample.1000.so");
}
