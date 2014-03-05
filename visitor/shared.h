/* <visitor/shared.h>

   Defines a custom version of the shared pointer which __guarantees__ that
   the control block and managed element get allocated together.

   Consider,

     * auto x = std::make_shared<int>(42);
     * auto y = std::shared_ptr<int>(new int(42));

   We clearly prefer the first version since it guarantees a single heap
   allocation, however std::shared_ptr<> supports the second version because
   std::make_shared<> was not introduced later on until variadic templates.

   However, suppose that we have access to a managed element. We know that
   there's a control block somewhere with a ref-count for it, but there's no
   way to provide that information to std::shared_ptr<>.

   For example, suppose we pass an already-managed element to a function like,

     // In this version, we computed a new value which needs to be managed, so
        fine, we'll give it to std::make_shared<> to get it managed.
     std::shared_ptr<std::string> ToString(const int &that) {
       assert(&that);
       return std::make_shared<std::string>(std::to_string(that));
     }

     // For this version, we don't have a new value computed, and we know that
        the string is already managed. We'd rather not make another copy of it
        and worse-yet, do an unnecessary heap-allocation.
     std::shared_ptr<std::string> ToString(const std::string &that) {
       assert(&that);
       return std::make_shared<std::string>(that);
     }

   With TShared<>, we can do:

     // ToString(const int &that) omitted.

     TShared<std::string> ToString(const std::string &that) {
       assert(&that);
       return TShared<std::string>::Share(&that);
     }

   This also solves the std::enable_shared_from_this<> craziness.
   Using std::shared_ptr<> we must write:

     class TObj : public std::enable_shared_from_this<TObj> {
       public:

       std::shared_ptr<TObj> GetThis() {
         assert(this);
         return shared_from_this();
       }

     };  // TObj

   With TShared<>, we can write:

     class TObj {
       public:

       TShared<TObj> GetThis() {
         assert(this);
         return TShared<TObj>::Share(this);
       }

     };  // TObj

   Note that there are no constructors available for TShared<> aside from the
   copy and move constructors.  You'll want to use the following to create
   instances of TShared<>.

     * TShared<T>::New(TArgs &&...);
     * TShared<T>::Share(T *);

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

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <typeinfo>
#include <utility>

#include <base/no_construction.h>
#include <base/no_copy_semantics.h>
#include <mpl/enable_if.h>
#include <mpl/not.h>

#include <iostream>

namespace Visitor {

  /* Our control block that holds reference count information about TShared<>,
     TWeak<> instances and manages the life-time of the managed element.

     It holds,

       * The destroy function we will call when the number of TShared<>
         instances that point to the managed element reeaches 0.
       * Number of TShared<> instances that point to the managed element.
       * Number of TWeak<> instances that point to the managed element.

       */
  class TControlBlock final {
    NO_COPY_SEMANTICS(TControlBlock);
    private:

    /* The deleter for std::unique_ptr<> which manages malloc'ed memory. */
    struct Free {

      /* Pipe through to std::free(). */
      void operator()(void *buffer) const { std::free(buffer); }

    };  // Free

    /* Convenience type alias for our buffer type. */
    using TBuffer = std::unique_ptr<void, Free>;

    /* Convenience type alias for the destroy function which is simply a
       type-erased destructor of the managed element. */
    using TDestroy = void (*)(void *elem);

    public:

    /* Allocate space for TControlBlock, followed immediately by the TElem.
       We access the TElem by accessing the address immediately after the
       TControlBlock. Construct the TControlBlock and TElem in-place,
       providing TControlBlock with the type-erased ~TElem(). This is
       neceesary to support TShared<void> since we must still call the
       destructor of the original type before being constructed into
       TShared<void>. */
    static TBuffer New(std::size_t size, TDestroy destroy) {
      TBuffer buffer(std::malloc(sizeof(TControlBlock) + size));
      if (!buffer) {
        throw std::bad_alloc();
      }  // if
      new (buffer.get()) TControlBlock(destroy);
      return buffer;
    }

    /* Helper function to get the control block given a pointer to an element.
       The pointer must already have a valid TControlBlock preceding it. */
    static TControlBlock *GetControlBlock(const void *elem) noexcept {
      auto *control_block =
          std::prev(static_cast<TControlBlock *>(const_cast<void *>(elem)));
      assert(control_block->IsValid());
      return control_block;
    }

    /* Increment the number of TShared<> instances that are currently pointing
       at the managed element. */
    void IncrSharedCount() noexcept {
      assert(this);
      ++SharedCount;
    }

    /* Decrement the number of TShared<> instances that are currently pointing
       at the managed element. If no TShared<> instances are pointing at the
       managed element, destroy the managed element. If there are TWeak<>
       instances pointing at the managed element, we keep this alive so that
       they can inspect the SharedCount. If no TWeak<> instances are pointing
       at the managed element either, No one wants us... we die. */
    void DecrSharedCount() noexcept {
      assert(this);
      assert(SharedCount > 0);
      if (--SharedCount == 0) {
        Destroy(GetElem());
        if (WeakCount == 0) {
          Delete();
        }  // if
      }  // if
    }

    /* Increment the number of TWeak<> instances that are currently pointing
       at the managed element. */
    void IncrWeakCount() noexcept {
      assert(this);
      ++WeakCount;
    }

    /* Decrement the number of TWeak<> instances that are currently pointing
       at the managed element. If no TShared<> instance are pointing at the
       managed element and neither are any TWeak<> instances, no one wants us,
       we die. */
    void DecrWeakCount() noexcept {
      assert(this);
      assert(WeakCount > 0);
      if (--WeakCount == 0 && SharedCount == 0) {
        Delete();
      }  // if
    }

    /* Return the number of TShared<> instances that are currently pointing at
       the managed element. */
    std::size_t GetCount() const noexcept {
      assert(this);
      return SharedCount;
    }

    /* Validate the control_block and return the memory address immediately
       after the control block. */
    void *GetElem() const noexcept {
      assert(this);
      assert(IsValid());
      return static_cast<void *>(const_cast<TControlBlock *>(std::next(this)));
    }

    private:

    /* Do-little. */
    TControlBlock(TDestroy destroy) noexcept
        : Destroy(destroy),
          SharedCount(0),
          WeakCount(0),
#ifdef NDEBUG
#else
          Type(&typeid(TControlBlock)),
          This(this)
#endif
          {}

    /* Invoke ~TControlBlock() and delete the memory allocated by TBuffer.
       Use whatever deleter is set for TBuffer so that we stay consistent. */
    void Delete() noexcept {
      assert(this);
      this->~TControlBlock();
      TBuffer::deleter_type()(this);
    }

    /* Used in debug mode to catch references to control blocks that are
       invalid. In release mode it just returns true. */
    bool IsValid() const noexcept {
      assert(this);
      return
#ifdef NDEBUG
          true;
#else
          Type == &typeid(TControlBlock) && This == this;
#endif
    }

    /* Our type-erased destroy function for the element. */
    TDestroy Destroy;

    /* Stores the number of TShared<> instances that point to the managed
       element. The life-time of the managed element is until SharedCount
       reaches zero. */
    std::atomic_size_t SharedCount;

    /* Stores the number of TWeak<> instances that point to the managed
       element. The life-time of the control block is until SharedCount and
       WeakCount both reach zero. */
    std::atomic_size_t WeakCount;

#ifdef NDEBUG
#else

    /* The pointer to the std::type_info of TControlBlock, used for the
       validity check where we match the 'magic constant'. */
    const std::type_info *Type;

    /* A pointer referring to itself. Matching the above 'magic constant' and
       the self-referencing pointer at this location makes up our validity
       check of the TControlBlock pointer. */
    TControlBlock *This;

#endif

  };  // TControlBlock

  /* Defines the shared shared pointer class described above. */
  template <typename TElem_>
  class TShared final {
    public:

    /* Bring our element type into scope for look-up. */
    using TElem = TElem_;

    /* Default-construct. Owns nothing. */
    TShared() noexcept : Elem(nullptr) {}

    /* Steal the state from the donor. Leaving it stateless. */
    TShared(TShared &&that) noexcept : Elem(that.Elem) { that.Elem = nullptr; }

    /* Steal the state from the donor. Leaving it stateless. */
    template <typename TThatElem,
              Mpl::EnableIf<std::is_convertible<TThatElem *, TElem *>>...>
    TShared(TShared<TThatElem> &&that) noexcept : Elem(that.Elem) {
      that.Elem = nullptr;
    }

    /* Share the ownership of the managed element with that. */
    TShared(const TShared &that) noexcept : TShared(that.Elem) {}

    /* Share the ownership of the managed element with that. */
    template <typename TThatElem,
              Mpl::EnableIf<std::is_convertible<TThatElem *, TElem *>>...>
    TShared(const TShared<TThatElem> &that) noexcept : TShared(that.Elem) {}

    /* If we're stateless, do nothing. Otherwise, decrement the shared count.
       The control block will take care of destroying things as it needs. */
    ~TShared() noexcept {
      assert(this);
      if (!*this) {
        return;
      }  // if
      TControlBlock::GetControlBlock(Elem)->DecrSharedCount();
      Elem = nullptr;
    }

    /* Returns true iff we have a state, returns false otherwise. */
    explicit operator bool() const noexcept {
      assert(this);
      return Elem;
    }

    /* Steal the state from the donor, leaving it stateless.
       If we already have a state, abandon it. */
    TShared &operator=(TShared &&that) noexcept {
      assert(this);
      TShared temp(std::move(that));
      return Swap(temp);
    }

    /* Steal the state from the donor, leaving it stateless.
       If we already have a state, abandon it. */
    template <typename TThatElem>
    TShared &operator=(TShared<TThatElem> &&that) {
      assert(this);
      TShared temp(std::move(that));
      return Swap(temp);
    }

    /* Share the state with the other shared.
       If we already have a state, abandon it. */
    TShared &operator=(const TShared &that) noexcept {
      assert(this);
      TShared temp(that);
      return Swap(temp);
    }

    /* Share the state with the other shared.
       If we already have a state, abandon it. */
    template <typename TThatElem>
    TShared &operator=(const TShared<TThatElem> &that) {
      assert(this);
      TShared temp(that);
      return Swap(temp);
    }

    /* Dereference the managed element. */
    template <typename TElem = TElem,
              Mpl::DisableIf<std::is_same<TElem, void>>...>
    TElem &operator*() const noexcept {
      assert(this);
      assert(Elem);
      return *Elem;
    }

    /* Dereference the managed element. */
    TElem *operator->() const noexcept {
      assert(this);
      return Elem;
    }

    /* Return the native pointer to the managed element. */
    TElem *Get() const noexcept {
      assert(this);
      return Elem;
    }

    /* Return the number of TShared<> instances that are currently pointing at
       the managed element. */
    std::size_t GetCount() const noexcept {
      assert(this);
      return Elem ? TControlBlock::GetControlBlock(Elem)->GetCount() : 0;
    }

    /* Returns true iff we're the only one referring to the managed element,
       otherwise return false. */
    bool IsUnique() const noexcept {
      assert(this);
      return GetCount() == 1;
    }

    /* Swap the pointers to the managed element. */
    TShared &Swap(TShared &that) noexcept {
      assert(this);
      assert(&that);
      if (this == &that) {
        return *this;
      }  // if
      std::swap(Elem, that.Elem);
      return *this;
    }

    /* Bring us back to the default-constructed state. */
    TShared &Reset() noexcept {
      assert(this);
      TShared temp;
      return Swap(temp);
    }

    private:

    /* Cache the element and increment the shared count. The given element must
       have a control block preceding it. */
    TShared(TElem *elem) noexcept : Elem(elem) {
      TControlBlock::GetControlBlock(Elem)->IncrSharedCount();
    }

    /* The pointer to the managed element. */
    TElem *Elem;

    /* Friend TShared<TThatElem> templates. */
    template <typename TThatElem>
    friend class TShared;

    /* Friend Share() function for it to use our private contructor. */
    template <typename TElem>
    friend TShared<TElem> Share(TElem &) noexcept;

  };  // TShared<TElem>

  /* Store the pointer to the already-managed element and increment the
     shared count. The pointer must already have a valid TControlBlock
     preceding it. Passing an element with no TControlBlock preceding it
     will cause assertion failures under debug builds and seg-faults in
     release mode, so be __sure__ to use it __correctly__!
     NOTE: If in doubt, use MakeShared<>... this is just an optimization. */
  template <typename TElem>
  TShared<TElem> Share(TElem &elem) noexcept {
    return TShared<TElem>(&elem);
  }

  /* TControlBlock::New<> handles allocation of the TControlBlock and TElem
     and returns the pointer to the element. */
  template <typename TElem, typename... TArgs>
  TShared<TElem> MakeShared(TArgs &&... args) {
    using elem_t = c14::decay_t<TElem>;
    auto buffer = TControlBlock::New(sizeof(elem_t), [](void * elem)->void {
      assert(elem);
      static_cast<elem_t *>(elem)->~elem_t();
    });
    auto *control_block = static_cast<TControlBlock *>(buffer.get());
    auto *elem =
        new (control_block->GetElem()) elem_t(std::forward<TArgs>(args)...);
    buffer.release();
    return Share(*elem);
  }

  /* Returns true iff the two TShared<> instances manage the same element,
     otherwise returns false. */
  template <typename TLhsElem, typename TRhsElem>
  bool operator==(const TShared<TLhsElem> &lhs,
                  const TShared<TRhsElem> &rhs) noexcept {
    assert(&lhs);
    assert(&rhs);
    return lhs.Get() == rhs.Get();
  }

  template <typename TElem>
  bool operator==(const TShared<TElem> &lhs, std::nullptr_t rhs) noexcept {
    assert(&lhs);
    return lhs.Get() == rhs;
  }

  template <typename TElem>
  bool operator==(std::nullptr_t lhs, const TShared<TElem> &rhs) noexcept {
    assert(&rhs);
    return lhs == rhs.Get();
  }

  /* Returns !(lhs == rhs). */
  template <typename TLhsElem, typename TRhsElem>
  bool operator!=(const TShared<TLhsElem> &lhs,
                  const TShared<TRhsElem> &rhs) noexcept {
    return !(lhs == rhs);
  }

  template <typename TElem>
  bool operator!=(const TShared<TElem> &lhs, std::nullptr_t rhs) noexcept {
    return !(lhs == rhs);
  }

  template <typename TElem>
  bool operator!=(std::nullptr_t lhs, const TShared<TElem> &rhs) noexcept {
    return !(lhs == rhs);
  }

  /* Compares the pointers to the managed elements by value. */
  template <typename TLhsElem, typename TRhsElem>
  bool operator<(const TShared<TLhsElem> &lhs,
                 const TShared<TRhsElem> &rhs) noexcept {
    assert(&lhs);
    assert(&rhs);
    using common_t = c14::common_type_t<TLhsElem *, TRhsElem *>;
    return std::less<common_t>()(lhs.Get(), rhs.Get());
  }

  template <typename TElem>
  bool operator<(const TShared<TElem> &lhs, std::nullptr_t rhs) noexcept {
    assert(&lhs);
    return std::less<TElem *>()(lhs.Get(), rhs);
  }

  template <typename TElem>
  bool operator<(std::nullptr_t lhs, const TShared<TElem> &rhs) noexcept {
    return std::less<TElem *>()(lhs, rhs.Get());
  }

  /* Returns rhs < lhs. */
  template <typename TLhsElem, typename TRhsElem>
  bool operator>(const TShared<TLhsElem> &lhs,
                 const TShared<TRhsElem> &rhs) noexcept {
    return rhs < lhs;
  }

  template <typename TElem>
  bool operator>(const TShared<TElem> &lhs, std::nullptr_t rhs) noexcept {
    return rhs < lhs;
  }

  template <typename TElem>
  bool operator>(std::nullptr_t lhs, const TShared<TElem> &rhs) noexcept {
    return rhs < lhs;
  }

  /* Returns !(rhs < lhs). */
  template <typename TLhsElem, typename TRhsElem>
  bool operator<=(const TShared<TLhsElem> &lhs,
                  const TShared<TRhsElem> &rhs) noexcept {
    return !(rhs < lhs);
  }

  template <typename TElem>
  bool operator<=(const TShared<TElem> &lhs, std::nullptr_t rhs) noexcept {
    return !(rhs < lhs);
  }

  template <typename TElem>
  bool operator<=(std::nullptr_t lhs, const TShared<TElem> &rhs) noexcept {
    return !(rhs < lhs);
  }

  /* Returns !(lhs < rhs). */
  template <typename TLhsElem, typename TRhsElem>
  bool operator>=(const TShared<TLhsElem> &lhs,
                  const TShared<TRhsElem> &rhs) noexcept {
    return !(lhs < rhs);
  }

  template <typename TElem>
  bool operator>=(const TShared<TElem> &lhs, std::nullptr_t rhs) noexcept {
    return !(lhs < rhs);
  }

  template <typename TElem>
  bool operator>=(std::nullptr_t lhs, const TShared<TElem> &rhs) noexcept {
    return !(lhs < rhs);
  }

  /* Output the internal pointer to the given output stream. */
  template <typename TChar, typename TCharTraits, typename TElem>
  std::basic_ostream<TChar, TCharTraits> &operator<<(
      std::basic_ostream<TChar, TCharTraits> &strm,
      const TShared<TElem> &that) {
    assert(&strm);
    assert(&that);
    return strm << that.Get();
  }

  /* Defines a class which holds a non-owning reference to an element managed
     by TShared. This class must be converted into a TShared<> before
     accessing the managed element. */
  template <typename TElem_>
  class TWeak final {
    public:

    /* Bring our managed element type into scope for look-up. */
    using TElem = TElem_;

    /* Default-construct. Owns nothing. */
    TWeak() noexcept : Elem(nullptr) {}

    /* Share the weak reference of the managed element with that. */
    template <typename TThatElem,
              Mpl::EnableIf<std::is_convertible<TThatElem *, TElem *>>...>
    TWeak(const TShared<TThatElem> &that) noexcept : TWeak(that.Get()) {}

    /* Share the weak reference of the managed element with that. */
    TWeak(const TWeak &that) noexcept : TWeak(that.Elem) {}

    /* Share the weak reference of the managed element with that. */
    template <typename TThatElem,
              Mpl::EnableIf<std::is_convertible<TThatElem *, TElem *>>...>
    TWeak(const TWeak<TThatElem> &that) noexcept : TWeak(that.Elem) {}

    /* If we're stateless, do nothing. Otherwise, decrement the weak count.
       The control block will take care of destroying things as it needs. */
    ~TWeak() {
      assert(this);
      if (!Elem) {
        return;
      }  // if
      TControlBlock::GetControlBlock(Elem)->DecrWeakCount();
      Elem = nullptr;
    }

    /* Share the state with the other shared.
       If we already have a state, abandon it. */
    template <typename TThatElem>
    TWeak &operator=(const TShared<TThatElem> &that) noexcept {
      assert(this);
      TWeak temp(that);
      return Swap(temp);
    }

    /* Share the state with the other shared.
       If we already have a state, abandon it. */
    TWeak &operator=(const TWeak &that) noexcept {
      assert(this);
      TWeak temp(that);
      return Swap(temp);
    }

    /* Share the state with the other shared.
       If we already have a state, abandon it. */
    template <typename TThatElem>
    TWeak &operator=(const TWeak<TThatElem> &that) noexcept {
      assert(this);
      TWeak temp(that);
      return Swap(temp);
    }

    /* Return true when there is no instance of TShared<> currently
       pointing at the manage element. Use this to check if an instance of
       TWeak<> is dead. */
    bool IsExpired() const noexcept {
      assert(this);
      return GetCount() == 0;
    }

    /* Return an instance of TShared<> which shares ownership of the managed
       element. If the life-time of the managed element is expired, returns an
       empty TShared<> instance. */
    TShared<TElem> Lock() const noexcept {
      assert(this);
      return IsExpired() ? TShared<TElem>() : Share(*Elem);
    }

    /* Return the number of TShared<> instances that are currently pointing at
       the managed element. */
    std::size_t GetCount() const noexcept {
      assert(this);
      return Elem ? TControlBlock::GetControlBlock(Elem)->GetCount() : 0;
    }

    /* Swap the pointers to the managed element. */
    TWeak &Swap(TWeak &that) noexcept {
      assert(this);
      assert(&that);
      if (this == &that) {
        return *this;
      }  // if
      std::swap(Elem, that.Elem);
      return *this;
    }

    /* Bring us back to the default-constructed state. */
    TWeak &Reset() noexcept {
      assert(this);
      TWeak temp;
      return Swap(temp);
    }

    private:

    /* Store the pointer to the already-managed element and increment the
       weak count. The pointer must already have a valid TControlBlock
       preceding it. Passing an element with no TControlBlock preceding it
       will cause assertion failures under debug builds and seg-faults in
       release mode, so be __sure__ to use it __correctly__! */
    TWeak(TElem *elem) noexcept : Elem(elem) {
      TControlBlock::GetControlBlock(Elem)->IncrWeakCount();
    }

    /* The pointer to the managed element. */
    TElem *Elem;

    /* Friend TWeak<TThatElem> templates. */
    template <typename TThatElem>
    friend class TWeak;

  };  // TWeak<TElem>

}  // Visitor

namespace std {

  /* std::hash<> specialization. */
  template <typename TElem>
  struct hash<Visitor::TShared<TElem>> {

    using argument_type = Visitor::TShared<TElem>;
    using result_type = std::size_t;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      return std::hash<TElem *>()(that.Get());
    }

  };  // hash<Visitor::TShared<TElem>>

  /* std::swap<> specialization. */
  template <typename TElem>
  void swap(Visitor::TShared<TElem> &lhs,
            Visitor::TShared<TElem> &rhs) noexcept {
    assert(&lhs);
    lhs.Swap(rhs);
  }

  /* std::swap<> specialization. */
  template <typename TElem>
  void swap(Visitor::TWeak<TElem> &lhs, Visitor::TWeak<TElem> &rhs) noexcept {
    assert(&lhs);
    lhs.Swap(rhs);
  }

}  // std
