/* <visitor/applier.test.cc>

   Unit test for <visitor/visitor.h>

   Visitor::TApplier is a visitor that helps us we move back to top-level functions instead of having visitors all over
   the place. It takes a struct which contains all of the supported overloads for a function, and dispatches to the
   correct one. Also, if we happen to have the final type already, then we get an early binding and dispatch to it
   directly. The usage pattern is demonstrated below.

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

#include <visitor/visitor.h>

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include <tuple>

#include <mpl/type_set.h>

#include <test/kit.h>

/* If you're curious about how to lay the classes out, there's documentation for that in <visitor/auto.test.cc>.
   There's an interesting piece in here though where a few useful subsets of the final classes are in namespace Groups. */
namespace Shape {

  class TShape {
    public:

    class TVisitor;

    virtual ~TShape() {}

    virtual void Accept(const TVisitor &visitor) const = 0;

  };  // TShape

  class TCircle final : public TShape {
    public:

    TCircle(int radius) : Radius(radius) {}

    virtual void Accept(const TVisitor &visitor) const override;

    int GetRadius() const {
      assert(this);
      return Radius;
    }

    private:

    int Radius;
  };  // TCircle

  class TSquare final : public TShape {
    public:

    TSquare(int length) : Length(length) {}

    virtual ~TSquare() {}

    virtual void Accept(const TVisitor &visitor) const override;

    int GetLength() const {
      assert(this);
      return Length;
    }

    private:

    int Length;

  };  // TSquare

  class TTriangle final : public TShape {
    public:

    TTriangle(int base, int height) : Base(base), Height(height) {}

    virtual ~TTriangle() {}

    virtual void Accept(const TVisitor &visitor) const override;

    int GetBase() const {
      assert(this);
      return Base;
    }

    int GetHeight() const {
      assert(this);
      return Height;
    }

    private:

    int Base, Height;

  };  // TTriangle

  /* 1. We define a few useful subsets of the final classes here. */
  namespace Groups {

    using TAll = Mpl::TTypeSet<TCircle, TSquare, TTriangle>;
    using TPointless = Mpl::TTypeSet<TCircle>;
    using TPoint = Mpl::TTypeSet<TSquare, TTriangle>;

  }  // Groups

  class TShape::TVisitor : public Visitor::Single::TVisitor<Visitor::Cptr, Groups::TAll> {};

  using Single = Visitor::Alias::Single<TShape::TVisitor>;
  using Double = Visitor::Alias::Double<TShape::TVisitor, TShape::TVisitor>;

  void TCircle::Accept(const TVisitor &visitor) const {
    assert(this);
    assert(&visitor);
    visitor(this);
  }

  void TSquare::Accept(const TVisitor &visitor) const {
    assert(this);
    assert(&visitor);
    visitor(this);
  }

  void TTriangle::Accept(const TVisitor &visitor) const {
    assert(this);
    assert(&visitor);
    visitor(this);
  }

}  // Shape

using namespace Shape;

/**
 *   void return type, no extra parameters.
 **/

// print_name.h

/* 1. We define a 'TVerb' struct with all of the overloads for the final classes. */
struct TPrintName {
  void operator()(const TCircle *) const;
  void operator()(const TSquare *) const;
  void operator()(const TTriangle *) const;
};  // PrintName

void PrintName(const TShape *that);

/* 2. Templated 'TVerb' function fowards the member and extra arguments to the operator() functions in 'TVerb' class. */
template <typename TMember>
void PrintName(const TMember *member) {
  TPrintName()(member);
}

// print_name.cc

/* 4. In the cc file we provide the implementation of each of the overloaded functions in 'TVerb' class. */
void TPrintName::operator()(const TCircle *) const { std::cout << "TCircle" << std::endl; }
void TPrintName::operator()(const TSquare *) const { std::cout << "TSquare" << std::endl; }
void TPrintName::operator()(const TTriangle *) const { std::cout << "TTriangle" << std::endl; }

void PrintName(const TShape *that) {
  using applier_t = Single::TApplier<TPrintName>;
  Visitor::Single::Accept<applier_t>(that);
}
// print_name.test.cc
FIXTURE(PrintName) {
  /* TCircle */ {
    std::unique_ptr<TShape> shape(new TCircle(0));
    PrintName(shape.get());
  }
  /* TSquare */ {
    std::unique_ptr<TShape> shape(new TSquare(0));
    PrintName(shape.get());
  }
  /* TTriangle */ {
    std::unique_ptr<TShape> shape(new TTriangle(0, 0));
    PrintName(shape.get());
  }
}

/* Below are similar examples which follow the same pattern, just with a few extra things like return values and extra
   arguments passed to the functions. */

/**
 *  non-void return type, no extra parameters.
 **/

// get_area.h
struct TGetArea {
  double operator()(const TCircle *that) const;
  double operator()(const TSquare *that) const;
  double operator()(const TTriangle *that) const;
};

double GetArea(const TShape *that);

template <typename TMember>
double GetArea(const TMember *member) {
  return TGetArea()(member);
}

