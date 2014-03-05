/* <stig/type/impl.test.cc>

   Unit test for <stig/type/impl.h>.

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

#include <stig/type.h>

#include <stig/type/type_czar.h>

#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Type;

FIXTURE(Typical) {
  TTypeCzar type_czar;
  {TType t1 = TAddr::Get(vector<pair<TAddrDir, TType>>{{TAddrDir::Desc, TInt::Get()}, {TAddrDir::Asc, TBool::Get()}});}
  {TType t2 = TAddr::Get(vector<pair<TAddrDir, TType>>{{TAddrDir::Asc, TInt::Get()}, {TAddrDir::Desc, TBool::Get()}});}
  {TType t1 = TAny::Get();}
  {TType t2 = TAny::Get();}
  {TType t1 = TBool::Get();}
  {TType t2 = TBool::Get();}
  {TType t1 = TDict::Get(TInt::Get(), TReal::Get());}
  {TType t2 = TDict::Get(TInt::Get(), TReal::Get());}
  {TType t1 = TErr::Get(TInt::Get());}
  {TType t2 = TErr::Get(TInt::Get());}
  {TType t1 = TId::Get();}
  {TType t2 = TId::Get();}
  {TType t1 = TInt::Get();}
  {TType t2 = TInt::Get();}
  {TType t1 = TList::Get(TInt::Get());}
  {TType t2 = TList::Get(TInt::Get());}
  {TType t1 = TObj::Get(map<string, TType>{ {"A", TInt::Get()}, {"B", TBool::Get()}});}
  {TType t2 = TObj::Get(map<string, TType>{ {"A", TInt::Get()}, {"B", TBool::Get()}});}
  {TType t1 = TOpt::Get(TInt::Get());}
  {TType t2 = TOpt::Get(TInt::Get());}
  {TType t1 = TReal::Get();}
  {TType t2 = TReal::Get();}
  {TType t1 = TSeq::Get(TInt::Get());}
  {TType t2 = TSeq::Get(TInt::Get());}
  {TType t1 = TSet::Get(TInt::Get());}
  {TType t2 = TSet::Get(TInt::Get());}
  {TType t1 = TStr::Get();}
  {TType t2 = TStr::Get();}
  {TType t1 = TMutable::Get(TAddr::Get(vector<pair<TAddrDir, TType>>{{TAddrDir::Asc, TInt::Get()}, {TAddrDir::Desc, TBool::Get()}}), TBool::Get());}
  {TType t1 = TMutable::Get(TAddr::Get({}), TBool::Get());}
  {TType t1 = TTimeDiff::Get();}
  {TType t2 = TTimeDiff::Get();}
  {TType t1 = TTimePnt::Get();}
  {TType t2 = TTimePnt::Get();}
  {TType t1 = TUnknown::Get();}
  {TType t2 = TUnknown::Get();}

  auto obj1 = TObj::Get({{"a", TDict::Get(TInt::Get(), TBool::Get())}, {"b", TSet::Get(TBool::Get())}, {"c", TList::Get(TInt::Get())}});
  auto obj2 = TObj::Get({{"mutable", obj1}, {"irtual", TInt::Get()}, {"float", TInt::Get()}});

  auto obj2_ptr = obj2.TryAs<TObj>();
  EXPECT_TRUE(obj2_ptr);
  if(obj2_ptr) {
    EXPECT_EQ(obj2, obj2_ptr->AsType());
    EXPECT_EQ(obj2.GetHash(), obj2_ptr->AsType().GetHash());
  }

}


FIXTURE(Mangling) {
  TTypeCzar type_czar;
  //Basic Types
  EXPECT_EQ(TAny::Get().GetMangledName(),  "a");
  EXPECT_EQ(TBool::Get().GetMangledName(), "b");
  EXPECT_EQ(TInt::Get().GetMangledName(),  "i");
  EXPECT_EQ(TReal::Get().GetMangledName(), "r");
  EXPECT_EQ(TStr::Get().GetMangledName(),  "s");
  EXPECT_EQ(TTimeDiff::Get().GetMangledName(), "d");
  EXPECT_EQ(TTimePnt::Get().GetMangledName(),  "t");

  //Unary, Binary types (Excluding mutable).
  EXPECT_EQ(TDict::Get(TInt::Get(), TBool::Get()).GetMangledName(), "Dib");
  EXPECT_EQ(TErr::Get(TInt::Get()).GetMangledName(),  "Ei");
  EXPECT_EQ(TList::Get(TInt::Get()).GetMangledName(), "Li");
  EXPECT_EQ(TOpt::Get(TInt::Get()).GetMangledName(),  "Pi");
  EXPECT_EQ(TSeq::Get(TInt::Get()).GetMangledName(),  "Qi");
  EXPECT_EQ(TSeq::Get(TSeq::Get(TInt::Get())).GetMangledName(), "QQi");
  EXPECT_EQ(TSet::Get(TInt::Get()).GetMangledName(),  "Si");

  //Addr
  auto addr1 = TAddr::Get({});
  auto addr2 = TAddr::Get({{Asc, TInt::Get()}, {Desc, TBool::Get()}});
  auto addr3 = TAddr::Get({{Desc, TReal::Get()}, {Asc, TBool::Get()}, {Asc, TSeq::Get(TInt::Get())}});
  std::string addr2_str = "A2aidb";
  EXPECT_EQ(addr1.GetMangledName(), "A0");
  EXPECT_EQ(addr2.GetMangledName(), addr2_str);
  EXPECT_EQ(addr3.GetMangledName(), "A3drabaQi");

  //Mutable
  EXPECT_EQ(TMutable::Get(addr2, TInt::Get()).GetMangledName(), "M2" + addr2_str + "i");
  EXPECT_EQ(TMutable::Get(addr1, TBool::Get()).GetMangledName(), "M2A0b");

  //Object
  auto obj1 = TObj::Get({{"a", TInt::Get()}, {"b", TBool::Get()}});
  auto obj2 = TObj::Get({{"c", TList::Get(TInt::Get())}, {"a", TBool::Get()}, {"asdf", TReal::Get()}});
  auto obj3 = TObj::Get({{"qwerty", obj2}, {"ab", TInt::Get()}});
  auto obj4 = TObj::Get({{"a", TInt::Get()}, {"b", TBool::Get()}, {"A", TReal::Get()}, {"B", TStr::Get()}});

  std::string obj2_str = "O3b1ar4asdfLi1c";

  EXPECT_EQ(obj1.GetMangledName(), "O2i1ab1b");
  EXPECT_EQ(obj2.GetMangledName(), obj2_str);
  EXPECT_EQ(obj3.GetMangledName(), "O2i2ab" + obj2_str + "6qwerty");
  EXPECT_EQ(obj4.GetMangledName(), "O4r1As1Bi1ab1b");

  //Func
  EXPECT_EQ(TFunc::Get(obj2, TInt::Get()).GetMangledName(), "Fi" + obj2_str);
}