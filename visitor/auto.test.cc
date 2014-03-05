/* <visitor/auto.test.cc>

   Unit test for <visitor/visitor.h>

   This file demonstrates how to use the visitor library. It uses the template magic in the library to generate the
   abstract visitors. If there are too many final types in the family, you can manually write out (probably code gen)
   the abstract visitors. Take a look at <visitor/manual.test.cc>

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

/* The demonstration is done with our typical/classic shape hierarchy. */
namespace Shape {

  // shape.h
  class TShape {
    public:

    /* 1. Forward declaration. The definition will follow after all of the final classes are fully defined. */
    class TVisitor;

    virtual ~TShape() {}

    virtual void Accept(const TVisitor &visitor) const = 0;

  };  // TShape

  /* 2. Accept() functions of final classes are left to be defined later. */

  // circle.h
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

  // square.h
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

  // triangle.h
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

  // all_shapes.h

  /* 3. Define TShape::TVisitor. Visitor::Single::TVisitor<> is the template magic class that generates the abstract visitor. */
  class TShape::TVisitor : public Visitor::Single::TVisitor<Visitor::Cptr, Mpl::TTypeSet<TCircle, TSquare, TTriangle>> {};

  /* 4. Visitor::Single creates an alias "namespace" for TComputer and TApplier. */
  using Single = Visitor::Alias::Single<TShape::TVisitor>;
  using Double = Visitor::Alias::Double<TShape::TVisitor, TShape::TVisitor>;

  /* 5. Finish the definitions for the final classes' Accept() functions. */

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

/* Above is our convention for defining a class hierarchy. */

/* Below is a demonstration of using these visitors similar to our classic style. */

using namespace Shape;

/* 1. Note the use of Shape::TFunction<void ()> instead of Shape::TShape::TVisitor. */
class TPrintVisitor : public Shape::Single::TComputer<void> {
  public:
  virtual void operator()(const TCircle *) const override { std::cout << "TCircle" << std::endl; }
  virtual void operator()(const TSquare *) const override { std::cout << "TSquare" << std::endl; }
  virtual void operator()(const TTriangle *) const override { std::cout << "TTriangle" << std::endl; }
};

FIXTURE(Print) {
  std::unique_ptr<TShape> shape{new TSquare(5)};
  /* 2. Second way of Accept() is useful when there is a non-void return type and/or when there are extra arguments. */
  Visitor::Single::Accept(shape.get(), TPrintVisitor());
  Visitor::Single::Accept<TPrintVisitor>(shape.get());
}

// Computes a result.
class TGetAreaVisitor : public Shape::Single::TComputer<double> {
  public:
  using TSuper = Shape::Single::TComputer<double>;
  TGetAreaVisitor(TResult &result) : TSuper(result) {}
  virtual void operator()(const TCircle *that) const override { Result = 3.14 * that->GetRadius() * that->GetRadius(); }
  virtual void operator()(const TSquare *that) const override { Result = that->GetLength() * that->GetLength(); }
  virtual void operator()(const TTriangle *that) const override { Result = that->GetBase() * that->GetHeight() / 2.0; }
};

FIXTURE(GetArea) {
  /* Old. */ {
    std::unique_ptr<TShape> shape{new TSquare(5)};
    double result;
    Visitor::Single::Accept(shape.get(), TGetAreaVisitor(result));
    EXPECT_EQ(result, 25);
  }
  /* New. */ {
    std::unique_ptr<TShape> shape{new TTriangle(10, 4)};
    double result = Visitor::Single::Accept<TGetAreaVisitor>(shape.get());
    EXPECT_EQ(result, 20);
  }
}

/* No result, but takes extra arguments. */
class TWriteNameVisitor : public Shape::Single::TComputer<void> {
  public:
  TWriteNameVisitor(std::ostream &strm) : Strm(strm) {}
  virtual void operator()(const TCircle   *) const override { Strm << "TCircle";   }
  virtual void operator()(const TSquare   *) const override { Strm << "TSquare";   }
  virtual void operator()(const TTriangle *) const override { Strm << "TTriangle"; }
  private:
  std::ostream &Strm;
};

FIXTURE(WriteName) {
  /* Old */ {
    std::unique_ptr<TShape> shape{new TCircle(12)};
    std::ostringstream strm;
    Visitor::Single::Accept(shape.get(), TWriteNameVisitor(strm));
    EXPECT_EQ(strm.str(), "TCircle");
  }
  /* New */ {
    std::unique_ptr<TShape> shape{new TTriangle(10, 4)};
    std::ostringstream strm;
    Visitor::Single::Accept<TWriteNameVisitor>(shape.get(), strm);
    EXPECT_EQ(strm.str(), "TTriangle");
  }
}

/* Computes a result, and takes extra arguments. */
class TGetAreaAndWriteVectorVisitor : public Shape::Single::TComputer<double> {
  public:
  using TSuper = Shape::Single::TComputer<double>;
  TGetAreaAndWriteVectorVisitor(TResult &result, std::ostream &strm, const std::vector<int> &ints)
      : TSuper(result), Strm(strm), Ints(ints) {}
  virtual void operator()(const TCircle *that) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(that);
  }
  virtual void operator()(const TSquare *that) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(that);
  }
  virtual void operator()(const TTriangle *that) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(that);
  }
  private:
  std::ostream &Strm;
  const std::vector<int> &Ints;
};

