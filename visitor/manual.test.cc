/* <visitor/manual.test.cc>

   Unit test for <visitor/visitor.h>.

   The template magic that is available in the library to generate the abstract visitors is demonstrated in
   <visitor/auto.test.cc>. This file shows what it would look like to manually write out (probably code gen) the
   abstract visitors instead in order to save compile time.

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

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>

#include <base/no_copy_semantics.h>
#include <mpl/type_set.h>

#include <test/kit.h>

/* The demonstration is done with our typical/classic shape hierarchy.
   You can skip to the TShape class for the meat of the content. */
namespace Shape {

  /* True iff. a value lies within (or on) bounds. */
  bool InBounds(double lower, double val, double upper) { return lower <= val && val <= upper; }

  /* A point in 2-space. */
  class TPoint {
    public:

    /* Default-construct at the origin. */
    TPoint() : X(0), Y(0) {}

    /* Cache the coordinates. */
    TPoint(double x, double y) : X(x), Y(y) {}

    /* The sum of this point and that one. */
    TPoint operator+(const TPoint &that) const {
      assert(this);
      assert(&that);
      return TPoint(X + that.X, Y + that.Y);
    }

    /* The difference of this point and that one. */
    TPoint operator-(const TPoint &that) const {
      assert(this);
      assert(&that);
      return TPoint(X - that.X, Y - that.Y);
    }

    /* The dot-product of this point and that one. */
    double operator*(const TPoint &that) const {
      assert(this);
      assert(&that);
      return (X * that.X) + (Y * that.Y);
    }

    /* This point scaled up. */
    TPoint operator*(double that) const {
      assert(this);
      return TPoint(X * that, Y * that);
    }

    /* This point scaled down. */
    TPoint operator/(double that) const {
      assert(this);
      assert(that);
      return TPoint(X / that, Y / that);
    }

    /* The distance between this point and that one. */
    double operator, (const TPoint &that) const {
      assert(this);
      assert(&that);
      TPoint diff = that - *this;
      return sqrt((diff.X * diff.X) + (diff.Y * diff.Y));
    }

    /* The distance of this point from the origin. */
    double GetMagnitude() const {
      assert(this);
      return *this, TPoint();
    }

    /* This point, scaled to the unit circle. */
    TPoint GetNormal() const {
      assert(this);
      return *this / GetMagnitude();
    }

    /* Our coordinates. */
    double X, Y;

  };  // TPoint

  /* A line segment defined by two endpoints. */
  class TLineSegment {
    public:

    /* Default construct as the a degenerate case. */
    TLineSegment() {}

    /* Cache the endpoints */
    TLineSegment(const TPoint &p1, const TPoint &p2) : P1(p1), P2(p2) {}

    /* Our endpoints. */
    TPoint P1, P2;

  };  // TLineSegment

  /* The base class for all shapes. */
  class TShape {
    NO_COPY_SEMANTICS(TShape);
    public:

    /* 1. Forward declaration. The definition of TVisitor will follow after the final classes are fully defined. */
    class TVisitor;

    /* Do-little. */
    virtual ~TShape() {}

    /* Override to accept the visitor. */
    virtual void Accept(const TVisitor &visitor) const = 0;

    protected:

    /* Do-little. */
    TShape() {}

  };  // TShape

  /* 2. Accept() functions of final classes are left to be defined later. */

  /* A circle, defined by center and radius. */
  class TCircle final : public TShape {
    public:

    /* Do-little. */
    TCircle(const TPoint &center, double radius) : Center(center), Radius(radius) { assert(radius >= 0); }

    /* See base class.. */
    virtual void Accept(const TVisitor &visitor) const override;

    /* The center of the circle. */
    const TPoint &GetCenter() const {
      assert(this);
      return Center;
    }

    /* The radius of the circle. */
    double GetRadius() const {
      assert(this);
      return Radius;
    }

    private:

    /* See accessor. */
    TPoint Center;

    /* See accessor. */
    double Radius;

  };  // TCircle

  /* A rectangle, defined by a pair of points. */
  class TRectangle final : public TShape {
    public:

    /* Do-little. */
    TRectangle(const TPoint &p1, const TPoint &p2)
        : Top(std::min(p1.Y, p2.Y)), Left(std::min(p1.X, p2.X)), Bottom(std::max(p1.Y, p2.Y)), Right(std::max(p1.X, p2.X)) {}