// get_area.cc
double TGetArea::operator()(const TCircle *that) const {
  assert(that);
  return 3.14 * that->GetRadius() * that->GetRadius();
}
double TGetArea::operator()(const TSquare *that) const {
  assert(that);
  return that->GetLength() * that->GetLength();
}
double TGetArea::operator()(const TTriangle *that) const {
  assert(that);
  return (that->GetBase() * that->GetHeight()) / 2;
}

double GetArea(const TShape *that) {
  using applier_t = Single::TApplier<TGetArea>;
  return Visitor::Single::Accept<applier_t>(that);
}

// get_area.test.cc
FIXTURE(GetArea) {
  /* TCircle */ {
    std::unique_ptr<TShape> shape(new TCircle(10));
    EXPECT_EQ(GetArea(shape.get()), 3.14 * 10 * 10);
  }
  /* TSquare */ {
    std::unique_ptr<TShape> shape(new TSquare(10));
    EXPECT_EQ(GetArea(shape.get()), 100);
  }
  /* TTriangle */ {
    std::unique_ptr<TShape> shape(new TTriangle(5, 4));
    EXPECT_EQ(GetArea(shape.get()), 10);
  }
}

/**
 *   void return type, extra parameters.
 **/

// write_name.h
struct TWriteName {
  void operator()(const TCircle *, std::ostream &strm) const;
  void operator()(const TSquare *, std::ostream &strm) const;
  void operator()(const TTriangle *, std::ostream &strm) const;
};

void WriteName(const TShape *that, std::ostream &strm);

template <typename TMember>
void WriteName(const TMember *member, std::ostream &strm) {
  TWriteName()(member, strm);
}

// write_name.cc
void TWriteName::operator()(const TCircle *, std::ostream &strm) const { strm << "TCircle"; }
void TWriteName::operator()(const TSquare *, std::ostream &strm) const { strm << "TSquare"; }
void TWriteName::operator()(const TTriangle *, std::ostream &strm) const { strm << "TTriangle"; }

void WriteName(const TShape *that, std::ostream &strm) {
  using applier_t = Single::TApplier<TWriteName>;
  Visitor::Single::Accept<applier_t>(that, strm);
};

// write_name.test.cc
FIXTURE(WriteName) {
  /* TCircle */ {
    std::ostringstream strm;
    std::unique_ptr<TShape> shape(new TCircle(10));
    WriteName(shape.get(), strm);
    EXPECT_EQ(strm.str(), "TCircle");
  }
  /* TSquare */ {
    std::ostringstream strm;
    std::unique_ptr<TShape> shape(new TSquare(10));
    WriteName(shape.get(), strm);
    EXPECT_EQ(strm.str(), "TSquare");
  }
  /* TTriangle */ {
    std::ostringstream strm;
    std::unique_ptr<TShape> shape(new TTriangle(5, 4));
    WriteName(shape.get(), strm);
    EXPECT_EQ(strm.str(), "TTriangle");
  }
}

/**
 *  non-void return type, extra parameters.
 **/

// get_area_and_write_vector.h
struct TGetAreaAndWriteVector {
  double operator()(const TCircle *that, std::ostream &strm, const std::vector<int> &ints) const;
  double operator()(const TSquare *that, std::ostream &strm, const std::vector<int> &ints) const;
  double operator()(const TTriangle *that, std::ostream &strm, const std::vector<int> &ints) const;
};

double GetAreaAndWriteVector(const TShape *that, std::ostream &strm, const std::vector<int> &ints);

template <typename TMember>
double GetAreaAndWriteVector(const TMember *member, std::ostream &strm, const std::vector<int> &ints) {
  return TGetAreaAndWriteVector()(member, strm, ints);
}

// get_area_and_write_vector.cc
double TGetAreaAndWriteVector::operator()(const TCircle *that, std::ostream &strm, const std::vector<int> &ints) const {
  assert(that);
  for (const auto &i : ints) {
    strm << i;
  }
  return GetArea(that);;
}
double TGetAreaAndWriteVector::operator()(const TSquare *that, std::ostream &strm, const std::vector<int> &ints) const {
  assert(that);
  for (const auto &i : ints) {
    strm << i;
  }
  return GetArea(that);
}
double TGetAreaAndWriteVector::operator()(const TTriangle *that, std::ostream &strm, const std::vector<int> &ints) const {
  assert(that);
  for (const auto &i : ints) {
    strm << i;
  }
  return GetArea(that);
}

double GetAreaAndWriteVector(const TShape *that, std::ostream &strm, const std::vector<int> &ints) {
  using applier_t = Single::TApplier<TGetAreaAndWriteVector>;
  return Visitor::Single::Accept<applier_t>(that, strm, ints);
}

