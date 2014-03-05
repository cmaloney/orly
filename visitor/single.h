/* <visitor/single.h>

   A single visitor.  (That is, not a double visitor.)

   The TVisitor<> class in here generates an abstract single visitor for you. It takes a type modifier (I call it Decorator here),
   and a list of types, and defines pure virtual operator()s for each of the types decorated with the Decorator.

   For example, if we wanted:

     class TVisitor {
       public:

       virtual void operator()(const TCircle *) {}
       virtual void operator()(const TSquare *) {}
       virtual void operator()(const TTriangle *) {}

     };  // TVisitor

   We can say:

     using TVisitor = Visitor::Single::TVisitor<Visitor::Cptr, Mpl::TypeSet<TCircle, TSquare, TTriangle>>;

   You can imagine as the number of final types get larger, this gets more and more convenient for us, which means it's less
   and less convenient for the compiler... so win!. (The compile time actually is negligible for single visitors for even up to
   100 types, but it does get kind of bad for double visitors)

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

#include <c14/lang.h>
#include <cassert>
#include <tuple>

#include <base/no_construction.h>
#include <mpl/type_list.h>
#include <mpl/type_set.h>
#include <visitor/util.h>

namespace Visitor {

  namespace Single {

    /* Implementation of an abstract visitor generator through template instantiation. TVisitor takes a decorator and and a set
       of members and defines a pure virtual functions for the members decorated with the given decorator.

       For example, Suppose we're given Cref defined in <pass.h>, and set of types X, Y, Z.
       Then the visitor would define pure virtual functions defined are:

          virtual void operator()(typename Cref<X>::type) const = 0;  ->  virtual void operator()(const X &) const = 0;
          virtual void operator()(typename Cref<Y>::type) const = 0;  ->  virtual void operator()(const Y &) const = 0;
          virtual void operator()(typename Cref<Z>::type) const = 0;  ->  virtual void operator()(const Z &) const = 0;

       The implementation is stashed in this helper type to avoid cluttering the surrounding namespace with intermediate
       templates.  The useful template is aliased below as TVisitor<>. */
    class VisitorImpl {
      NO_CONSTRUCTION(VisitorImpl);
      private:

      /* Forward declaration. */
      template <template <typename> class Decorator, typename TMembers>
      class TVisitorRecur;

      /* Base case. */
      template <template <typename> class Decorator, typename TMember>
      class TVisitorRecur<Decorator, Mpl::TTypeList<TMember>> {
        protected:

        /* Override to handle the instance of TMember. */
        virtual void operator()(typename Decorator<TMember>::type that) const = 0;

      };  // TVisitorRecur<Decorator, Mpl::TTypeList<TMember>>

      /* Recursive case. */
      template <template <typename> class Decorator, typename TMember, typename... TMoreMembers>
      class TVisitorRecur<Decorator, Mpl::TTypeList<TMember, TMoreMembers...>>
          : public TVisitorRecur<Decorator, Mpl::TTypeList<TMoreMembers...>> {
        protected:

        /* Pull in all the operator() overloads we inherit so that overload
           resolution will work properly at the call site. */
        using TVisitorRecur<Decorator, Mpl::TTypeList<TMoreMembers...>>::operator();

        /* Override to handle the instance of TMember. */
        virtual void operator()(typename Decorator<TMember>::type that) const = 0;

      };  // TVisitorRecur<Decorator, Mpl::TTypeList<TMember, TMoreMembers...>>

      public:

      /* Final visitor. */
      template <template <typename> class Decorator_, typename TMembers_>
      class TVisitor
          : public TVisitorRecur<Decorator_, Mpl::TGetList<TMembers_>> {
        public:

        /* Bring decorator into scope. */
        template <typename T>
        using Decorator = Decorator_<T>;

        /* Our set of types */
        using TMembers = TMembers_;

        /* Pull in all the operator() overloads we inherit so that overload
           resolution will work properly at the call site. */
        using TVisitorRecur<Decorator_, Mpl::TGetList<TMembers_>>::operator();

      };  // TVisitor<Decorator, TMembers_>

    };  // VisitorImpl

    /* Convenience type alias to bring VisitorImpl::TVisitor into Visitor namespace. */
    template <template <typename> class Decorator, typename TMembers>
    using TVisitor = VisitorImpl::TVisitor<Decorator, TMembers>;

    /* Given a base and a visitor, call Accept() with the visitor. It's not particularly useful since all it does is reorders the
       function and arguments, but exists to keep Accept() symmetric. */
    template <typename TBase, typename TVisitor>
    void Accept(TBase &&base, const TVisitor &visitor) {
      DerefIfPtr(std::forward<TBase>(base)).Accept(visitor);
    }

  }  // Single

}  // Visitor
