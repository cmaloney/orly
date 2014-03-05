/* <stig/package/name.test.cc>

   Unit test for <stig/package/name.h>.

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

#include <stig/package/name.h>

#include <test/kit.h>

using namespace Base;
using namespace Stig::Package;

/* Expected kinds of input:
   "scope/A.1"
   "scope/A"
   "A.1"
   "A"
*/

//TODO: Nowhere near all of package_name.cc is tested here.
FIXTURE(VersionedNameWithScope) {
  const TVersionedName package = TVersionedName::Parse(AsPiece("scope/sample.1"));

  EXPECT_EQ(package.Name, TName(Jhm::TStrList{"scope", "sample"}));
  EXPECT_EQ(package.Version, 1u);
  EXPECT_EQ(package.GetSoRelPath().AsStr(), std::string("scope/sample.1.so"));
}

FIXTURE(NameWithScope) {
  const TName package("scope/sample");
  EXPECT_EQ(package, TName(Jhm::TStrList{"scope", "sample"}));
}

FIXTURE(VersionedNameWithoutScope) {
  const TVersionedName package = TVersionedName::Parse(AsPiece("sample.1000"));
  EXPECT_TRUE(package.Name == TName(Jhm::TStrList{"sample"}));
  EXPECT_EQ(package.Version, 1000u);
  EXPECT_EQ(package.GetSoRelPath().AsStr(), std::string("sample.1000.so"));
}

FIXTURE(NameWithSlashFollowingPiece) {
  Base::TPiece<const char> piece = AsPiece("database/");
  piece.AdjustLimit(-1);
  TName package(piece);
  EXPECT_EQ(package, TName(Jhm::TStrList{"database"}));
}