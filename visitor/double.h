/* <visitor/double.h>

   Our double visitor template magic.
   The entities of importance here are TRhsVisitor<>, TLhsVisitor<>, TVisitor<>, and Accept().

   TVisitor<> is the abstract double visitor class which is actually only useful for the purposes of letting the compiler confirm
   that you have covered all of the cases. While it's of course good practice to do that, things will still work if you don't as
   long as you've covered the cases.

   TLhsVisitor<> and TRhsVisitor<> are templates that generate the dispatch path for a double visitor. TLhsVisitor<> caches the
   rhs object while figuring out the dynamic type of lhs. Once the dynamic type lhs known, we dispatch to TRhsVisitor to figure
   out the rhs this time, while having cached the lhs's dynamic type. Once we know both dynamic types we forward both of them over
   to the double visitor.

   You don't actually have to ever worry about TLhsVisitor<> and TRhsVisitor stuff because all of that is hidden inside the
   Accept() function. All you need to do is define some double visitor, pass it to the Accept() function, the Accept() function
   will generate the corresponding TLhsVisitor<> and TRhsVisitor<> and do the dispatches for you.

   For example, suppose we want to write an intersects visitor for TShape.

     // We inherit from the abstract double visitor so that the compiler can confirm that we've covered all the cases.
     class TIntersectsVisitor : public Visitor::Double::TVisitor<TShape::TVisitor, TShape::TVisitor> {
       public:

       TIntersectsVisitor(double &result) : Result(result) {}

       virtual void operator()(const TCircle *, const TCircle *) const override { Result = ...; }
       virtual void operator()(const TCircle *, const TSquare *) const override { Result = ...; }
       virtual void operator()(const TCircle *, const TTriangle *) const override { Result = ...; }
       virtual void operator()(const TSquare *, const TCircle *) const override { Result = ...; }
       virtual void operator()(const TSquare *, const TSquare *) const override { Result = ...; }
       virtual void operator()(const TSquare *, const TTriangle *) const override { Result = ...; }
       virtual void operator()(const TTriangle *, const TCircle *) const override { Result = ...; }
       virtual void operator()(const TTriangle *, const TSquare *) const override { Result = ...; }
       virtual void operator()(const TTriangle *, const TTriangle *) const override { Result = ...; }

       private:

       double &Result;

     };  // TIntersectsVisitor

   Now we can use it like:

     TShape *lhs = new TCircle(...);
     TShape *rhs = new TSquare(...);
     Visitor::Double::Accept(lhs, rhs, TIntersectsVisitor());

   Again, just define a double visitor and let Accept() take care of TLhsVisitor<> and TRhsVisitor<> for you :).

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
#include <visitor/single.h>
#include <visitor/util.h>

namespace Visitor {

  namespace Double {

    /* Implementation of a general rhs visitor through template instantiation.  It generates an rhs visitor which calls the
       corresponding function in the double visitor.  The curiously recurring template pattern is used here to allow recursive
       cases to have access of the Lhs and Visitor that is stored in TRhsVisitor.

       The implementation is stashed in this helper type to avoid cluttering the surrounding namespace with intermediate
       templates.  The useful template is aliased below as TRhsVisitor<>. */
    struct RhsVisitorImpl {
      NO_CONSTRUCTION(RhsVisitorImpl);
      private:

      /* Forward declaration. */
      template <typename TFinalRhsVisitor, typename TSomeRhsVisitor, typename TRhsMembers>
      class TRhsVisitorRecur;

      /* Base case. */
      template <typename TFinalRhsVisitor, typename TSomeRhsVisitor>
      class TRhsVisitorRecur<TFinalRhsVisitor, TSomeRhsVisitor, Mpl::TTypeList<>> : public TSomeRhsVisitor {};

      /* Recursive case. */
      template <typename TFinalRhsVisitor, typename TSomeRhsVisitor, typename TRhsMember, typename... TMoreRhsMembers>
      class TRhsVisitorRecur<TFinalRhsVisitor, TSomeRhsVisitor, Mpl::TTypeList<TRhsMember, TMoreRhsMembers...>>
          : public TRhsVisitorRecur<TFinalRhsVisitor, TSomeRhsVisitor, Mpl::TTypeList<TMoreRhsMembers...>> {
        protected:

        /* Our decorated type of the rhs. */
        using TDecoratedRhs = typename TSomeRhsVisitor::template Decorator<TRhsMember>::type;

        /* Pass the finally typed lhs and rhs on to the rhs visitor. */
        virtual void operator()(TDecoratedRhs rhs) const override {
          assert(dynamic_cast<const TFinalRhsVisitor *>(this));
          static_cast<const TFinalRhsVisitor *>(this)->Visitor(static_cast<const TFinalRhsVisitor *>(this)->Lhs, rhs);
        }

      };  // TRhsVisitorRecur<TFinalRhsVisitor, TSomeRhsVisitor, Mpl::TTypeList<TRhsMember, TMoreRhsMembers...>>

      public:

      /* Final visitor. */
      template <typename TSomeRhsVisitor, typename TLhs, typename TVisitor>
      class TRhsVisitor : public TRhsVisitorRecur<TRhsVisitor<TSomeRhsVisitor, TLhs, TVisitor>,
                                                  TSomeRhsVisitor,
                                                  Mpl::TGetList<typename TSomeRhsVisitor::TMembers>> {
        public:

        /* Do-little. */
        TRhsVisitor(TLhs lhs, const TVisitor &visitor) : Lhs(lhs), Visitor(visitor) {}

        /* The finally typed lhs. */
        TLhs Lhs;

        /* Visitor we'll dispatch to. */
        const TVisitor &Visitor;

      };  // TRhsVisitor

    };  // RhsVisitorImpl

    /* Convenience type alias to bring RhsVisitorImpl::TRhsVisitor into Visitor namespace. */
    template <typename TSomeRhsVisitor, typename TLhs, typename TVisitor>
    using TRhsVisitor = RhsVisitorImpl::TRhsVisitor<TSomeRhsVisitor, TLhs, TVisitor>;

    /* Implementation of a general lhs visitor through template instantiation.  It generates an lhs visitor which constructs a
       TRhsVisitor and the stored Rhs will accept the constructed rhs visitor. The curiously recurring template pattern is used
       here to allow recursive cases to have access of the Rhs and Visitor that is stored in TLhsVisitor.

       The implementation is stashed in this helper type to avoid cluttering the surrounding namespace with intermediate
       templates.  The useful template is aliased below as TLhsVisitor<>. */
    struct LhsVisitorImpl {
      NO_CONSTRUCTION(LhsVisitorImpl);
      private:

      /* Forward declaration. */
      template <typename TFinalLhsVisitor,
                typename TSomeLhsVisitor,
                typename TSomeRhsVisitor,
                typename TVisitor,
                typename TLhsMembers>
      class TLhsVisitorRecur;

      /* Base case. */
      template <typename TFinalLhsVisitor, typename TSomeLhsVisitor, typename TSomeRhsVisitor, typename TVisitor>
      class TLhsVisitorRecur<TFinalLhsVisitor,
                             TSomeLhsVisitor,
                             TSomeRhsVisitor,
                             TVisitor,
                             Mpl::TTypeList<>> : public TSomeLhsVisitor {};

      /* Recursive case. */
      template <typename TFinalLhsVisitor,
                typename TSomeLhsVisitor,
                typename TSomeRhsVisitor,
                typename TVisitor,
                typename TLhsMember,
                typename... TMoreLhsMembers>
      class TLhsVisitorRecur<TFinalLhsVisitor,
                             TSomeLhsVisitor,
                             TSomeRhsVisitor,
                             TVisitor,
                             Mpl::TTypeList<TLhsMember, TMoreLhsMembers...>>
          : public TLhsVisitorRecur<TFinalLhsVisitor,
                                    TSomeLhsVisitor,
                                    TSomeRhsVisitor,
                                    TVisitor,
                                    Mpl::TTypeList<TMoreLhsMembers...>> {
        protected:

        /* Our decorated type of the rhs. */
        using TDecoratedLhs = typename TSomeLhsVisitor::template Decorator<TLhsMember>::type;

        /* Pass the finally typed lhs on to the rhs visitor. */
        virtual void operator()(TDecoratedLhs lhs) const override {
          assert(dynamic_cast<const TFinalLhsVisitor *>(this));
          DerefIfPtr(static_cast<const TFinalLhsVisitor *>(this)->Rhs).Accept(
              TRhsVisitor<TSomeRhsVisitor, TDecoratedLhs, TVisitor>(lhs, static_cast<const TFinalLhsVisitor *>(this)->Visitor));
        }

      };  // TLhsVisitorRecur<...>

      public:

      /* Final visitor. */
      template <typename TSomeLhsVisitor, typename TSomeRhsVisitor, typename TRhs, typename TVisitor>
      class TLhsVisitor : public TLhsVisitorRecur<TLhsVisitor<TSomeLhsVisitor, TSomeRhsVisitor, TRhs, TVisitor>,
                                                  TSomeLhsVisitor,
                                                  TSomeRhsVisitor,
                                                  TVisitor,
                                                  Mpl::TGetList<typename TSomeLhsVisitor::TMembers>> {
        public:

        /* Do-little. */
        TLhsVisitor(TRhs rhs, const TVisitor &visitor) : Rhs(rhs), Visitor(visitor) {}

        /* Our rhs that we'll pass to TRhsVisitor to visit. */
        TRhs Rhs;

        /* Visitor we cache and pass to TRhsVisitor. */
        const TVisitor &Visitor;

      };  // TLhsVisitor

    };  // LhsVisitorImpl

    /* Convenience type alias to bring LhsVisitorImpl::TLhsVisitor into Visitor namespace. */
    template <typename TSomeLhsVisitor, typename TSomeRhsVisitor, typename TRhs, typename TVisitor>
    using TLhsVisitor = LhsVisitorImpl::TLhsVisitor<TSomeLhsVisitor, TSomeRhsVisitor, TRhs, TVisitor>;

    /* Implementation of an abstract double visitor through template instantiation.  TVisitor takes 2 single visitors
       as opposed to single visitors which take a decorator and a set of members.  The double visitor figures out the
       lhs and rhs decorators and members through the visitors.

       The implementation is stashed in this helper type to avoid cluttering the surrounding namespace with intermediate
       templates.  The useful template is aliased below as TVisitor<>. */
    struct VisitorImpl {
      NO_CONSTRUCTION(VisitorImpl);
      private:

      /* Forward declaration. */
      template <template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMembers,
                typename TRhsMembers>
      class TVisitorRecur;

      /* Base case. */
      template <template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMember,
                typename TRhsMember>
      class TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TRhsMember>> {
        protected:

        /* Pure virtual function for the last pair of members. */
        virtual void operator()(typename LhsDecorator<TLhsMember>::type, typename RhsDecorator<TRhsMember>::type) const = 0;

      };  // TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TRhsMember>>

      /* Single recursive case. */
      template <template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMember,
                typename TRhsMember,
                typename... TMoreRhsMembers>
      class TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TRhsMember, TMoreRhsMembers...>>
          : public TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TMoreRhsMembers...>> {
        protected:

        /* Bring in the operator() from the parent class. */
        using TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TMoreRhsMembers...>>::operator();

        /* Pure virtual function for current pair of members. */
        virtual void operator()(typename LhsDecorator<TLhsMember>::type, typename RhsDecorator<TRhsMember>::type) const = 0;

      };  // TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TRhsMember, TMoreRhsMembers...>>

      /* Double recursive case. */
      template <template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMember,
                typename... TMoreLhsMembers,
                typename TRhsMembers>
      class TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember, TMoreLhsMembers...>, TRhsMembers>
          : public TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, TRhsMembers>,
            public TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TMoreLhsMembers...>, TRhsMembers> {
        protected:

        /* Bring in the operator() from the recursive cases. */
        using TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, TRhsMembers>::operator();
        using TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TMoreLhsMembers...>, TRhsMembers>::operator();

      };  // TVisitorRecur<LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember, TMoreLhsMembers...>, TRhsMembers>

      public:

      /* Final visitor. */
      template <typename TSomeLhsVisitor, typename TSomeRhsVisitor>
      class TVisitor : public TVisitorRecur<TSomeLhsVisitor::template Decorator,
                                            TSomeRhsVisitor::template Decorator,
                                            Mpl::TGetList<typename TSomeLhsVisitor::TMembers>,
                                            Mpl::TGetList<typename TSomeRhsVisitor::TMembers>> {
        public:

        /* Type alises that Accept() will look for. */
        using TLhsVisitor = TSomeLhsVisitor;
        using TRhsVisitor = TSomeRhsVisitor;

        /* We now have all of the operator()s defined in the recursive cases. */
        using TVisitorRecur<TSomeLhsVisitor::template Decorator,
                            TSomeRhsVisitor::template Decorator,
                            Mpl::TGetList<typename TSomeLhsVisitor::TMembers>,
                            Mpl::TGetList<typename TSomeRhsVisitor::TMembers>>::operator();

      };  // TVisitor

    };  // VisitorImpl

    /* Convenience type alias to bring VisitorImpl::TVisitor into Visitor namespace. */
    template <typename TLhsVisitor, typename TRhsVisitor>
    using TVisitor = VisitorImpl::TVisitor<TLhsVisitor, TRhsVisitor>;

    /* Given 2 bases and a double visitor, we set up the necessary components for double dispatch and start the process. */
    template <typename TLhs, typename TRhs, typename TVisitor>
    void Accept(TLhs &&lhs, TRhs &&rhs, const TVisitor &visitor) {
      using lhs_visitor_t = TLhsVisitor<typename TVisitor::TLhsVisitor, typename TVisitor::TRhsVisitor, TRhs, TVisitor>;
      DerefIfPtr(std::forward<TLhs>(lhs)).Accept(lhs_visitor_t(std::forward<TRhs>(rhs), visitor));
    }

  }  // Double

}  // Visitor