FIXTURE(GetAreaAndWriteVector) {
  /* Old */ {
    std::unique_ptr<TShape> shape{new TCircle(23)};
    std::ostringstream strm;
    double result;
    Visitor::Single::Accept(shape.get(), TGetAreaAndWriteVectorVisitor(result, strm, std::vector<int>{0, 1, 2}));
    EXPECT_EQ(result, 3.14 * 23 * 23);
    EXPECT_EQ(strm.str(), "012");
  }
  /* New */ {
    std::unique_ptr<TShape> shape{new TSquare(50)};
    std::ostringstream strm;
    double result = Visitor::Single::Accept<TGetAreaAndWriteVectorVisitor>(shape.get(), strm, std::vector<int>{3, 4, 5});
    EXPECT_EQ(result, 2500);
    EXPECT_EQ(strm.str(), "345");
  }
}

class TPrintDoubleVisitor : public Double::TComputer<void> {
  public:

  virtual void operator()(const TCircle *, const TCircle *) const override {
    std::cout << "(TCircle, TCircle)" << std::endl;
  }
  virtual void operator()(const TCircle *, const TSquare *) const override {
    std::cout << "(TCircle, TSquare)" << std::endl;
  }
  virtual void operator()(const TCircle *, const TTriangle *) const override {
    std::cout << "(TCircle, TTriangle)" << std::endl;
  }
  virtual void operator()(const TSquare *, const TCircle *) const override {
    std::cout << "(TSquare, TCircle)" << std::endl;
  }
  virtual void operator()(const TSquare *, const TSquare *) const override {
    std::cout << "(TSquare, TSquare)" << std::endl;
  }
  virtual void operator()(const TSquare *, const TTriangle *) const override {
    std::cout << "(TSquare, TTriangle)" << std::endl;
  }
  virtual void operator()(const TTriangle *, const TCircle *) const override {
    std::cout << "(TTriangle, TCircle)" << std::endl;
  }
  virtual void operator()(const TTriangle *, const TSquare *) const override {
    std::cout << "(TTriangle, TSquare)" << std::endl;
  }
  virtual void operator()(const TTriangle *, const TTriangle *) const override {
    std::cout << "(TTriangle, TTriangle)" << std::endl;
  }
};

FIXTURE(PrintDouble) {
  std::unique_ptr<TShape> lhs{new TCircle(23)};
  std::unique_ptr<TShape> rhs{new TSquare(34)};
  /* Old */ Visitor::Double::Accept(lhs.get(), rhs.get(), TPrintDoubleVisitor());
  /* New */ Visitor::Double::Accept<TPrintDoubleVisitor>(lhs.get(), rhs.get());
}

