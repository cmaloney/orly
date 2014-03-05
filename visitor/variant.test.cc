/* <visitor/variant.test.cc>

   Unit test for <visitor/variant.h>.

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

#include <visitor/variant.h>

#include <array>
#include <condition_variable>
#include <iostream>
#include <future>
#include <list>
#include <mutex>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <mpl/and.h>
#include <mpl/contains.h>
#include <mpl/get_union.h>
#include <mpl/type_set.h>
#include <visitor/visitor.h>

#include <test/kit.h>

using namespace Visitor;

class FooFamily;

using TFoo = TVariant<FooFamily>;

namespace Visitor {

  template <>
  struct TVariant<FooFamily>::Members {

    using All = Mpl::TTypeSet<int, std::string, std::vector<TFoo>>;

  };  // TVariant<FooFamily>::Members

}  // Visitor

template <typename TMember>
using IsFoo = Mpl::Contains<TFoo::Members::All, c14::decay_t<TMember>>;

FIXTURE(Stateless) {
  TFoo variant;
  EXPECT_FALSE(variant);
}

FIXTURE(Stateful) {
  TFoo variant = TFoo::New(42);
  EXPECT_TRUE(variant);
  variant.Reset();
  EXPECT_FALSE(variant);
}

/**
 *   void return type, no extra parameters.
 **/

// print_name.h
struct TPrintName {

  void operator()(int) const;
  void operator()(const std::string &) const;
  void operator()(const std::vector<TFoo> &) const;

};  // TPrintName

template <typename TMember>
requires<IsFoo<TMember>::value,
void> PrintName(TMember &&member) {
  TPrintName()(std::forward<TMember>(member));
}

void PrintName(const TFoo &that);

// print_name.cc
void TPrintName::operator()(int) const { std::cout << "int" << std::endl; }
void TPrintName::operator()(const std::string &) const {
  std::cout << "std::string" << std::endl;
}
void TPrintName::operator()(const std::vector<TFoo> &) const {
  std::cout << "std::vector<TFoo>" << std::endl;
}

void PrintName(const TFoo &that) {
  Single::Accept<TFoo::TApplier<TPrintName>>(that);
};

// print_name.test.cc
FIXTURE(PrintName) {
  /* int */ {
    TFoo variant = TFoo::New(0);
    PrintName(variant);
  }
  /* std::string */ {
    TFoo variant = TFoo::New(std::string("Hello"));
    PrintName(variant);
  }
  /* std::vector<TFoo> */ {
    TFoo variant = TFoo::New(
        std::vector<TFoo>{TFoo::New(42), TFoo::New(std::string("Hello")),
                          TFoo::New(24), TFoo::New(std::vector<TFoo>{})});
    PrintName(variant);
  }
}

/**
 *  non-void return type, no extra parameters.
 **/

// get_int.h
struct TGetInt {

  int operator()(int that) const;
  int operator()(const std::string &that) const;
  int operator()(const std::vector<TFoo> &that) const;

};  // TGetInt

template <typename TMember>
requires<IsFoo<TMember>::value,
int> GetInt(TMember &&member) {
  return TGetInt()(std::forward<TMember>(member));
}

int GetInt(const TFoo &that);

// get_int.cc
int TGetInt::operator()(int that) const { return that; }
int TGetInt::operator()(const std::string &that) const {
  assert(&that);
  return stoi(that);
}
int TGetInt::operator()(const std::vector<TFoo> &that) const {
  assert(&that);
  int result = 0;
  for (const auto &variant : that) {
    result += GetInt(variant);
  }
  return result;
}

int GetInt(const TFoo &that) {
  return Single::Accept<TFoo::TApplier<TGetInt>>(that);
}

// get_int.test.cc
FIXTURE(GetInt) {
  /* int */ {
    TFoo variant = TFoo::New(42);
    EXPECT_EQ(GetInt(variant), 42);
  }
  /* std::string */ {
    TFoo variant = TFoo::New(std::string("42"));
    EXPECT_EQ(GetInt(variant), 42);
  }
  /* std::vector<TFoo> */ {
    TFoo variant = TFoo::New(std::vector<TFoo>{
        TFoo::New(23), TFoo::New(std::string("42")), TFoo::New(5),
        TFoo::New(
            std::vector<TFoo>{TFoo::New(9), TFoo::New(std::string("22"))})});
    EXPECT_EQ(GetInt(variant), 101);
  }
}