// get_area_and_write_vector.test.cc
FIXTURE(GetAreaAndWriteVector) {
  /* TCircle */ {
    std::ostringstream strm;
    std::unique_ptr<TShape> shape(new TCircle(5));
    EXPECT_EQ(GetAreaAndWriteVector(shape.get(), strm, {0, 1, 2}), 3.14 * 5 * 5);
    EXPECT_EQ(strm.str(), "012");
  }
  /* TSquare */ {
    std::ostringstream strm;
    std::unique_ptr<TShape> shape(new TSquare(5));
    EXPECT_EQ(GetAreaAndWriteVector(shape.get(), strm, {3, 4, 5}), 25);
    EXPECT_EQ(strm.str(), "345");
  }
  /* TTriangle */ {
    std::ostringstream strm;
    std::unique_ptr<TShape> shape(new TTriangle(10, 5));
    EXPECT_EQ(GetAreaAndWriteVector(shape.get(), strm, {6, 7, 8}), 25);
    EXPECT_EQ(strm.str(), "678");
  }
}

/**
 *   Early binding, late binding.
 **/

// count.h
struct TCount {
  void operator()(const TCircle *, int &base, int &final) const;
  void operator()(const TSquare *, int &base, int &final) const;
  void operator()(const TTriangle *, int &base, int &final) const;
};

void Count(const TShape *that, int &base, int &final);

template <typename TMember>
void Count(const TMember *member, int &base, int &final) {
  return TCount()(member, base, final);
}

// count.cc
void TCount::operator()(const TCircle *, int &, int &final) const { ++final; }
void TCount::operator()(const TSquare *, int &, int &final) const { ++final; }
void TCount::operator()(const TTriangle *, int &, int &final) const { ++final; }

void Count(const TShape *that, int &base, int &final) {
  using applier_t = Single::TApplier<TCount>;
  ++base;
  Visitor::Single::Accept<applier_t>(that, base, final);
};

// count.test.cc
FIXTURE(EarlyBinding) {
  int base = 0, final = 0;
  std::unique_ptr<TCircle> circle(new TCircle(10));
  Count(circle.get(), base, final);
  EXPECT_EQ(base, 0);
  EXPECT_EQ(final, 1);
}

FIXTURE(LateBinding) {
  int base = 0, final = 0;
  std::unique_ptr<TShape> shape(new TSquare(10));
  Count(shape.get(), base, final);
  EXPECT_EQ(base, 1);
  EXPECT_EQ(final, 1);
}

// write_name_double.h
struct TWriteNameDouble {
  void operator()(const TCircle   *, const TCircle   *, std::ostream &) const;
  void operator()(const TCircle   *, const TSquare   *, std::ostream &) const;
  void operator()(const TCircle   *, const TTriangle *, std::ostream &) const;
  void operator()(const TSquare   *, const TCircle   *, std::ostream &) const;
  void operator()(const TSquare   *, const TSquare   *, std::ostream &) const;
  void operator()(const TSquare   *, const TTriangle *, std::ostream &) const;
  void operator()(const TTriangle *, const TCircle   *, std::ostream &) const;
  void operator()(const TTriangle *, const TSquare   *, std::ostream &) const;
  void operator()(const TTriangle *, const TTriangle *, std::ostream &) const;
};

void WriteNameDouble(const TShape *, const TShape *, std::ostream &);

template <typename TLhs, typename TRhs>
void WriteNameDouble(const TLhs *lhs, const TRhs *rhs, std::ostream &strm) {
  TWriteNameDouble()(lhs, rhs, strm);
}

// write_name_double.cc
void TWriteNameDouble::operator()(const TCircle   *, const TCircle   *, std::ostream &strm) const { strm << "(TCircle, TCircle)"; }
void TWriteNameDouble::operator()(const TCircle   *, const TSquare   *, std::ostream &strm) const { strm << "(TCircle, TSquare)"; }
void TWriteNameDouble::operator()(const TCircle   *, const TTriangle *, std::ostream &strm) const { strm << "(TCircle, TTriangle)"; }
void TWriteNameDouble::operator()(const TSquare   *, const TCircle   *, std::ostream &strm) const { strm << "(TSquare, TCircle)"; }
void TWriteNameDouble::operator()(const TSquare   *, const TSquare   *, std::ostream &strm) const { strm << "(TSquare, TSquare)"; }
void TWriteNameDouble::operator()(const TSquare   *, const TTriangle *, std::ostream &strm) const { strm << "(TSquare, TTriangle)"; }
void TWriteNameDouble::operator()(const TTriangle *, const TCircle   *, std::ostream &strm) const { strm << "(TTriangle, TCircle)"; }
void TWriteNameDouble::operator()(const TTriangle *, const TSquare   *, std::ostream &strm) const { strm << "(TTriangle, TSquare)"; }
void TWriteNameDouble::operator()(const TTriangle *, const TTriangle *, std::ostream &strm) const { strm << "(TTriangle, TTriangle)"; }

void WriteNameDouble(const TShape *lhs, const TShape *rhs, std::ostream &strm) {
  using applier_t = Double::TApplier<TWriteNameDouble>;
  Visitor::Double::Accept<applier_t>(lhs, rhs, strm);
}

// write_name_double.test.cc
FIXTURE(WriteNameDouble) {
  std::ostringstream strm;
  std::unique_ptr<TShape> lhs(new TSquare(10));
  std::unique_ptr<TShape> rhs(new TCircle(5));
  WriteNameDouble(lhs.get(), rhs.get(), strm);
  EXPECT_EQ(strm.str(), "(TSquare, TCircle)");
}
