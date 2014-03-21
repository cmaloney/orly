/* <stig/var/impl.test.cc>

   Unit test for <stig/var/impl.h>.

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

#include <stig/var.h>

#include <stig/rt/obj.h>
#include <stig/type.h>
#include <stig/type/type_czar.h>

#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Var;

struct TSomeObj : public Rt::TObj {
  TSomeObj(int64_t int_, double double_, bool bool_) : Int(int_), Real(double_), Bool(bool_) {}
  int64_t Int;
  double  Real;
  bool    Bool;
  struct  Meta {
    static const Type::TObj::Meta<TSomeObj>::TClass          Class_;
    static const Type::TObj::Meta<TSomeObj>::TField<int64_t> Int;
    static const Type::TObj::Meta<TSomeObj>::TField<double>  Real;
    static const Type::TObj::Meta<TSomeObj>::TField<bool>    Bool;
  };  // Meta
};  // TSomeObj

const Type::TObj::Meta<TSomeObj>::TClass          TSomeObj::Meta::Class_("TSomeObj");
const Type::TObj::Meta<TSomeObj>::TField<int64_t> Int("Int", &TSomeObj::Int);
const Type::TObj::Meta<TSomeObj>::TField<double>  Real("Real", &TSomeObj::Real);
const Type::TObj::Meta<TSomeObj>::TField<bool>    Bool("Bool", &TSomeObj::Bool);

Type::TTypeCzar TypeCzar;

//TODO: Test mutables.
FIXTURE(Typical) {
  TVar bool_(true);
  EXPECT_TRUE(bool_.GetType() == Type::TBool::Get());
  TUUID id;
  TVar id_(id);
  EXPECT_TRUE(id_.GetType() == Type::TId::Get());
  TVar int_(5);
  EXPECT_TRUE(int_.GetType() == Type::TInt::Get());
  TVar real_(5.22);
  EXPECT_TRUE(real_.GetType() == Type::TReal::Get());
  TVar str_(string("Hello World"));
  EXPECT_TRUE(str_.GetType() == Type::TStr::Get());
  auto td = Base::Chrono::TTimeDiff(1);
  TVar timediff_(td);
  EXPECT_TRUE(timediff_.GetType() == Type::TTimeDiff::Get());
  auto tp = Base::Chrono::Now();
  TVar timepnt_(tp);
  EXPECT_TRUE(timepnt_.GetType() == Type::TTimePnt::Get());
  TVar list_int_(vector<int64_t>{5, 2, 1});
  EXPECT_TRUE(list_int_.GetType() == Type::TList::Get(Type::TInt::Get()));
  TVar list_real_(vector<double>{5.5, 2.2, 1.1});
  EXPECT_TRUE(list_real_.GetType() == Type::TList::Get(Type::TReal::Get()));
  TVar list_list_(vector<vector<int64_t>>{{5, 2, 1}, {1, 2, 5}});
  EXPECT_TRUE(list_list_.GetType() == Type::TList::Get(Type::TList::Get(Type::TInt::Get())));
  TVar set_int_(Rt::TSet<int64_t>{5, 2, 1});
  EXPECT_TRUE(set_int_.GetType() == Type::TSet::Get(Type::TInt::Get()));
  TVar set_real_(Rt::TSet<double>{5.5, 2.2, 1.1});
  EXPECT_TRUE(set_real_.GetType() == Type::TSet::Get(Type::TReal::Get()));
  /* Sets of sets are disallowed
  TVar set_set_(Rt::TSet<Rt::TSet<int64_t>>{{5, 2, 1}, {1, 2, 3}});
  EXPECT_TRUE(set_set_.GetType() == Type::TSet::Get(Type::TSet::Get(Type::TInt::Get())));
  */
  Rt::TDict<int64_t, double> m{{5, 5.5}, {2, 2.2}, {1, 1.1}};
  TVar dict_int_real(m);
  EXPECT_TRUE(dict_int_real.GetType() == Type::TDict::Get(Type::TInt::Get(), Type::TReal::Get()));
  TVar dict_int_dict_int_real(Rt::TDict<int64_t, Rt::TDict<int64_t, double>>{{5, m}});
  EXPECT_TRUE(dict_int_real.GetType() == Type::TDict::Get(Type::TInt::Get(), Type::TReal::Get()));
  /* TODO
  Rt::TAddr<Rt::TAddrElem<TAddrDir::Asc, int64_t>, Rt::TAddrElem<TAddrDir::Asc, string>, Rt::TAddrElem<TAddrDir::Asc, bool>, Rt::TAddrElem<TAddrDir::Asc, double>> a(5, string("Hello World"), true, 2.2);
  TVar addr_int_str_bool_double(a);
  EXPECT_TRUE(addr_int_str_bool_double.GetType() == Type::TAddr::Get(vector<pair<TAddrDir, Type::TType>>{{TAddrDir::Asc, Type::TInt::Get()}, {TAddrDir::Asc, Type::TStr::Get()}, {TAddrDir::Asc, Type::TBool::Get()}, {TAddrDir::Asc, Type::TReal::Get()}}));
  vector<pair<TAddrDir, TVar>> a2{{TAddrDir::Asc, TVar(5)}, {TAddrDir::Asc, TVar(string("Hello World"))}, {TAddrDir::Asc, TVar(true)}, {TAddrDir::Asc, TVar(2.2)}};
  TVar addr_int_str_bool_double_2;
  addr_int_str_bool_double_2 = TVar::Addr(a2);
  EXPECT_TRUE(addr_int_str_bool_double_2.GetType() == Type::TAddr::Get(vector<pair<TAddrDir, Type::TType>>{{TAddrDir::Asc, Type::TInt::Get()}, {TAddrDir::Asc, Type::TStr::Get()}, {TAddrDir::Asc, Type::TBool::Get()}, {TAddrDir::Asc, Type::TReal::Get()}}));
  */
  TSomeObj some_obj(5, 2.2, true);
  TSomeObj some_other_obj(7, 2.9, false);
  /* TVar obj(some_obj);
  EXPECT_TRUE(obj.GetType() == Type::TObj::Get(map<string, Type::TType>{{"Bool", Type::TBool::Get()}, {"Int", Type::TInt::Get()}, {"Real", Type::TReal::Get()}}));

  class TObjVisitor : public TVar::TVisitor {
    NO_COPY_SEMANTICS(TObjVisitor);
    public:

    TObjVisitor() {}

    virtual void operator()(const Var::TAddr *) const {}
    virtual void operator()(const Var::TBool *) const {}
    virtual void operator()(const Var::TDict *) const {}
    virtual void operator()(const Var::TFree *) const {}
    virtual void operator()(const Var::TId *) const {}
    virtual void operator()(const Var::TInt *) const {}
    virtual void operator()(const Var::TList *) const {}
    virtual void operator()(const Var::TMutable *) const {}
    virtual void operator()(const Var::TObj *that) const {
      const Var::TObj::TFieldsByName &fields_by_name = that->GetVal();
      EXPECT_EQ(fields_by_name.size(), 3U);
      EXPECT_TRUE(fields_by_name.find("Int")->second == TVar(5));
      EXPECT_FALSE(fields_by_name.find("Int")->second == TVar(7));
      EXPECT_TRUE(fields_by_name.find("Real")->second == TVar(2.2));
      EXPECT_TRUE(fields_by_name.find("Bool")->second == TVar(true));
    }
    virtual void operator()(const Var::TOpt *) const {}
    virtual void operator()(const Var::TReal *) const {}
    virtual void operator()(const Var::TSet *) const {}
    virtual void operator()(const Var::TStr *) const {}
    virtual void operator()(const Var::TTimeDiff *) const {}
    virtual void operator()(const Var::TTimePnt *) const {}
    virtual void operator()(const Var::TUnknown *) const {}

  };
  TObjVisitor obj_visitor;
  obj.Accept(obj_visitor);
  */

  TVar opt_int(Rt::TOpt<int64_t>(5));
  EXPECT_TRUE(opt_int.GetType() == Type::TOpt::Get(Type::TInt::Get()));
  TVar opt_int_unknown(*Rt::TOpt<int64_t>::Unknown);
  EXPECT_TRUE(opt_int.GetType() == Type::TOpt::Get(Type::TInt::Get()));
}