/**
 *   void return type, extra parameters.
 **/

// write_name.h
struct TWriteName {

  std::ostream &operator()(int, std::ostream &strm) const;
  std::ostream &operator()(const std::string &, std::ostream &strm) const;
  std::ostream &operator()(const std::vector<TFoo> &, std::ostream &strm) const;

};  // TWriteName

template <typename TMember>
requires<IsFoo<TMember>::value,
std::ostream &> WriteName(TMember &&member, std::ostream &strm) {
  return TWriteName()(std::forward<TMember>(member), strm);
}

std::ostream &WriteName(const TFoo &that, std::ostream &strm);

// write_name.cc
std::ostream &TWriteName::operator()(int, std::ostream &strm) const {
  return strm << "int";
};
std::ostream &TWriteName::operator()(const std::string &,
                                     std::ostream &strm) const {
  return strm << "std::string";
}
std::ostream &TWriteName::operator()(const std::vector<TFoo> &,
                                     std::ostream &strm) const {
  return strm << "std::vector<TFoo>";
}

std::ostream &WriteName(const TFoo &that, std::ostream &strm) {
  return Single::Accept<TFoo::TApplier<TWriteName>>(that, strm);
};

// write_name.test.cc
FIXTURE(WriteName) {
  /* int */ {
    std::ostringstream strm;
    TFoo variant = TFoo::New(0);
    WriteName(variant, strm);
    EXPECT_EQ(strm.str(), "int");
  }
  /* std::string */ {
    std::ostringstream strm;
    TFoo variant = TFoo::New(std::string("Hello"));
    WriteName(variant, strm);
    EXPECT_EQ(strm.str(), "std::string");
  }
  /* std::vector<TFoo> */ {
    std::ostringstream strm;
    TFoo variant = TFoo::New(
        std::vector<TFoo>{TFoo::New(std::string("Hello")), TFoo::New(42)});
    WriteName(variant, strm);
    EXPECT_EQ(strm.str(), "std::vector<TFoo>");
  }
}

/**
 *  non-void return type, extra parameters.
 **/

// get_area_and_write_vector.h
struct TGetIntAndWriteVector {

  int operator()(int that,
                 std::ostream &strm,
                 const std::vector<int> &ints) const;

  int operator()(const std::string &that,
                 std::ostream &strm,
                 const std::vector<int> &ints) const;

  int operator()(const std::vector<TFoo> &that,
                 std::ostream &strm,
                 const std::vector<int> &ints) const;

};  // TGetIntAndWriteVector

template <typename TMember>
requires<IsFoo<TMember>::value,
int> GetIntAndWriteVector(TMember &&member,
                         std::ostream &strm,
                         const std::vector<int> &ints) {
  return TGetIntAndWriteVector()(std::forward<TMember>(member), strm, ints);
}

int GetIntAndWriteVector(const TFoo &that,
                         std::ostream &strm,
                         const std::vector<int> &ints);

// get_area_and_write_vector.cc
int TGetIntAndWriteVector::operator()(int that,
                                      std::ostream &strm,
                                      const std::vector<int> &ints) const {
  for (const auto &i : ints) {
    strm << i;
  }
  return GetInt(that);
}
int TGetIntAndWriteVector::operator()(const std::string &that,
                                      std::ostream &strm,
                                      const std::vector<int> &ints) const {
  assert(&that);
  for (const auto &i : ints) {
    strm << i;
  }
  return GetInt(that);
}
int TGetIntAndWriteVector::operator()(const std::vector<TFoo> &that,
                                      std::ostream &strm,
                                      const std::vector<int> &ints) const {
  assert(&that);
  for (const auto &i : ints) {
    strm << i;
  }
  return GetInt(that);
}

int GetIntAndWriteVector(const TFoo &that,
                         std::ostream &strm,
                         const std::vector<int> &ints) {
  using applier_t = TFoo::TApplier<TGetIntAndWriteVector>;
  return Single::Accept<applier_t>(that, strm, ints);
}

