/* <visitor/computer.h>

   Introduces a TComputer<> which is a visitor which has some result, and an Accepto() for it.

   A TComputer<> augments a visitor and adds a Result field to it.

   Consider a visitor which calculates the area of a shape. We would have something like:

     class TGetAreaVisitor : public TShape::TVisitor {
       public:

       TGetAreaVisitor(double &result) : Result(result) {}

       virtual void operator()(const TCircle *) { Result = ...; }
       virtual void operator()(const TSquare *) { Result = ...; }
       virtual void operator()(const TTriangle *) { Result = ...; }

       private:

       double &Result;

     };  // TGetAreaVisitor

   Using TComputer<>, this becomes,

     class TGetAreaVisitor : public TComputer<double, TShape::TVisitor> {
       public:

       using TSuper = TComputer<double, TShape::TVisitor>;

       using TSuper::TSuper;

       virtual void operator()(const TCircle *) { Result = ...; }
       virtual void operator()(const TSquare *) { Result = ...; }
       virtual void operator()(const TTriangle *) { Result = ...; }

     };  // TGetAreaVisitor

   Now, given that we have a TResult type alias, we can make Accept() handle the default constructing and passing in of the result
   out parameter for us.

   Instead of,

     double result;
     shape->Accept(TGetAreaVisitor(result));
     // Use result

   We say:

     auto result = Visitor::Single::Accept<TGetAreaVisitor>(shape);
     // Use result

   Accept() will default-construct an object of type TGetAreaVisitor::TResult and construct TGetAreaVisitor with it as an argument
   just like the former case.

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

#pragma once

#include <visitor/single.h>
#include <visitor/double.h>

namespace Visitor {

  /* Generic visitor which has some result. */
  template <typename TResult_, typename TSomeVisitor>
  class TComputer : public TSomeVisitor {
    public:

    /* Bring result into scope. */
    using TResult = TResult_;

    protected:

    /* Do-little. */
    TComputer(TResult &result) : Result(result) {}

    /* Reference to our result out parameter. */
    TResult &Result;

  };  // TComputer<TResult_, TSomeVisitor>

  /* Specific case for void. */
  template <typename TSomeVisitor>
  class TComputer<void, TSomeVisitor> : public TSomeVisitor {
    public:

    /* Result is void in this case. */
    using TResult = void;

    protected:

    /* Do-nothing. */
    TComputer() {}

  };  // TComputer<void, TSomeVisitor>

  template <typename TResult, typename TVisitor>
  struct TAcceptor;

  /* General Acceptor. */
  template <typename TResult, typename TVisitor>
  struct TAcceptor {

    /* Single accept. */
    template <typename TBase, typename... TArgs>
    static TResult Single(TBase &&base, TArgs &&... args) {
      TResult result;
      Single::Accept(std::forward<TBase>(base), TVisitor(result, std::forward<TArgs>(args)...));
      return result;
    }

    /* Double accept. */
    template <typename TLhs, typename TRhs, typename... TArgs>
    static TResult Double(TLhs &&lhs, TRhs &&rhs, TArgs &&... args) {
      TResult result;
      Double::Accept(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs), TVisitor(result, std::forward<TArgs>(args)...));
      return result;
    }
  };

  template <typename TResult, typename TVisitor>
  struct TAcceptor<TResult &, TVisitor> {

    /* Single accept. */
    template <typename TBase, typename... TArgs>
    static TResult &Single(TBase &&base, TArgs &&... args) {
      TResult *result = nullptr;
      Single::Accept(std::forward<TBase>(base), TVisitor(result, std::forward<TArgs>(args)...));
      return *result;
    }

    /* Double accept. */
    template <typename TLhs, typename TRhs, typename... TArgs>
    static TResult &Double(TLhs &&lhs, TRhs &&rhs, TArgs &&... args) {
      TResult *result = nullptr;
      Double::Accept(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs), TVisitor(result, std::forward<TArgs>(args)...));
      return *result;
    }
  };

  /* Special case for void. */
  template <typename TVisitor>
  struct TAcceptor<void, TVisitor> {

    /* Single accept. */
    template <typename TBase, typename... TArgs>
    static void Single(TBase &&base, TArgs &&... args) {
      Single::Accept(std::forward<TBase>(base), TVisitor(std::forward<TArgs>(args)...));
    }

    /* Double accept. */
    template <typename TLhs, typename TRhs, typename... TArgs>
    static void Double(TLhs &&lhs, TRhs &&rhs, TArgs &&... args) {
      Double::Accept(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs), TVisitor(std::forward<TArgs>(args)...));
    }
  };

  namespace Single {

    /* Single accept with a result. Dispatches to the correct TAcceptor depending on TVisitor::TResult. */
    template <typename TVisitor, typename TBase, typename... TArgs>
    auto Accept(TBase &&base, TArgs &&... args)
        DECLTYPE_AUTO((TAcceptor<typename TVisitor::TResult, TVisitor>::Single(std::forward<TBase>(base),
                                                                               std::forward<TArgs>(args)...)));

  }  // Single

  namespace Double {

    /* Double accept with a result. Dispatches to the correct TAcceptor depending on TVisitor::TResult. */
    template <typename TVisitor, typename TLhs, typename TRhs, typename... TArgs>
    auto Accept(TLhs &&lhs, TRhs &&rhs, TArgs &&... args)
        DECLTYPE_AUTO((TAcceptor<typename TVisitor::TResult, TVisitor>::Double(std::forward<TLhs>(lhs),
                                                                               std::forward<TRhs>(rhs),
                                                                               std::forward<TArgs>(args)...)));

  }  // Double

}  // Visitor
