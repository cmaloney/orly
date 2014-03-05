/* <visitor/variant.h>

   A variable which can change its type at runtime, but only among a finite set
   of possible types.  The set is called the variant's family.  Each member of a
   variant's family must be a value type.

   To define a variant family, you should make a header which includes all the
   types which are in the family.  (Note that a type may be in more than one
   family.)  You must provide a forward-declaration of a type which will stand
   for the family as a whole, then use this type to explicitly specialize the
   visitor classes in variant.

   For example:

     // Family identifier.
     class NumberFamily;

     // TVariant alias.
     using TNumber = Visitor::TVariant<NumberFamily>;

   Define the members that make up the family like this,

     namespace Visitor {

       template <>
       class TVariant<NumberFamily>::Members {

         using All = Mpl::TTypeSet<int, double>;  // Required!

         // Define other subgroups.

       };  // TVariant<NumberFamily>::Members

     }  // Visitor

   We can now use instances of TNumber to hold either ints or doubles. Notice
   that we don't actually define the visitors ourselves. TVisitor and
   TMutatingVisitor are defined within the TVariant class based on the 'All'
   type alias inside the class Members. Each override in the regular
   (non-mutating) visitor takes its argument by val or constant reference,
   whichever one is more efficient for passing, while each override in the
   mutating visitor takes its argument by non-const reference.

   To operate on a variant, you must apply a visitor.  The visitor embodies the
   algorithm you wish to apply.  To create a visitor class, inherit from one of
   the visitor base classes and provide a handler for each type in the family.
   If you wish to mutate the variant in-place, inherit from TMutatingVisitor;
   otherwise, use TVisitor.

   A visitor object should not itself be modified by visiting--that's why the
   operator() overloads are constant--but visiting may have side-effects, such
   as writing to a stream or modifying a variable to which the visitor holds a
   non-constant reference.  It's ok to reuse a visitor, and doing so helps to
   amortize the cost of constructing it.

   Variants are themselves value types and support default-, move-, and copy-
   construction, move- and copy-assignment, and swapping.  A variant uses a
   heap allocation to keep track of its internal state, but multiple variants
   will share a single such allocation, even across threads and families, so
   the overhead is mitigated and moving and copying are very fast.  Variants
   also implement copy-on-write (cow) semantics which trigger automatically
   when a mutating visitor is applied to a variant with a shared state.

   The default-constructed variant has no state and represents no type in its
   family.  When cast to bool, a stateless variant such as this will return
   false.  It is an error to apply a visitor to a stateless variant.

   A variant constructed by move- or copy-construction from an instance of
   a family member will have a state.  If cast to bool, it will return true
   and it can accept a visitor.

   All operations on variants are no-throw except for three cases:
     * accepting a non-mutating visitor will throw iff. the visitor throws;
     * accepting a mutating visitor will throw if the visitor throws or
       if accepting it it triggers cow semantics and memory is low; and
     * constructing a variant by copying from a family member will throw
       iff. the family member's copy-constructor throws.

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

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <c14/type_traits.h>

#include <base/demangle.h>
#include <base/no_copy_semantics.h>
#include <mpl/contains.h>
#include <mpl/enable_if.h>
#include <mpl/get_difference.h>
#include <visitor/shared.h>
#include <visitor/visitor.h>

namespace Visitor {

  //TODO: This belongs somewhere in <mpl/*>
  template <bool B, typename T>
  using requires = typename std::enable_if<B, T>::type;

  /* Defines a class that can change its type at runtime, but only among a
     finite set of possible types. */
  template <typename Family>
  class TVariant final {
    public:

    /* Structure which defines the type alias 'All' which lists all of the types
       in the family.  Additional subgroups of the family members may be defined
       here for convenience. */
    struct Members;

    /* Our non-mutating visitor. Pipes the family members through by Pass<T>. */
    class TVisitor
        : public Single::TVisitor<Pass, typename Members::All> {};

    /* Our mutating visitor. Pipes the family members through by Ref<T>. */
    class TMutatingVisitor
        : public Single::TVisitor<Ref, typename Members::All> {};

    /* Our non-mutating double visitor. You can specify the TRhsVisitor if you
       have a different variant on the rhs, but it defaults to our visitor. */
    template <typename TRhsVisitor = TVisitor>
    using TDoubleVisitor = Double::TVisitor<TVisitor, TRhsVisitor>;

    /* Our mutating double visitor. */
    template <typename TRhsVisitor = TMutatingVisitor>
    using TMutatingDoubleVisitor =
        Double::TVisitor<TMutatingVisitor, TRhsVisitor>;

    /* Our non-mutating applier. Provide TVerb function object, and use the
       Single::Accept<> template function to visit the variant. */
    template <typename TVerb>
    class TApplier : public Single::TApplier<TVerb, TVisitor> {
      public:

      using TSuper = Single::TApplier<TVerb, TVisitor>;

      template <typename... TArgs>
      TApplier(TArgs &&... args)
          : TSuper(std::forward<TArgs>(args)...) {}

    };  // TApplier<TVerb>

    /* Our mutating applier. Provide TVerb function object, and use the
       Single::Accept<> template function to visit the variant. */
    template <typename TVerb>
    class TMutatingApplier : public Single::TApplier<TVerb, TMutatingVisitor> {
      public:

      using TSuper = Single::TApplier<TVerb, TMutatingVisitor>;

      template <typename... TArgs>
      TMutatingApplier(TArgs &&... args)
          : TSuper(std::forward<TArgs>(args)...) {}

    };  // TMutatingApplier<TVerb>

    /* Our non-mutating double visitor. You can specify the TRhsVisitor if you
       have a different variant on the rhs, but it default to our visitor. */
    template <typename TVerb, typename TRhsVisitor = TVisitor>
    using TDoubleApplier = Double::TApplier<TVerb, TDoubleVisitor<TRhsVisitor>>;

    /* Our mutating double visitor. */
    template <typename TVerb, typename TRhsVisitor = TMutatingVisitor>
    using TMutatingDoubleApplier =
        Double::TApplier<TVerb, TMutatingDoubleVisitor<TRhsVisitor>>;

    /* Our integral constant to test if TVal is a member of this family. */
    template <typename TElem>
    class IsMember
        : public Mpl::Contains<typename Members::All, c14::decay_t<TElem>> {};

    /* Construct a new variant off of a non-managed element. */
    template <typename TElem, Mpl::EnableIf<IsMember<TElem>>...>
    static TVariant New(TElem &&elem) {
      using elem_t = c14::decay_t<TElem>;
      return TVariant(TAcceptor<elem_t>::Get(),
                      MakeShared<elem_t>(std::forward<TElem>(elem)));
    }

/* Commented out since we'll defer support for sharing variants off of managed
   elements. Because we pass our element by value if it's small enough, within a
   function we could have a copy of a managed element which of course is not
   managed. Calling Share() on such elements will cause problems. Once we add
   the support for storing small elements in-place, both New() and Share() will
   store the element in-place if the element is small enough. */
#if 0
    /* Construct a new variant off of a managed element. Note that the given
       element __must__ be managed. If the element is not managed, it will hit
       an assertion failure under debug mode and most likely seg-faults in
       release mode so be __sure__ to use it carefully! */
    template <typename TElem, Mpl::EnableIf<IsMember<TElem>>...>
    static TVariant Share(TElem &elem) {
      using elem_t = c14::decay_t<TElem>;
      return TVariant(TAcceptor<elem_t>::Get(),
                      Visitor::Share(const_cast<elem_t &>(elem)));
    }
#endif

    /* Default-construct as stateless. */
    TVariant() noexcept : Acceptor(nullptr) {}

    /* Steal the state from the donor, leaving it stateless. */
    TVariant(TVariant &&that) noexcept : TVariant() { Swap(that); }

    /* Steal the state from a different variant family. If the 'that' is
       stateless, we just leave this in the default-constructed state.
       Otherwise, we visit 'that' with the TGetAcceptor verb, TGetAcceptor will
       throw a std::bad_cast for any type that is not member of this family.
       For the types that are in this family, we set the our acceptor to save
       the type that we currently are. Once we have set our acceptor we know
       that we have a valid conversion, we conclude by moving the data. */
    template <typename ThatFamily>
    TVariant(TVariant<ThatFamily> &&that) : TVariant() {
      assert(&that);
      if (!that) {
        return;
      }  // if
      using get_acceptor_t =
          typename TVariant<ThatFamily>::template TApplier<TGetAcceptor>;
      Acceptor = Single::Accept<get_acceptor_t>(that);
      that.Acceptor = nullptr;
      Data = std::move(that.Data);
    }

    /* Share the state of the other variant. */
    TVariant(const TVariant &that) noexcept
        : Acceptor(that.Acceptor), Data(that.Data) {}

    /* Same as the across-family move-constructor except we copy the data at the
       end rather than moving it. */
    template <typename ThatFamily>
    TVariant(const TVariant<ThatFamily> &that) : TVariant() {
      assert(&that);
      if (!that) {
        return;
      }  // if
      using get_acceptor_t =
          typename TVariant<ThatFamily>::template TApplier<TGetAcceptor>;
      Acceptor = Single::Accept<get_acceptor_t>(that);
      Data = that.Data;
    }

    /* Destroy our state. */
    ~TVariant() noexcept {
      assert(this);
      Acceptor = nullptr;
    }

    /* Returns true iff. this variant is non-empty. */
    explicit operator bool() const noexcept {
      assert(this);
      assert((Acceptor && Data) || (!Acceptor && !Data));
      return Acceptor;
    }

    /* Steal the state from the donor, leaving it stateless.
       If we already have a state, abandon it. */
    TVariant &operator=(TVariant &&that) noexcept {
      assert(this);
      TVariant temp(std::move(that));
      return Swap(temp);
    }

    /* Same as above but across-family. */
    template <typename ThatFamily>
    TVariant &operator=(TVariant<ThatFamily> &&that) {
      assert(this);
      TVariant temp(std::move(that));
      return Swap(temp);
    }

    /* Share the state with the other variant.
       If we already have a state, abandon it. */
    TVariant &operator=(const TVariant &that) noexcept {
      assert(this);
      TVariant temp(that);
      return Swap(temp);
    }

    /* Same as above but across-family. */
    template <typename ThatFamily>
    TVariant &operator=(const TVariant<ThatFamily> &that) {
      assert(this);
      TVariant temp(that);
      return Swap(temp);
    }

    /* Accept a regular (non-mutating) visitor. */
    void Accept(const TVisitor &visitor) const {
      assert(this);
      assert(*this);
      assert(&visitor);
      (*Acceptor)(visitor, Data.Get());
    }

    /* Accept a mutating visitor. */
    void Accept(const TMutatingVisitor &visitor) {
      assert(this);
      assert(*this);
      assert(&visitor);
      if (!Data.IsUnique()) {
        Data = Single::Accept<TApplier<TMakeShared>>(*this);
      }  // if
      (*Acceptor)(visitor, Data.Get());
    }

    /* Bring us back to the default constructed state. */
    TVariant &Reset() noexcept {
      assert(this);
      TVariant temp;
      return Swap(temp);
    }

    /* Swap two variants. */
    TVariant &Swap(TVariant &that) noexcept {
      assert(this);
      assert(&that);
      if (this == &that) {
        return *this;
      }  // if
      std::swap(Acceptor, that.Acceptor);
      std::swap(Data, that.Data);
      return *this;
    }

    private:

    /* The type of our type-erased data. */
    using TData = TShared<void>;

    /* Our type-erased acceptor. We keep the actual type of the data saved in
       the typed acceptor. See below for the definition of typed acceptor. */
    class TAnyAcceptor {
      NO_COPY_SEMANTICS(TAnyAcceptor);
      public:

      /* See below. */
      virtual void operator()(const TVisitor &visitor,
                              const void *data) const = 0;

      /* See below. */
      virtual void operator()(const TMutatingVisitor &visitor,
                              void *data) const = 0;

      protected:

      /* Do-nothing. */
      TAnyAcceptor() noexcept {}

    };  // TAnyAcceptor

    /* Our typed acceptor which casts our data and passes it to the given
       visitor. We use the singleton pattern here, the variant class keeps a
       pointer to one of these with a pointer to a TAnyAcceptor. */
    template <typename TElem>
    class TAcceptor final : public TAnyAcceptor {
      NO_COPY_SEMANTICS(TAcceptor);
      public:

      /* Accessor to our singleton instance. */
      static const TAcceptor *Get() {
        static TAcceptor acceptor;
        return &acceptor;
      }

      private:

      /* Accept a regular (non-mutating) visitor and pass the data through with
         its correct type. */
      virtual void operator()(const TVisitor &visitor,
                              const void *data) const override {
        assert(this);
        assert(&visitor);
        assert(data);
        visitor(*static_cast<const TElem *>(data));
      }

      /* Accept a mutating visitor and pass the data through with its correct
         type. If the data is unique (we're the only one referring to it), we
         don't perform a copy, however if it is not unique (other instances of
         variants are referring to it) then we make a copy and visit that value
         instead. Fullfilling the COW (Copy-on-Write) feature of TVariant. */
      virtual void operator()(const TMutatingVisitor &visitor,
                              void *data) const override {
        assert(this);
        assert(&visitor);
        assert(data);
        visitor(*static_cast<TElem *>(data));
      }

      /* Do-nothing. */
      TAcceptor() noexcept {}

    };  // TAcceptor<TElem>

    /* If we're not the only one managing our data, and we want to visit it
       using a mutating visitor, we need to perform a copy before we visit it.
       (for copy-on-write semantics). */
    struct TMakeShared {

      /* Our function signature. */
      using Signature = TData ();

      /* Simply call the MakeShared function and return our copied data. */
      template <typename TElem>
      TData operator()(TElem &&that) const {
        return MakeShared<c14::decay_t<TElem>>(std::forward<TElem>(that));
      }

    };  // TMakeShared

    /* Our TGetAcceptor verb. This is used in the across-family copy- and
       move- constructors to visit the other family's variant and get the
       correct acceptor back iff the type of the value currently in the other
       variant is a member of this family. */
    class TGetAcceptor {
      public:

      /* Our function signature. */
      using Signature = const TAnyAcceptor *();

      /* The other variant's value is not supported by this family.
         Throw a std::bad_cast exception. */
      template <typename TElem>
      requires<!IsMember<TElem>::value,
      const TAnyAcceptor *> operator()(TElem &&) const {
        std::cerr
            << "Error attempting to copy- or move- construct variants across "
            << "families. The type of the value held by the other variant "
               "was "
            << Base::TDemangle(typeid(TElem)).Get()
            << ", which is not a member of this family." << std::endl;
        throw std::bad_cast();
      }

      /* The other variant's value is not supported by this family.
         Return the corresponding type-erased acceptor. */
      template <typename TElem>
      requires<IsMember<TElem>::value,
      const TAnyAcceptor *> operator()(TElem &&) const {
        return TAcceptor<c14::decay_t<TElem>>::Get();
      }

    };  // TGetAcceptor

    /* Cache the given acceptor and data. */
    TVariant(const TAnyAcceptor *acceptor, TData data)
        : Acceptor(acceptor), Data(std::move(data)) {}

    /* Our type-erased acceptor. */
    const TAnyAcceptor *Acceptor;

    /* Our type-erased data. */
    TData Data;

    /* Be friends with all definitions of TVariant<ThatFamily>. */
    template <typename ThatFamily>
    friend class TVariant;

  };  // TVariant

}  // Visitor