// get_area_and_write_vector.test.cc
FIXTURE(GetIntAndWriteVector) {
  /* int */ {
    std::ostringstream strm;
    TFoo variant = TFoo::New(101);
    EXPECT_EQ(GetIntAndWriteVector(variant, strm, {0, 1, 2, 3, 4}), 101);
    EXPECT_EQ(strm.str(), "01234");
  }
  /* std::string */ {
    std::ostringstream strm;
    TFoo variant = TFoo::New(std::string("101"));
    EXPECT_EQ(GetIntAndWriteVector(variant, strm, {5, 6, 7, 8, 9}), 101);
    EXPECT_EQ(strm.str(), "56789");
  }
  /* std::vector<TFoo> */ {
    std::ostringstream strm;
    TFoo variant = TFoo::New(
        std::vector<TFoo>{TFoo::New(std::string("11")),   TFoo::New(88),
                          TFoo::New(std::vector<TFoo>{}), TFoo::New(2)});
    EXPECT_EQ(GetIntAndWriteVector(variant, strm, std::vector<int>{}), 101);
    EXPECT_EQ(strm.str(), "");
  }
}

/* Multiple families and ToString(int) and ToString(std::string) being shared. */
class BarFamily;

using TBar = TVariant<BarFamily>;

namespace Visitor {

  template <>
  struct TVariant<BarFamily>::Members {

    using All = Mpl::TTypeSet<int, std::string, std::list<TBar>>;

  };  // TVariant<BarFamily>::Members

}  // Visitor

template <typename TMember>
using IsFooBar =
    Mpl::Contains<Mpl::TGetUnion<TFoo::Members::All, TBar::Members::All>,
                  c14::decay_t<TMember>>;

// to_string.h
struct TToString {

  std::string operator()(int that) const;
  std::string operator()(const std::string &that) const;
  std::string operator()(const std::vector<TFoo> &that) const;
  std::string operator()(const std::list<TBar> &that) const;

};  // TToString

template <typename TMember>
requires<IsFooBar<TMember>::value,
std::string> ToString(TMember &&member) {
  return TToString()(std::forward<TMember>(member));
}

std::string ToString(const TFoo &that);

std::string ToString(const TBar &that);

// to_string.cc
std::string TToString::operator()(int that) const {
  std::ostringstream strm;
  strm << that;
  return strm.str();
}

std::string TToString::operator()(const std::string &that) const {
  assert(&that);
  return "'" + that + "'";
}

std::string TToString::operator()(const std::vector<TFoo> &that) const {
  assert(&that);
  std::ostringstream strm;
  strm << '[';
  bool sep = false;
  for (const auto &variant : that) {
    sep ? strm << ", " : sep = true;
    strm << ToString(variant);
  }
  strm << ']';
  return strm.str();
}

std::string TToString::operator()(const std::list<TBar> &that) const {
  assert(&that);
  std::ostringstream strm;
  strm << '[';
  bool sep = false;
  for (const auto &variant : that) {
    sep ? strm << ", " : sep = true;
    strm << ToString(variant);
  }
  strm << ']';
  return strm.str();
}

std::string ToString(const TFoo &that) {
  return Single::Accept<TFoo::TApplier<TToString>>(that);
}

std::string ToString(const TBar &that) {
  return Single::Accept<TBar::TApplier<TToString>>(that);
}

FIXTURE(MultipleFamilies) {
  TFoo x;
  EXPECT_FALSE(x);
  x = TFoo::New(42);
  EXPECT_EQ(ToString(x), "42");
  TBar bar =
      TBar::New(std::list<TBar>{TBar::New(4), TBar::New(std::string("World"))});
  EXPECT_EQ(ToString(bar), "[4, 'World']");
}

FIXTURE(ChangingType) {
  TFoo variant = TFoo::New(101);
  EXPECT_EQ(ToString(variant), "101");
  variant = TFoo::New(std::string("mofo"));
  EXPECT_EQ(ToString(variant), "'mofo'");
}

FIXTURE(Move) {
  TFoo a = TFoo::New(101);
  TFoo b;
  b = std::move(a);
  EXPECT_FALSE(a);
  EXPECT_EQ(ToString(b), "101");
}