    /* See base class.. */
    virtual void Accept(const TVisitor &visitor) const override;

    /* Call back for each edge of the rectangle. */
    bool ForEachEdge(const std::function<bool(const TLineSegment &)> &cb) const {
      assert(this);
      assert(&cb);
      return cb(GetTopEdge()) && cb(GetBottomEdge()) && cb(GetLeftEdge()) && cb(GetRightEdge());
    }

    /* The vertical max of the rectangle. */
    double GetBottom() const {
      assert(this);
      return Bottom;
    }

    /* The line segment forming the bottom edge of the rectangle. */
    TLineSegment GetBottomEdge() const { return TLineSegment(GetBottomLeft(), GetBottomRight()); }

    /* The bottom-left point of the rectangle. */
    TPoint GetBottomLeft() const {
      assert(this);
      return TPoint(Left, Bottom);
    }

    /* The bottom-right point of the rectangle. */
    TPoint GetBottomRight() const {
      assert(this);
      return TPoint(Right, Bottom);
    }

    /* The height of the rectangle. */
    double GetHeight() const {
      assert(this);
      return Bottom - Top;
    }

    /* The horizontal min of the rectangle. */
    double GetLeft() const {
      assert(this);
      return Left;
    }

    /* The line segment forming the left edge of the rectangle. */
    TLineSegment GetLeftEdge() const { return TLineSegment(GetTopLeft(), GetBottomLeft()); }

    /* The horizontal max of the rectangle. */
    double GetRight() const {
      assert(this);
      return Right;
    }

    /* The line segment forming the right edge of the rectangle. */
    TLineSegment GetRightEdge() const { return TLineSegment(GetTopRight(), GetBottomRight()); }

    /* The vertical min of the rectangle. */
    double GetTop() const {
      assert(this);
      return Top;
    }

    /* The line segment forming the top edge of the rectangle. */
    TLineSegment GetTopEdge() const { return TLineSegment(GetTopLeft(), GetTopRight()); }

    /* The top-left point of the rectangle. */
    TPoint GetTopLeft() const {
      assert(this);
      return TPoint(Left, Top);
    }

    /* The top-right point of the rectangle. */
    TPoint GetTopRight() const {
      assert(this);
      return TPoint(Right, Top);
    }

    /* The width of the rectangle. */
    double GetWidth() const {
      assert(this);
      return Right - Left;
    }

    private:

    /* See accessors. */
    double Top, Left, Bottom, Right;

  };  // TRectangle

  /* 3. Handrolled abstract visitor. */
  class TVisitor {
    public:

    /* 4. The decorator for the final classes.  Visitor::TApplier and Visitor::Double::TVisitor will look for this. */
    template <typename T>
    using Decorator = Visitor::Cref<T>;

    /* 5. The list of final classes.  Visitor::TApplier looks for this. */
    using TMembers = Mpl::TTypeSet<TCircle, TRectangle>;

    virtual void operator()(typename Decorator<TCircle   >::type) const = 0;
    virtual void operator()(typename Decorator<TRectangle>::type) const = 0;
  };

  /* 6. Define TShape::TVisitor. We inherit from the manually written out abstract visitor this time. */
  class TShape::TVisitor : public Shape::TVisitor {};

  /* Handrolled double visitor. */
  class TDoubleVisitor {
    public:

    /* 7. If you handroll a double visitor, TLhsVisitor and TRhsVisitor members are necessary.
       They are single visitors of each of the sides. In this case our lhs and rhs are both TShapes. */
    using TLhsVisitor = TShape::TVisitor;
    using TRhsVisitor = TShape::TVisitor;

    virtual void operator()(typename TLhsVisitor::Decorator<TCircle   >::type,
                            typename TRhsVisitor::Decorator<TCircle   >::type) const = 0;
    virtual void operator()(typename TLhsVisitor::Decorator<TCircle   >::type,
                            typename TRhsVisitor::Decorator<TRectangle>::type) const = 0;
    virtual void operator()(typename TLhsVisitor::Decorator<TRectangle>::type,
                            typename TRhsVisitor::Decorator<TCircle   >::type) const = 0;
    virtual void operator()(typename TLhsVisitor::Decorator<TRectangle>::type,
                            typename TRhsVisitor::Decorator<TRectangle>::type) const = 0;
  };