#if 0 // TODO: TAddr
FIXTURE(Memory) {
  TVar *bool_ = new TVar(true);
  delete bool_;
  typedef Rt::TAddr<Rt::TAddrElem<Asc, int64_t>, Rt::TAddrElem<Asc, string>> TAddr1;
  TAddr1 a(5, string("Hello World"));
  typedef Rt::TAddr<Rt::TAddrElem<Asc, TAddr1>, Rt::TAddrElem<Asc, int64_t>> TAddr2;
  TVar *addr_int_str = new TVar(TAddr2(a, 5));
  delete addr_int_str;
}
#endif

FIXTURE(Assign) {
  TVar int_;
  int_ = TVar(5);
  EXPECT_TRUE(int_ == TVar(5));
}

FIXTURE(Casts) {
  EXPECT_TRUE(TVar::TDt<bool>::As(TVar(true)) == true);
  TUUID uuid;
  EXPECT_TRUE(TVar::TDt<TUUID>::As(TVar(uuid)) == uuid);
  EXPECT_TRUE(TVar::TDt<int64_t>::As(TVar(5)) == 5);
  EXPECT_TRUE(TVar::TDt<double>::As(TVar(5.5)) == 5.5);
  EXPECT_TRUE(TVar::TDt<std::string>::As(TVar(string("Hello World"))) == "Hello World");
  std::vector<int64_t> vec_{5, 6, 7, 8, 9};
  EXPECT_TRUE(TVar::TDt<std::vector<int64_t>>::As(TVar(vec_)) == vec_);
  Rt::TSet<int64_t> set_{5, 6, 7, 8, 9};
  EXPECT_TRUE(TVar::TDt<Rt::TSet<int64_t>>::As(TVar(set_)) == set_);
  typedef Rt::TDict<int64_t, double> TMap;
  TMap map_{{5, 5.5}, {6, 6.6}};
  EXPECT_TRUE(TVar::TDt<TMap>::As(TVar(map_)) == map_);
#if 0 // TODO: TAddr
  typedef Rt::TAddr<Rt::TAddrElem<Asc, int64_t>, Rt::TAddrElem<Desc, int64_t>, Rt::TAddrElem<Asc, double>> TAddr_;
  TAddr_ addr_(5, 1, 5.5);
  EXPECT_TRUE(TVar::TDt<TAddr_>::As(TVar(addr_)) == addr_);
#endif
}

FIXTURE(Copy) {
  TVar int_ = Var::TVar(5);
  TVar int_new = int_.Copy();
  EXPECT_TRUE(int_ == int_new);
}

FIXTURE(Index) {
  TVar list_(std::vector<int64_t>{1, 2, 3, 4, 5});
  EXPECT_TRUE(list_.Index(1) == TVar(2));
  EXPECT_TRUE(list_.Index(1) != TVar(1));
}