FIXTURE(Copy) {
  TFoo a = TFoo::New(101);
  TFoo b;
  b = a;
  EXPECT_EQ(ToString(a), "101");
  EXPECT_EQ(ToString(b), "101");
}

/* Disabled for now. Refer to <visitor/variant.h> for details. */
#if 0
FIXTURE(Share) {
  struct TGetAddress {
    const void *operator()(int) const { return nullptr; }
    const void *operator()(const std::string &that) const { return &that; }
    const void *operator()(const std::vector<TFoo> &that) const { return &that; }
  };  // TGetAddress
  struct TShare {
    TFoo operator()(int that) const { return TFoo::New(that); }
    TFoo operator()(const std::string &that) const { return TFoo::Share(that); }
    TFoo operator()(const std::vector<TFoo> &that) const {
      return TFoo::Share(that);
    }
  };  // TShare
  TFoo lhs = TFoo::New(std::string("hello"));
  auto *lhs_addr = Single::Accept<TFoo::TApplier<TGetAddress>>(lhs);
  TFoo rhs = Single::Accept<TFoo::TApplier<TShare>>(lhs);
  auto *rhs_addr = Single::Accept<TFoo::TApplier<TGetAddress>>(rhs);
  EXPECT_EQ(lhs_addr, rhs_addr);
}
#endif

// set_default.h
struct TSetDefault {

  void operator()(int &that) const;
  void operator()(std::string &that) const;
  void operator()(std::vector<TFoo> &that) const;

};  // TSetDefault

template <typename TMember>
requires<IsFoo<TMember>::value,
void> SetDefault(TMember &&member) {
  TSetDefault()(std::forward<TMember>(member));
}

void SetDefault(TFoo &that);

// set_default.cc
void TSetDefault::operator()(int &that) const {
  assert(&that);
  that = 0;
}

void TSetDefault::operator()(std::string &that) const {
  assert(&that);
  that.clear();
}

void TSetDefault::operator()(std::vector<TFoo> &that) const {
  assert(&that);
  for (auto &variant : that) {
    SetDefault(variant);
  }
}

void SetDefault(TFoo &that) {
  return Single::Accept<TFoo::TMutatingApplier<TSetDefault>>(that);
}

FIXTURE(Mutation) {
  /* int */ {
    TFoo variant = TFoo::New(101);
    SetDefault(variant);
    EXPECT_EQ(ToString(variant), "0");
  }
  /* std::string */ {
    TFoo variant = TFoo::New(std::string("Foo"));
    SetDefault(variant);
    EXPECT_EQ(ToString(variant), "''");
  }
  /* std::vector<TFoo> */ {
    TFoo variant = TFoo::New(
        std::vector<TFoo>{TFoo::New(std::string("11")),   TFoo::New(88),
                          TFoo::New(std::vector<TFoo>{}), TFoo::New(2)});
    SetDefault(variant);
    EXPECT_EQ(ToString(variant), "['', 0, [], 0]");
  }
}

// set_default_template.h
struct TSetDefaultTemplate {

  using Signature = void ();

  template <typename TMember>
  void operator()(TMember &that) const {
    assert(&that);
    that = TMember{};
  }

};  // TSetDefaultTemplate

template <typename TMember>
requires<IsFoo<TMember>::value,
void> SetDefaultTemplate(TMember &&member) {
  TSetDefaultTemplate()(std::forward<TMember>(member));
}

void SetDefaultTemplate(TFoo &that);

void SetDefaultTemplate(TFoo &that) {
  return Single::Accept<TFoo::TMutatingApplier<TSetDefaultTemplate>>(that);
}

// set_default_template.test.cc
FIXTURE(SetDefaultTemplate) {
  /* int */ {
    TFoo variant = TFoo::New(101);
    SetDefaultTemplate(variant);
    EXPECT_EQ(ToString(variant), "0");
  }
  /* std::string */ {
    TFoo variant = TFoo::New(std::string("Foo"));
    SetDefaultTemplate(variant);
    EXPECT_EQ(ToString(variant), "''");
  }
  /* std::vector<TFoo> */ {
    TFoo variant = TFoo::New(
        std::vector<TFoo>{TFoo::New(std::string("11")),   TFoo::New(88),
                          TFoo::New(std::vector<TFoo>{}), TFoo::New(2)});
    SetDefaultTemplate(variant);
    EXPECT_EQ(ToString(variant), "[]");
  }
}