  /* 8. Visitor::Single creates an alias "namespace" for TComputer and TApplier. */
  using Single = Visitor::Alias::Single<TShape::TVisitor>;
  using Double = Visitor::Alias::Double<TShape::TVisitor, TShape::TVisitor, TDoubleVisitor>;

  /* 9. Finish the definitions for the final classes' Accept() functions. */

  void TCircle::Accept(const TVisitor &visitor) const {
    assert(this);
    assert(&visitor);
    visitor(*this);
  }

  void TRectangle::Accept(const TVisitor &visitor) const {
    assert(this);
    assert(&visitor);
    visitor(*this);
  }

}  // Shape

/* The area of the shape. */
double GetArea(const Shape::TShape &shape) {
  using super_t = Shape::Single::TComputer<double>;
  class visitor_t
      : public super_t {
    public:
    visitor_t(TResult &result)
        : super_t(result) {}
    virtual void operator()(const Shape::TCircle &that) const override {
      Result = M_PI * pow(that.GetRadius(), 2.0);
    }
    virtual void operator()(const Shape::TRectangle &that) const override {
      Result = that.GetWidth() * that.GetHeight();
    }
  };
  assert(&shape);
  double result;
  Visitor::Single::Accept(shape, visitor_t(result));  /* 10. First method to Accept(). */
  return result;
};

/* True iff. the two shapes intersect. */
bool Intersects(const Shape::TShape &s1, const Shape::TShape &s2) {
  using super_t = Shape::Double::TComputer<bool>;
  class visitor_t
      : public super_t {
    public:
    visitor_t(TResult &result)
        : super_t(result) {}
    virtual void operator()(const Shape::TCircle &lhs, const Shape::TCircle &rhs) const override {
      Result = (lhs.GetCenter(), rhs.GetCenter()) <= lhs.GetRadius() + rhs.GetRadius();
    }
    virtual void operator()(const Shape::TCircle &lhs, const Shape::TRectangle &rhs) const override {
      const Shape::TPoint &ctr = lhs.GetCenter();
      double radius = lhs.GetRadius();
      Result = !rhs.ForEachEdge(
          [&lhs, &ctr, radius](const Shape::TLineSegment &edge) {
          Shape::TPoint
                v1 = edge.P2 - edge.P1,
                v2 = ctr - edge.P1;
            double dot = v2 * v1.GetNormal();
            return (ctr, ((dot <= 0) ? edge.P1 : (dot >= v1.GetMagnitude()) ? edge.P2 : edge.P1 + (v1 * dot))) > radius;
          }
      );
    }
    virtual void operator()(const Shape::TRectangle &lhs, const Shape::TCircle &rhs) const override {
      (*this)(rhs, lhs);
    }
    virtual void operator()(const Shape::TRectangle &lhs, const Shape::TRectangle &rhs) const override {
      Result =
          lhs.GetLeft() <= rhs.GetRight() && lhs.GetRight() >= rhs.GetLeft() &&
          lhs.GetTop() <= rhs.GetBottom() && lhs.GetBottom() >= rhs.GetTop();
    }
  };
  return Visitor::Double::Accept<visitor_t>(s1, s2);  /* 11. Second method to Accept(). */
};

using namespace Shape;

FIXTURE(GetArea) {
  EXPECT_EQ(GetArea(TCircle(TPoint(), 2)), M_PI * 4.0);
  EXPECT_EQ(GetArea(TRectangle(TPoint(-1, -1), TPoint(1, 1))), 4.0);
}

FIXTURE(Intersects) {
  EXPECT_TRUE(Intersects(TCircle(TPoint(0, 0), 1), TCircle(TPoint(1, 1), 1)));
  EXPECT_FALSE(Intersects(TCircle(TPoint(0, 0), 1), TCircle(TPoint(2, 2), 1)));
  EXPECT_TRUE(Intersects(TRectangle(TPoint(0, 0), TPoint(2, 2)), TRectangle(TPoint(1, 1), TPoint(3, 3))));
  EXPECT_FALSE(Intersects(TRectangle(TPoint(0, 0), TPoint(1, 1)), TRectangle(TPoint(2, 2), TPoint(3, 3))));
  EXPECT_TRUE(Intersects(TCircle(TPoint(0, 0), 2), TRectangle(TPoint(1, 1), TPoint(3, 3))));
  EXPECT_FALSE(Intersects(TCircle(TPoint(0, 0), 1), TRectangle(TPoint(2, 2), TPoint(3, 3))));
}
