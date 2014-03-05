/* <base/ref_counted.h>

   Defines the base class and smart pointer for all reference-counted objects.

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

#include <algorithm>
#include <cassert>

#include <base/no_copy_semantics.h>

namespace Base {

  /* The base class for all reference-counted objects.  Inherit from this class and make your
     constructor and destructor private.  Also provide a static factory which returns a
     TRefCounted::TPtr<> instance, like this:

        class TFoo
            : public TRefCounted {
          NO_COPY_SEMANTICS(TFoo);
          public:

          typedef TRefCounted::TPtr<TFoo> TPtr;

          static TPtr New() {
            return AsPtr(new TFoo);
          }

          private:

          TFoo() {}

          ~TFoo() {}

        };

     After construction, you can use instances of TFoo::TPtr as references to the single, shared
     instance of TFoo.  Copying the pointer increases the reference count, but does not copy TFoo.
     When the last TPtr instance goes out of scope, the TFoo instance has the chance to take a
     final action, such as destroying itself.

     Ref-count increment and decrement operations are syncronized between threads, so it is safe
     for multiple threads to refer to the same ref-counted object.  However, operations on the
     shared object itself must still be syncronized by means of a mutex or something similar.

     Note that it is not possible to tell whether a given instance of TRefCounted is being referred
     to by more than one instance of TRefCounted::TPtr<>.  This is intentional; you should not rely
     on the presence of absence of smart-pointers to provide syncronization.*/
  class TRefCounted {
    NO_COPY_SEMANTICS(TRefCounted);
    public:

    /* A smart pointer to a ref-counted object.  This is a value type.
       The TObj argument must inherit from TRefCounted. */
    template <typename TObj>
    class TPtr {
      public:

      /* The default is a null pointer. */
      TPtr() : Obj(0) {}

      /* Swap constructor.  The donor ptr is left null. */
      TPtr(TPtr &&that) {
        assert(this);
        assert(&that);
        Obj = that.Obj;
        that.Obj = 0;
      }

      /* Copy constructor.  This increments the reference count. */
      TPtr(const TPtr &that) {
        assert(this);
        assert(&that);
        Obj = that.Obj;
        if (Obj) {
          Obj->IncrRefCount();
        }
      }

      /* Up-casting copy constructor.  Same as above, but between
         up-castable types. */
      template <typename TDown>
      TPtr(const TPtr<TDown> &that) {
        assert(this);
        assert(&that);
        Obj = that.Obj;
        if (Obj) {
          Obj->IncrRefCount();
        }
      }

      /* Decrements the reference count.  Destroying the last pointer
         triggers the shared object's finalization routine. */
      ~TPtr() {
        assert(this);
        if (Obj) {
          Obj->DecrRefCount();
        }
      }

      /* Swaperator. */
      TPtr &operator=(TPtr &&that) {
        assert(this);
        assert(&that);
        std::swap(Obj, that.Obj);
        return *this;
      }

      /* Assignment operator.  This increments the reference count of
         the new object and decrements the reference count of the old
         object.  The operation is exception-safe. */
      TPtr &operator=(const TPtr &that) {
        assert(this);
        return *this = TPtr(that);
      }

      /* Up-casting assignment operator.  Same as above, but between
         up-castable types. */
      template <typename TDown>
      TPtr &operator=(const TPtr<TDown> &that) {
        assert(this);
        return *this = TPtr(that);
      }

      /* Returns true iff. the pointer is non-null. */
      operator bool() const {
        assert(this);
        return Obj != 0;
      }

      /* Returns a reference to the reference-counted object.
         This function can return null. */
      TObj &operator*() const {
        assert(this);
        return *Obj;
      }

      /* Returns a reference to the reference-counted object.
         This function can return null. */
      TObj *operator->() const {
        assert(this);
        return Obj;
      }

      /* Resets the pointer to the default (null) state.
        Returns a chaining reference. */
      TPtr &Reset() {
        assert(this);
        return *this = TPtr();
      }

      private:

      /* A pointer to the ref-counted object, possibly shared with other pointers.
         This can be null. */
      TObj *Obj;

      /* This friendship allows TRefCounted::AsPtr to work. */
      friend class TRefCounted;

    };  // TPtr<TObj>

    protected:

    /* Default constructor. */
    TRefCounted() : RefCount(0) {}

    /* Destructor should never be directly invoked.  It should only run as a
       side-effect of the ref-count decrementing to zero. */
    virtual ~TRefCounted();

    /* Invoked when the last reference goes away.
       The default implementation deletes the object. */
    virtual void Finalize();

    /* Returns a smart-pointer to the given ref-counted object, which must have
       no smart-pointers referring to it already.  This function is intended be
       used in static factory functions to create the initial smart-pointer
       referring to a newly constructed object. */
    template <typename TObj>
    static TPtr<TObj> AsPtr(TObj *obj) {
      assert(obj);
      TPtr<TObj> ptr;
      ptr.Obj = obj;
      bool success = __sync_bool_compare_and_swap(&(obj->RefCount), 0, 1);
      /* This assertion will fail if you attempt to use TRefCounted::AsPtr() to
         create a new smart-pointer to an object with existing smart-pointers. */
      assert(success);
      return ptr;
    }

    /* WARNING: THIS IS TO SUPPORT A CALL IN PAGED_ARENA. DO NOT USE ELSEWHERE */
    /* Convert a weak refernce to a strong one.  This will work on a newly-created native pointer or on an existing one. */
    template <typename TObj>
    static TPtr<TObj> Strengthen(TObj *obj) {
      assert(obj);
      TPtr<TObj> ptr;
      ptr.Obj = obj;
      __sync_add_and_fetch(&(obj->RefCount), 1);
      return ptr;
    }

    private:

    /* Decrement the ref-count.  If it has reached zero, invoke the finalizer.
       The operation is syncronized between threads. */
    void DecrRefCount() {
      assert(this);
      int ref_count = __sync_sub_and_fetch(&RefCount, 1);
      assert(ref_count >= 0);
      if (!ref_count) {
        Finalize();
      }
    }

    /* Increment the ref-count.  The operation is syncronized between threads. */
    void IncrRefCount() {
      assert(this);
      int ref_count = __sync_add_and_fetch(&RefCount, 1);
      assert(ref_count > 0);
    }

    /* The number of smart-pointers currently referring to this object.  This
       value is never negative.  It is zero only when the object is newly constructed
       (before AsPtr() is called on it) or after DecrRefCount() has decremented away
       the last reference and finalizer has been called. */
    int RefCount;

  };  // TRefCounter

}  // Base