// write_name_double.h
struct TWriteNameDouble {

  void operator()(int,
                  int,
                  std::ostream &) const;
  void operator()(int,
                  const std::string &,
                  std::ostream &) const;
  void operator()(int,
                  const std::list<TBar> &,
                  std::ostream &) const;
  void operator()(int,
                  const std::vector<TFoo> &,
                  std::ostream &) const;
  void operator()(const std::string &,
                  int,
                  std::ostream &) const;
  void operator()(const std::string &,
                  const std::string &,
                  std::ostream &) const;
  void operator()(const std::string &,
                  const std::list<TBar> &,
                  std::ostream &) const;
  void operator()(const std::string &,
                  const std::vector<TFoo> &,
                  std::ostream &) const;
  void operator()(const std::vector<TFoo> &,
                  int,
                  std::ostream &) const;
  void operator()(const std::vector<TFoo> &,
                  const std::string &,
                  std::ostream &) const;
  void operator()(const std::vector<TFoo> &,
                  const std::list<TBar> &,
                  std::ostream &) const;
  void operator()(const std::vector<TFoo> &,
                  const std::vector<TFoo> &,
                  std::ostream &) const;
  void operator()(const std::list<TBar> &,
                  int,
                  std::ostream &) const;
  void operator()(const std::list<TBar> &,
                  const std::string &,
                  std::ostream &) const;
  void operator()(const std::list<TBar> &,
                  const std::vector<TFoo> &,
                  std::ostream &) const;
  void operator()(const std::list<TBar> &,
                  const std::list<TBar> &,
                  std::ostream &) const;

};  // TWriteNameDouble

template <typename TLhs, typename TRhs>
requires<IsFooBar<TLhs>::value && IsFooBar<TRhs>::value,
void> WriteNameDouble(TLhs &&lhs, TRhs &&rhs, std::ostream &strm) {
  TWriteNameDouble()(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs), strm);
}

void WriteNameDouble(const TFoo &, const TFoo &, std::ostream &);
void WriteNameDouble(const TFoo &, const TBar &, std::ostream &);
void WriteNameDouble(const TBar &, const TFoo &, std::ostream &);
void WriteNameDouble(const TBar &, const TBar &, std::ostream &);

// write_name_double.cc
void TWriteNameDouble::operator()(int,
                                  int,
                                  std::ostream &strm) const {
  strm << "(int, int)";
}
void TWriteNameDouble::operator()(int,
                                  const std::string &,
                                  std::ostream &strm) const {
  strm << "(int, const std::string &)";
}
void TWriteNameDouble::operator()(int,
                                  const std::list<TBar> &,
                                  std::ostream &strm) const {
  strm << "(int, const std::list<TBar> &)";
}
void TWriteNameDouble::operator()(int,
                                  const std::vector<TFoo> &,
                                  std::ostream &strm) const {
  strm << "(int, const std::vector<TFoo> &)";
}
void TWriteNameDouble::operator()(const std::string &,
                                  int,
                                  std::ostream &strm) const {
  strm << "(const std::string &, int)";
}
void TWriteNameDouble::operator()(const std::string &,
                                  const std::string &,
                                  std::ostream &strm) const {
  strm << "(const std::string &, const std::string &)";
}
void TWriteNameDouble::operator()(const std::string &,
                                  const std::list<TBar> &,
                                  std::ostream &strm) const {
  strm << "(const std::string &, const std::list<TBar> &)";
}
void TWriteNameDouble::operator()(const std::string &,
                                  const std::vector<TFoo> &,
                                  std::ostream &strm) const {
  strm << "(const std::string &, const std::vector<TFoo> &)";
}
void TWriteNameDouble::operator()(const std::vector<TFoo> &,
                                  int,
                                  std::ostream &strm) const {
  strm << "(const std::vector<TFoo> &, int)";
}
void TWriteNameDouble::operator()(const std::vector<TFoo> &,
                                  const std::string &,
                                  std::ostream &strm) const {
  strm << "(const std::vector<TFoo> &, const std::string &)";
}
void TWriteNameDouble::operator()(const std::vector<TFoo> &,
                                  const std::list<TBar> &,
                                  std::ostream &strm) const {
  strm << "(const std::vector<TFoo> &, const std::list<TBar> &)";
}
void TWriteNameDouble::operator()(const std::vector<TFoo> &,
                                  const std::vector<TFoo> &,
                                  std::ostream &strm) const {
  strm << "(const std::vector<TFoo> &, const std::vector<TFoo> &)";
}
void TWriteNameDouble::operator()(const std::list<TBar> &,
                                  int,
                                  std::ostream &strm) const {
  strm << "(const std::list<TBar> &, int)";
}
void TWriteNameDouble::operator()(const std::list<TBar> &,
                                  const std::string &,
                                  std::ostream &strm) const {
  strm << "(const std::list<TBar> &, const std::string &)";
}
void TWriteNameDouble::operator()(const std::list<TBar> &,
                                  const std::vector<TFoo> &,
                                  std::ostream &strm) const {
  strm << "(const std::list<TBar> &, const std::vector<TFoo> &)";
}
void TWriteNameDouble::operator()(const std::list<TBar> &,
                                  const std::list<TBar> &,
                                  std::ostream &strm) const {
  strm << "(const std::list<TBar> &, const std::list<TBar> &)";
}