class TGetAreaDoubleVisitor : public Shape::Double::TComputer<std::pair<double, double>> {
  public:
  using TSuper = Shape::Double::TComputer<std::pair<double, double>>;
  TGetAreaDoubleVisitor(TResult &result) : TSuper(result) {}
  virtual void operator()(const TCircle *lhs, const TCircle *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TCircle *lhs, const TSquare *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TCircle *lhs, const TTriangle *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TSquare *lhs, const TCircle *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TSquare *lhs, const TSquare *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TSquare *lhs, const TTriangle *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TTriangle *lhs, const TCircle *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TTriangle *lhs, const TSquare *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
  virtual void operator()(const TTriangle *lhs, const TTriangle *rhs) const override {
    Result = std::make_pair(Visitor::Single::Accept<TGetAreaVisitor>(lhs),
                            Visitor::Single::Accept<TGetAreaVisitor>(rhs));
  }
};

FIXTURE(GetAreaDouble) {
  /* Old */ {
    std::unique_ptr<TShape> lhs{new TTriangle(10, 4)};
    std::unique_ptr<TShape> rhs{new TSquare(30)};
    typename TGetAreaDoubleVisitor::TResult result;
    Visitor::Double::Accept(lhs.get(), rhs.get(), TGetAreaDoubleVisitor{result});
    EXPECT_EQ(result.first, 20);
    EXPECT_EQ(result.second, 900);
  }

  /* New */ {
    std::unique_ptr<TShape> lhs{new TTriangle(10, 4)};
    std::unique_ptr<TShape> rhs{new TSquare(30)};
    auto result = Visitor::Double::Accept<TGetAreaDoubleVisitor>(lhs.get(), rhs.get());
    EXPECT_EQ(result.first, 20);
    EXPECT_EQ(result.second, 900);
  }
}

class TWriteNameDoubleVisitor : public Shape::Double::TComputer<void> {
  public:
  using TSuper = Shape::Double::TComputer<void>;
  TWriteNameDoubleVisitor(std::ostream &strm) : Strm(strm) {}
  virtual void operator()(const TCircle *, const TCircle *) const override {
    Strm << "(TCircle, TCircle)";
  }
  virtual void operator()(const TCircle *, const TSquare *) const override {
    Strm << "(TCircle, TSquare)";
  }
  virtual void operator()(const TCircle *, const TTriangle *) const override {
    Strm << "(TCircle, TTriangle)";
  }
  virtual void operator()(const TSquare *, const TCircle *) const override {
    Strm << "(TSquare, TCircle)";
  }
  virtual void operator()(const TSquare *, const TSquare *) const override {
    Strm << "(TSquare, TSquare)";
  }
  virtual void operator()(const TSquare *, const TTriangle *) const override {
    Strm << "(TSquare, TTriangle)";
  }
  virtual void operator()(const TTriangle *, const TCircle *) const override {
    Strm << "(TTriangle, TCircle)";
  }
  virtual void operator()(const TTriangle *, const TSquare *) const override {
    Strm << "(TTriangle, TSquare)";
  }
  virtual void operator()(const TTriangle *, const TTriangle *) const override {
    Strm << "(TTriangle, TTriangle)";
  }
  private:
  std::ostream &Strm;
};

FIXTURE(WriteNameDouble) {
  std::unique_ptr<TShape> lhs{new TTriangle(10, 4)};
  std::unique_ptr<TShape> rhs{new TCircle(10)};
  std::ostringstream strm;
  Visitor::Double::Accept<TWriteNameDoubleVisitor>(lhs.get(), rhs.get(), strm);
  EXPECT_EQ(strm.str(), "(TTriangle, TCircle)");
}

class TGetAreaAndWriteVectorDoubleVisitor
    : public Shape::Double::TComputer<double> {
  public:
  using TSuper = Shape::Double::TComputer<double>;
  TGetAreaAndWriteVectorDoubleVisitor(TResult &result, std::ostream &strm, const std::vector<int> &ints)
      : TSuper(result), Strm(strm), Ints(ints) {}
  virtual void operator()(const TCircle *lhs, const TCircle *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TCircle *lhs, const TSquare *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TCircle *lhs, const TTriangle *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TSquare *lhs, const TCircle *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TSquare *lhs, const TSquare *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TSquare *lhs, const TTriangle *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TTriangle *lhs, const TCircle *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TTriangle *lhs, const TSquare *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  virtual void operator()(const TTriangle *lhs, const TTriangle *rhs) const override {
    for (const auto &i : Ints) {
      Strm << i;
    }
    Result = Visitor::Single::Accept<TGetAreaVisitor>(lhs) + Visitor::Single::Accept<TGetAreaVisitor>(rhs);
  }
  private:
  std::ostream &Strm;
  const std::vector<int> &Ints;
};

FIXTURE(GetAreaAndWriteVectorDouble) {
  std::unique_ptr<TShape> lhs{new TSquare(50)};
  std::unique_ptr<TShape> rhs{new TSquare(5)};
  std::ostringstream strm;
  double result = Visitor::Double::Accept<TGetAreaAndWriteVectorDoubleVisitor>(
      lhs.get(), rhs.get(), strm, std::vector<int>{3, 4, 5});
  EXPECT_EQ(result, 2525);
  EXPECT_EQ(strm.str(), "345");
}
