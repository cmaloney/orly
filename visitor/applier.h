/* <visitor/applier.h>

   An applier is a special type of visitor which, after performing virtual dispatch to figure out dynamic type of a base object,
   dispatches to a set of handler functions.

   For example, we may have written a visitor which performs some 'verb' like this:

     // A visitor that gets the area of a shape.  TShape::TVisitor is the abstract visitor for TShape.
     class TGetAreaVisitor : public TShape::TVisitor {
       public:

       TGetAreaVisitor(double &result) : Result(result) {}

       virtual void operator()(const TCircle *) { Result = ...; }
       virtual void operator()(const TSquare *) { Result = ...; }
       virtual void operator()(const TTriangle *) { Result = ...; }

       private:

       double &Result;

     };  // TGetAreaVisitor

     and used it like:

     TShape *shape = new TCircle(...);
     double result;
     shape->Accept(TGetAreaVisitor(result));
     // use result.

   What we would rather say is:

     auto result = Verb(shape);
     // use result.

   and have Verb figure out the dynamic type of shape is, and dispatch to the correct handler.

   The usage pattern looks like this:

     // .h:

     // Handlers for GetArea
     struct TGetArea {
       double operator()(const TCircle *) const;
       double operator()(const TSquare *) const;
       double operator()(const TTriangle *) const;
     };  // TGetArea

     // This function takes a shape and dispatches to one of the handlers in TGetArea
     double GetArea(const TShape *that);

     // We're given a final shape, early bind for the win.
     template <typename TMember>
     double GetArea(const TMember *member) {
       TGetArea()(member);
     }

     // .cc:

     // Handler definitions.
     double TGetArea::operator()(const TCircle *) const { ... }
     double TGetArea::operator()(const TSquare *) const { ... }
     double TGetArea::operator()(const TTriangle *) const { ... }

     double GetArea(const TShape *that) {
       using applier_t = Visitor::Single::TApplier<TGetArea, TShape::TVisitor>;
       Visitor::Single::Accept<applier_t>(that);
     }

   Now we can use GetArea() like this:

     TShape *shape = new TTriangle(...);
     auto result = GetArea(shape);  // Dispatches to TGetArea::operator()(const TTriangle *).

   It also does early-binding in situations where it can.

     TSquare *square = new TSquare(...);
     auto result = GetArea(square);  // Dispatches directly.

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

#include <cassert>
#include <c14/apply.h>
#include <c14/identity.h>
#include <c14/lang.h>
#include <c14/type_traits.h>
#include <c14/utility.h>
#include <tuple>

#include <mpl/get_at.h>
#include <mpl/type_list.h>
#include <mpl/type_set.h>

#include <base/no_construction.h>

namespace Visitor {

  /* Type modifier which figures out how to capture arguments which will be cached then passed later on.
     If it's a reference, we wrap the underlying type in a reference_wrapper<>, otherwise we decay the type, which means the value
     will either move or copy in. */
  template <typename T>
  using TCapture =
      c14::conditional_t<std::is_lvalue_reference<T>::value, std::reference_wrapper<c14::remove_reference_t<T>>, c14::decay_t<T>>;

  /* Forward declaration for TPartialForwarder. */
  template <typename TVerb, typename Signature>
  class TPartialForwarder;

  /* General case for PartialForwarder. */
  template <typename TVerb, typename TResult_, typename... TBoundArgs>
  class TPartialForwarder<TVerb, TResult_ (TBoundArgs...)> {
    public:

    /* Bring our result type into scope for look-up. */
    using TResult = TResult_;

    /* Cache the result out parameter and the arguments being bound. */
    template <typename... TForwardArgs>
    TPartialForwarder(TResult &result, TForwardArgs &&... args)
        : Result(result), BoundArgs(std::forward<TForwardArgs>(args)...) {}

    /* We take arbitrary number of arguments and we call the TVerb::operator() along with the bound arguments that we cached. */
    template <typename... TUnboundArgs>
    void operator()(TUnboundArgs &&... args) const {
      Result = c14::apply(TVerb(), std::tuple_cat(std::forward_as_tuple(std::forward<TUnboundArgs>(args)...), BoundArgs));
    }

    private:

    /* Result out parameter. */
    TResult &Result;

    /* Modify the bound args with TCapture and store a tuple of them. */
    const std::tuple<TCapture<TBoundArgs>...> BoundArgs;

  };  // TPartialForwarder<TVerb, TResult (TBoundArgs...)>

  template <typename TVerb, typename TResult_, typename... TBoundArgs>
  class TPartialForwarder<TVerb, TResult_ &(TBoundArgs...)> {
    public:

    /* Bring our result type into scope for look-up. */
    using TResult = TResult_ &;

    /* Cache the result out parameter and the arguments being bound. */
    template <typename... TForwardArgs>
    TPartialForwarder(TResult_  *&result, TForwardArgs &&... args)
        : Result(result), BoundArgs(std::forward<TForwardArgs>(args)...) {}

    /* We take arbitrary number of arguments and we call the TVerb::operator() along with the bound arguments that we cached. */
    template <typename... TUnboundArgs>
    void operator()(TUnboundArgs &&... args) const {
      Result = &c14::apply(TVerb(), std::tuple_cat(std::forward_as_tuple(std::forward<TUnboundArgs>(args)...), BoundArgs));
    }

    private:

    /* Result out parameter. */
    TResult_ *&Result;

    /* Modify the bound args with TCapture and store a tuple of them. */
    const std::tuple<TCapture<TBoundArgs>...> BoundArgs;

  };  // TPartialForwarder<TVerb, TResult &(TBoundArgs...)>

  /* Special case for void. It's pretty much the same as above except that we don't have a result out parameter. */
  template <typename TVerb, typename... TBoundArgs>
  class TPartialForwarder<TVerb, void (TBoundArgs...)> {
    public:

    /* Define for look-up. */
    using TResult = void;

    /* See above. */
    template <typename... TForwardArgs>
    TPartialForwarder(TForwardArgs &&... args)
        : BoundArgs(std::forward<TForwardArgs>(args)...) {}

    /* See above. */
    template <typename... TUnboundArgs>
    void operator()(TUnboundArgs &&... args) const {
      c14::apply(TVerb(), std::tuple_cat(std::forward_as_tuple(std::forward<TUnboundArgs>(args)...), BoundArgs));
    }

    private:

    /* See above. */
    const std::tuple<TCapture<TBoundArgs>...> BoundArgs;

  };  // TPartialForwarder<TVerb, void (TBoundArgs...)>

  /* This has a bit of magic... it uses the fact that functions deduce template parameters for us to figure out the return type
     and the extra arguments of overloaded functions.

     For example, if we have a Verb like:

       struct TSomeVerb {
         bool operator()(const U0 &, B1, B2, B3) {}
         bool operator()(const U1 &, B1, B2, B3) {}
         bool operator()(const U2 &, B1, B2, B3) {}
       };  // TSomeVerb

       Where Ux are unbound arguments and Bx are bound arguments.

     We want to deduce the return type and the bound arguments of this Verb. Well, we need to pick one of the functions,
     We'll pick U0 as our 'representative' version then we can say,

       TSignature<TVerb, const U0 &>  ==  bool (B1, B2, B3)

     Now specifying the type for TPartialForwarder can be done via

       TPartialForwarder<TVerb, TSignature<TVerb, const U0 &>>

     It can also handle another kind of Verb. Suppose the handlers are really similar for all the cases like... printing.
     So we might end up writing something like:

       struct TPrint {
         void operator()(const U0 &that, B1, B2, B3) { std::cout << that << std::endl; }
         void operator()(const U1 &that, B1, B2, B3) { std::cout << that << std::endl; }
         void operator()(const U2 &that, B1, B2, B3) { std::cout << that << std::endl; }
       };  // TPrint

     NOTE: if there is a template involved, it is necessary to explicitly state the signature type, like so,

       struct TSomeVerb {

         using Signature = void (B1, B2, B3);

         template <typename T>
         void operator()(T &&that, B1, B2, B3) {
           // do something with 'std::forward<T>(that)'
         }

       };  // TSomeVerb
  */
  template <typename TVerb, typename... TDecoratedMembers>
  class Signature {
    NO_CONSTRUCTION(Signature);
    private:

    /* The magic that deduces the return type and extra arguments based on the unbound args that we know are handled.
       NOTE: The bodies aren't defined because it should actually ever be called. */

    /* const-member functions. */
    template <typename TResult, typename... TArgs>
    static c14::identity<TResult (TArgs...)> Get(TResult (TVerb::*)(TDecoratedMembers..., TArgs...) const);

    /* These 2 Call() functions use SFINAE to pick the situation that fits our needs. */

    /* The case where we have all of the handlers written out. */
    template <typename T = TVerb>
    static decltype(Get(&T::operator())) Impl();

    /* Template involved, look up the Signature type. */
    template <typename T = TVerb>
    static c14::identity<typename T::Signature> Impl();

    public:

    /* This is our TSignature type which tells us the return type and extra arguments. */
    using type = typename decltype(Impl())::type;

  };  // Signature

  /* Convenience type alias for Signature<>. */
  template <typename TVerb, typename... TDecoratedMembers>
  using TSignature = typename Signature<TVerb, TDecoratedMembers...>::type;

  namespace Single {

    /* Implementation of an applier.  It inherits from TSomeVisitor which is an abstract single visitor and defines the operator()
       overrides which calls the TPartialForwarder that we cached. */
    struct ApplierImpl {
      NO_CONSTRUCTION(ApplierImpl);
      private:

      /* Forward declaration. */
      template <typename TFinalApplier, typename TSomeVisitor, typename TMembers>
      class TApplierRecur;

      /* Base case. Inherit from TSomeVisitor. */
      template <typename TFinalApplier, typename TSomeVisitor>
      class TApplierRecur<TFinalApplier, TSomeVisitor, Mpl::TTypeList<>> : public TSomeVisitor {};

      /* Recursive case. */
      template <typename TFinalApplier, typename TSomeVisitor, typename TMember, typename... TMoreMembers>
      class TApplierRecur<TFinalApplier, TSomeVisitor, Mpl::TTypeList<TMember, TMoreMembers...>>
          : public TApplierRecur<TFinalApplier, TSomeVisitor, Mpl::TTypeList<TMoreMembers...>> {
        protected:

        /* The decorated type of our member. */
        using TDecoratedMember = typename TSomeVisitor::template Decorator<TMember>::type;

        private:  // NOTE: private so that we can only call this function through the TSomeVisitor that we inherit from.

        virtual void operator()(TDecoratedMember that) const override {
          static_cast<const TFinalApplier *>(this)->Forwarder(std::forward<TDecoratedMember>(that));
        }

      };  // TApplierRecur<TFinalApplier, TSomeVisitor, Mpl::TTypeList<TMember, TMoreMembers...>>

      public:

      /* Final applier. */
      template <typename TVerb, typename TSomeVisitor, typename TMembers>
      class TApplier
          : public TApplierRecur<TApplier<TVerb, TSomeVisitor, TMembers>, TSomeVisitor, Mpl::TGetList<TMembers>> {
        private:

        using TSuper = TApplierRecur<TApplier<TVerb, TSomeVisitor, TMembers>, TSomeVisitor, Mpl::TGetList<TMembers>>;

        /* Our partial forwarder type. See above to see why the type looks like this. */
        using TPartialForwarder = Visitor::TPartialForwarder<TVerb, TSignature<TVerb, typename TSuper::TDecoratedMember>>;

        public:

        /* Bring TResult into scope for look-up. */
        using TResult = typename TPartialForwarder::TResult;

        /* Forward all the things to partial forwarder. */
        template <typename... TForwardArgs>
        TApplier(TForwardArgs &&... args)
            : Forwarder(std::forward<TForwardArgs>(args)...) {}

        private:

        /* Our forwarder. */
        const TPartialForwarder Forwarder;

        /* Friend our recursive implementers. */
        template <typename TVerb_, typename TSomeVisitor_, typename TMembers_>
        friend class TApplierRecur;

      };  // TApplier

    };  // ApplierImpl

    /* Convenience type alias to bring ApplierImpl::TApplier into Visitor namespace. */
    template <typename TVerb, typename TSomeVisitor, typename TMembers = typename TSomeVisitor::TMembers>
    using TApplier = ApplierImpl::TApplier<TVerb, TSomeVisitor, TMembers>;

  }  // Single

  namespace Double {

    /* Implementation of our double applier.  This is simply an instance of a double visitor, we use the TLhsVisitor and
       TRhsVisitor templates which generates whatever lhs visitor and rhs visitor we need.  */
    struct ApplierImpl {
      NO_CONSTRUCTION(ApplierImpl);
      private:

      /* Forward declaration. */
      template <typename TFinalApplier,
                template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMembers,
                typename TRhsMembers>
      class TApplierRecur;

      /* Base case. */
      template <typename TFinalApplier,
                template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMember,
                typename TRhsMember>
      class TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TRhsMember>> {
        protected:

        /* The decorated type of our members. */
        using TDecoratedLhs = typename LhsDecorator<TLhsMember>::type;
        using TDecoratedRhs = typename RhsDecorator<TRhsMember>::type;

        /* Overload for the pair of decorated members. */
        void operator()(TDecoratedLhs lhs, TDecoratedRhs rhs) const {
          static_cast<const TFinalApplier *>(this)->Forwarder(std::forward<TDecoratedLhs>(lhs), std::forward<TDecoratedRhs>(rhs));
        }

      };  // TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, Mpl::TTypeList<TRhsMember>>

      /* Single recursive case. */
      template <typename TFinalApplier,
                template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMember,
                typename TRhsMember,
                typename... TMoreRhsMembers>
      class TApplierRecur<TFinalApplier,
                          LhsDecorator,
                          RhsDecorator,
                          Mpl::TTypeList<TLhsMember>,
                          Mpl::TTypeList<TRhsMember, TMoreRhsMembers...>>
          : public TApplierRecur<TFinalApplier,
                                 LhsDecorator,
                                 RhsDecorator,
                                 Mpl::TTypeList<TLhsMember>,
                                 Mpl::TTypeList<TMoreRhsMembers...>> {
        protected:

        using TApplierRecur<TFinalApplier,
                            LhsDecorator,
                            RhsDecorator,
                            Mpl::TTypeList<TLhsMember>,
                            Mpl::TTypeList<TMoreRhsMembers...>>::operator();

        /* The decorated type of our members. */
        using TDecoratedLhs = typename LhsDecorator<TLhsMember>::type;
        using TDecoratedRhs = typename RhsDecorator<TRhsMember>::type;

        /* Overload for the pair of decorated members. */
        void operator()(TDecoratedLhs lhs, TDecoratedRhs rhs) const {
          static_cast<const TFinalApplier *>(this)->Forwarder(std::forward<TDecoratedLhs>(lhs), std::forward<TDecoratedRhs>(rhs));
        }

      };  // TApplierRecur<...>

      /* Double recursive case. */
      template <typename TFinalApplier,
                template <typename> class LhsDecorator,
                template <typename> class RhsDecorator,
                typename TLhsMember,
                typename... TMoreLhsMembers,
                typename TRhsMembers>
      class TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember, TMoreLhsMembers...>, TRhsMembers>
          : public TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, TRhsMembers>,
            public TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TMoreLhsMembers...>, TRhsMembers> {
        protected:

        using TLhsSuper = TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember>, TRhsMembers>;
        using TRhsSuper =
            TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TMoreLhsMembers...>, TRhsMembers>;

        /* Bring operator()s down the inheritance chain. */
        using TLhsSuper::operator();
        using TRhsSuper::operator();

      };  // TApplierRecur<TFinalApplier, LhsDecorator, RhsDecorator, Mpl::TTypeList<TLhsMember, TMoreLhsMembers...>, TRhsMembers>

      /* Final applier. */
      template <typename TVerb, typename TSomeVisitor, typename TLhsMembers, typename TRhsMembers>
      class TApplier : public TApplierRecur<TApplier<TVerb, TSomeVisitor, TLhsMembers, TRhsMembers>,
                                            TSomeVisitor::TLhsVisitor::template Decorator,
                                            TSomeVisitor::TRhsVisitor::template Decorator,
                                            Mpl::TGetList<TLhsMembers>,
                                            Mpl::TGetList<TRhsMembers>> {
        private:

        using TSuper = TApplierRecur<TApplier<TVerb, TSomeVisitor, TLhsMembers, TRhsMembers>,
                                     TSomeVisitor::TLhsVisitor::template Decorator,
                                     TSomeVisitor::TRhsVisitor::template Decorator,
                                     Mpl::TGetList<TLhsMembers>,
                                     Mpl::TGetList<TRhsMembers>>;

        /* Our partial forwarder type. See above for examples. */
        using TPartialForwarder = Visitor::TPartialForwarder<
            TVerb,
            TSignature<TVerb, typename TSuper::TLhsSuper::TDecoratedLhs, typename TSuper::TLhsSuper::TDecoratedRhs>>;

        public:

        /* We need these in scope because Accept() will look for it. */
        using TLhsVisitor = typename TSomeVisitor::TLhsVisitor;
        using TRhsVisitor = typename TSomeVisitor::TRhsVisitor;

        /* Bring result into scope for look-up. */
        using TResult = typename TPartialForwarder::TResult;

        /* Forward all the things to forwarder. */
        template <typename... TForwardArgs>
        TApplier(TForwardArgs &&... args)
            : Forwarder(std::forward<TForwardArgs>(args)...) {}

        /* Bringing in all of the operator()s defined in the parents. */
        using TSuper::operator();

        private:

        /* Our partial forwarder. */
        const TPartialForwarder Forwarder;

        /* Friend our recursive implementers. */
        template <typename TFinalApplier,
                  template <typename> class LhsDecorator,
                  template <typename> class RhsDecorator,
                  typename TLhsMembers_,
                  typename TRhsMembers_>
        friend class TApplierRecur;

      };  // TApplier

    };  // ApplierImpl

    /* Convenience type alias to bring ApplierImpl::TApplier into Visitor::Double namespace. */
    template <typename TVerb,
              typename TSomeVisitor,
              typename TLhsMembers = typename TSomeVisitor::TLhsVisitor::TMembers,
              typename TRhsMembers = typename TSomeVisitor::TRhsVisitor::TMembers>
    using TApplier = ApplierImpl::TApplier<TVerb, TSomeVisitor, TLhsMembers, TRhsMembers>;

  }  // Double

}  // Visitor