void WriteNameDouble(const TFoo &lhs, const TFoo &rhs, std::ostream &strm) {
  return Double::Accept<TFoo::TDoubleApplier<TWriteNameDouble>>(lhs, rhs, strm);
}

void WriteNameDouble(const TFoo &lhs, const TBar &rhs, std::ostream &strm) {
  using applier_t = TFoo::TDoubleApplier<TWriteNameDouble, TBar::TVisitor>;
  return Double::Accept<applier_t>(lhs, rhs, strm);
}

void WriteNameDouble(const TBar &lhs, const TFoo &rhs, std::ostream &strm) {
  using applier_t = TBar::TDoubleApplier<TWriteNameDouble, TFoo::TVisitor>;
  return Double::Accept<applier_t>(lhs, rhs, strm);
}

void WriteNameDouble(const TBar &lhs, const TBar &rhs, std::ostream &strm) {
  return Double::Accept<TBar::TDoubleApplier<TWriteNameDouble>>(lhs, rhs, strm);
}

// write_name_double.test.cc
FIXTURE(FooFoo) {
  std::ostringstream strm;
  TFoo lhs = TFoo::New(std::string("yo"));
  TFoo rhs = TFoo::New(
      std::vector<TFoo>{TFoo::New(101), TFoo::New(std::string("heh")),
                        TFoo::New(202), TFoo::New(std::vector<TFoo>{})});
  WriteNameDouble(lhs, rhs, strm);
  EXPECT_EQ(strm.str(), "(const std::string &, const std::vector<TFoo> &)");
}

FIXTURE(FooBar) {
  std::ostringstream strm;
  TFoo lhs = TFoo::New(101);
  TBar rhs = TBar::New(std::string("hello"));
  WriteNameDouble(lhs, rhs, strm);
  EXPECT_EQ(strm.str(), "(int, const std::string &)");
}

FIXTURE(BarFoo) {
  std::ostringstream strm;
  TBar lhs = TBar::New(
      std::list<TBar>{TBar::New(std::string("list")), TBar::New(505)});
  TFoo rhs = TFoo::New(std::vector<TFoo>{TFoo::New(101), TFoo::New(202),
                                         TFoo::New(std::string("heh"))});
  WriteNameDouble(lhs, rhs, strm);
  EXPECT_EQ(strm.str(), "(const std::list<TBar> &, const std::vector<TFoo> &)");
}

FIXTURE(BarBar) {
  std::ostringstream strm;
  TBar lhs = TBar::New(
      std::list<TBar>{TBar::New(std::string("list")), TBar::New(505)});
  TBar rhs = TBar::New(101);
  WriteNameDouble(lhs, rhs, strm);
  EXPECT_EQ(strm.str(), "(const std::list<TBar> &, int)");
}

