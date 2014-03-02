/* <stig/rpc/type/cmp.test.cc> 

   Unit test for <stig/rpc/type/cmp.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/type/cmp.h>

#include <stig/rpc/all_types.h>
#include <stig/rpc/type/dump.h>
#include <test/kit.h>

using namespace std;
using namespace Stig::Rpc;

FIXTURE(Monomorphs) {
  EXPECT_EQ(Type::TId      (), Type::TId      ());
  EXPECT_EQ(Type::TInt     (), Type::TInt     ());
  EXPECT_EQ(Type::TReal    (), Type::TReal    ());
  EXPECT_EQ(Type::TStr     (), Type::TStr     ());
  EXPECT_EQ(Type::TTimePnt (), Type::TTimePnt ());
  EXPECT_EQ(Type::TTimeDiff(), Type::TTimeDiff());
  EXPECT_NE(Type::TId      (), Type::TTimeDiff());
  EXPECT_NE(Type::TInt     (), Type::TId      ());
  EXPECT_NE(Type::TReal    (), Type::TInt     ());
  EXPECT_NE(Type::TStr     (), Type::TReal    ());
  EXPECT_NE(Type::TTimePnt (), Type::TStr     ());
  EXPECT_NE(Type::TTimeDiff(), Type::TTimePnt ());
}

FIXTURE(Lists) {
  EXPECT_EQ(Type::TList(unique_ptr<TType>(new Type::TInt)), Type::TList(unique_ptr<TType>(new Type::TInt)));
  EXPECT_NE(Type::TList(unique_ptr<TType>(new Type::TInt)), Type::TList(unique_ptr<TType>(new Type::TStr)));
  EXPECT_NE(Type::TList(unique_ptr<TType>(new Type::TInt)), Type::TOpt (unique_ptr<TType>(new Type::TInt)));
  EXPECT_NE(Type::TList(unique_ptr<TType>(new Type::TInt)), Type::TOpt (unique_ptr<TType>(new Type::TStr)));
}

FIXTURE(Opts) {
  EXPECT_EQ(Type::TOpt(unique_ptr<TType>(new Type::TInt)), Type::TOpt(unique_ptr<TType>(new Type::TInt)));
  EXPECT_NE(Type::TOpt(unique_ptr<TType>(new Type::TInt)), Type::TOpt(unique_ptr<TType>(new Type::TStr)));
  EXPECT_NE(Type::TOpt(unique_ptr<TType>(new Type::TInt)), Type::TSet(unique_ptr<TType>(new Type::TInt)));
  EXPECT_NE(Type::TOpt(unique_ptr<TType>(new Type::TInt)), Type::TSet(unique_ptr<TType>(new Type::TStr)));
}

FIXTURE(Sets) {
  EXPECT_EQ(Type::TSet(unique_ptr<TType>(new Type::TInt)), Type::TSet (unique_ptr<TType>(new Type::TInt)));
  EXPECT_NE(Type::TSet(unique_ptr<TType>(new Type::TInt)), Type::TSet (unique_ptr<TType>(new Type::TStr)));
  EXPECT_NE(Type::TSet(unique_ptr<TType>(new Type::TInt)), Type::TList(unique_ptr<TType>(new Type::TInt)));
  EXPECT_NE(Type::TSet(unique_ptr<TType>(new Type::TInt)), Type::TList(unique_ptr<TType>(new Type::TStr)));
}

FIXTURE(Dicts) {
  EXPECT_EQ(
      Type::TDict(unique_ptr<TType>(new Type::TInt), unique_ptr<TType>(new Type::TStr)),
      Type::TDict(unique_ptr<TType>(new Type::TInt), unique_ptr<TType>(new Type::TStr))
  );
  EXPECT_NE(
      Type::TDict(unique_ptr<TType>(new Type::TInt), unique_ptr<TType>(new Type::TStr)),
      Type::TDict(unique_ptr<TType>(new Type::TStr), unique_ptr<TType>(new Type::TInt))
  );
}

FIXTURE(Addrs) {
  Type::TAddr lhs, rhs;
  EXPECT_EQ(lhs, rhs);
  lhs.AddField(TDir::Asc, unique_ptr<TType>(new Type::TInt));
  EXPECT_NE(lhs, rhs);
  rhs.AddField(TDir::Asc, unique_ptr<TType>(new Type::TInt));
  EXPECT_EQ(lhs, rhs);
  lhs.AddField(TDir::Asc, unique_ptr<TType>(new Type::TStr));
  EXPECT_NE(lhs, rhs);
  rhs.AddField(TDir::Desc, unique_ptr<TType>(new Type::TStr));
  EXPECT_NE(lhs, rhs);
}

FIXTURE(Objs) {
  Type::TObj lhs, rhs;
  EXPECT_EQ(lhs, rhs);
  lhs.AddField("x", unique_ptr<TType>(new Type::TReal));
  EXPECT_NE(lhs, rhs);
  rhs.AddField("x", unique_ptr<TType>(new Type::TReal));
  EXPECT_EQ(lhs, rhs);
  lhs.AddField("y", unique_ptr<TType>(new Type::TReal));
  EXPECT_NE(lhs, rhs);
  rhs.AddField("y", unique_ptr<TType>(new Type::TInt));
  EXPECT_NE(lhs, rhs);
}