// Reference counting tests.

class TObj {
  public:

  static std::atomic_int_fast32_t ConstructCount;
  static std::atomic_int_fast32_t MoveCount;
  static std::atomic_int_fast32_t CopyCount;
  static std::atomic_int_fast32_t DestroyCount;

  static void Reset() {
    TObj::ConstructCount = 0;
    TObj::MoveCount = 0;
    TObj::CopyCount = 0;
    TObj::DestroyCount = 0;
  }

  TObj() { ++ConstructCount; }

  TObj(TObj &&) { ++MoveCount; }

  TObj(const TObj &) { ++CopyCount; }

  ~TObj() { ++DestroyCount; }

};  // TObj

std::atomic_int_fast32_t TObj::ConstructCount(0);
std::atomic_int_fast32_t TObj::MoveCount(0);
std::atomic_int_fast32_t TObj::CopyCount(0);
std::atomic_int_fast32_t TObj::DestroyCount(0);

class TrackerFamily;

using TTracker = TVariant<TrackerFamily>;

namespace Visitor {

  template <>
  struct TVariant<TrackerFamily>::Members {

    using All = Mpl::TTypeSet<TObj>;

  };  // TVariant<TrackerFamily>::Members

}  // Visitor

FIXTURE(SingleThreadedNoCopy) {
  TObj::Reset();
  /* scope */ {
    // Assign tracker with an instance of TObj.
    TTracker tracker = TTracker::New(TObj{});
    // tracker is non-empty, ie. is valid.
    EXPECT_TRUE(tracker);
    // Ref-count up with an instance of tracker_copy which "copies" tracker.
    TTracker tracker_copy = tracker;
    EXPECT_TRUE(tracker);
    // Release tracker.
    tracker.Reset();
    // tracker isn't valid, but the reference is still held by y.
    EXPECT_FALSE(tracker);
    EXPECT_TRUE(tracker_copy);
    tracker_copy.Reset();
    EXPECT_FALSE(tracker);
    EXPECT_FALSE(tracker_copy);
  }  // scope
  EXPECT_EQ(TObj::ConstructCount, 1);  // TObj constructs once.
  EXPECT_EQ(TObj::MoveCount, 1);       // TObj moves into TState
  EXPECT_EQ(TObj::CopyCount, 0);       // TObj does not get copied.
  EXPECT_EQ(TObj::DestroyCount,
            TObj::ConstructCount + TObj::MoveCount + TObj::CopyCount);
}

// mutate.h
struct TMutate {

  void operator()(TObj &) const;

};  // TMutate

template <typename TMember>
requires<IsFoo<TMember>::value,
void> Mutate(TMember &&member) {
  TMutate()(std::forward<TMember>(member));
}

void Mutate(TTracker &that);

// mutate.cc
void TMutate::operator()(TObj &) const {}

void Mutate(TTracker &that) {
  return Single::Accept<TTracker::TMutatingApplier<TMutate>>(that);
}

FIXTURE(SingleThreadedCopy) {
  TObj::Reset();
  /* scope */ {
    // Assign tracker with an instance of TObj.
    TTracker tracker = TTracker::New(TObj{});
    // tracker is non-empty, ie. is valid.
    EXPECT_TRUE(tracker);
    // Mutate tracker, there should be no copy since tracker is the only holder.
    Mutate(tracker);
    TTracker tracker_copy = tracker;
    // Now there are multiple holders of the TObj instance.
    // Mutating tracker should produce one copy.
    Mutate(tracker);
  }  // scope
  EXPECT_EQ(TObj::ConstructCount, 1);  // TObj constructs once.
  EXPECT_EQ(TObj::MoveCount, 1);       // TObj moves into TState
  EXPECT_EQ(TObj::CopyCount, 1);       // TObj gets copied once.
  EXPECT_EQ(TObj::DestroyCount,
            TObj::ConstructCount + TObj::MoveCount + TObj::CopyCount);
}

FIXTURE(MultiThreadedNoCopy) {
  TObj::Reset();
  /* scope */ {
    std::mutex mutex;
    std::condition_variable cv;
    int ready = 0;
    const int num_threads = 50;
    TTracker tracker = TTracker::New(TObj{});
    EXPECT_TRUE(tracker);
    /* The job of each thread is to copy the tracker in, then signal ready. Once
       the threads have signalled, the threads can keep
       copying the object within a loop, and none of these should be creating
       copies. */
    auto job = [&cv, &mutex, &ready, &tracker ]()->void {
      TTracker tracker_copy = tracker;
      /* scope */ {
        std::lock_guard<std::mutex> lock(mutex);
        ++ready;
      }  // scope
      cv.notify_one();
      for (int i = 0; i < 1000; ++i) {
        EXPECT_TRUE(tracker_copy);
        TTracker copy = tracker_copy;
        EXPECT_TRUE(copy);
      }  // for
    };
    // Launch num_threads threads with jobs.
    std::vector<std::future<void>> futures(num_threads);
    for (auto &future : futures) {
      future = std::async(std::launch::async, job);
    }  // for
    /* scope */ {
      std::unique_lock<std::mutex> lock(mutex);
      cv.wait(lock, [&ready, &num_threads]() { return ready == num_threads; });
    }
    // Release the original tracker since it's all been copied to the threads.
    tracker.Reset();
    for (const auto &future : futures) {
      future.wait();
    }  // for
  }  // scope
  EXPECT_EQ(TObj::ConstructCount, 1);  // TObj constructs once.
  EXPECT_EQ(TObj::MoveCount, 1);       // TObj moves into TState
  EXPECT_EQ(TObj::CopyCount, 0);       // TObj does not get copied.
  EXPECT_EQ(TObj::DestroyCount,
            TObj::ConstructCount + TObj::MoveCount + TObj::CopyCount);
}

FIXTURE(MultiThreadedCopy) {
  TObj::Reset();
  /* scope */ {
    std::mutex mutex;
    std::condition_variable cv;
    int ready = 0;
    const int num_threads = 50;
    TTracker tracker = TTracker::New(TObj{});
    EXPECT_TRUE(tracker);
    /* The job of each thread is to copy the tracker in, then signal ready. Once
       the threads have signalled, the threads can keep copying the object
       within a loop, and none of these should be creating copies. */
    auto job = [&cv, &mutex, &ready, &tracker]() -> void {
      for (int i = 0; i < 100; ++i) {
        TTracker tracker_copy = tracker;
        Mutate(tracker_copy);
      }  // for
    };
    // Launch num_threads threads with jobs.
    std::vector<std::future<void>> futures(num_threads);
    for (auto &future : futures) {
      future = std::async(std::launch::async, job);
    }  // for
    for (const auto &future : futures) {
      future.wait();
    }  // for
  }  // scope
  EXPECT_EQ(TObj::ConstructCount, 1);  // TObj constructs once.
  EXPECT_EQ(TObj::MoveCount, 1);       // TObj moves into TState
  EXPECT_EQ(TObj::CopyCount, 5000);    // TObj does not get copied.
  EXPECT_EQ(TObj::DestroyCount,
            TObj::ConstructCount + TObj::MoveCount + TObj::CopyCount);
}

FIXTURE(AcrossFamilyCopy) {
  /* Success. */ {
    TFoo foo = TFoo::New(42);
    TBar bar(foo);
    EXPECT_EQ(ToString(foo), "42");
    EXPECT_EQ(ToString(bar), "42");
  }
  /* Failure. */ {
    EXPECT_THROW(std::bad_cast, []() {
      TFoo foo = TFoo::New(std::vector<TFoo>{TFoo::New(23)});
      TBar bar(foo);
    });
  }
}

FIXTURE(AcrossFamilyMove) {
  /* Success. */ {
    TFoo foo = TFoo::New(42);
    TBar bar(std::move(foo));
    EXPECT_FALSE(foo);
    EXPECT_EQ(ToString(bar), "42");
  }
  /* Failure. */ {
    EXPECT_THROW(std::bad_cast, []() {
      TFoo foo = TFoo::New(std::vector<TFoo>{TFoo::New(23)});
      TBar bar(std::move(foo));
    });
  }
}
