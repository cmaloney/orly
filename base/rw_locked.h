/* <base/rw_locked.h>

   Wrapper which forces acquisition of a lock before access to an object can be had. Offers both shared and exclusive
   locks.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <mutex>
#include <shared_mutex>
#include <utility>

#include <base/class_traits.h>

namespace Base {
  template <typename TVal>
  class TRwLocked final {
    NO_COPY(TRwLocked);
    NO_MOVE(TRwLocked);

    public:

    //NOTE: We could make this one TLock, which never consts the object, giving us the access to atomics we __should__
    // have (and other things don't need to lock around). But meh.
    class TSharedLock final {
      NO_COPY(TSharedLock);
      NO_MOVE(TSharedLock);
      public:

      TSharedLock(const TRwLocked &locked) : Locked(locked), Lock(locked.Mutex) {}

      const TVal *operator->() const {
        return &Locked.Val;
      }

      const TVal &operator*() const {
        return Locked.Val;
      }

      private:
      const TRwLocked &Locked;
      std::shared_lock<std::shared_timed_mutex> Lock;

      friend class TRwLocked;
    };

    class TUniqueLock final {
      NO_COPY(TUniqueLock);
      NO_MOVE(TUniqueLock);
      public:

      TUniqueLock(TRwLocked &locked) : Locked(locked), Lock(locked.Mutex) {}

      TVal *operator->() {
        return &Locked.Val;
      }

      TVal &operator*() {
        return Locked.Val;
      }

      private:
      TRwLocked &Locked;
      std::unique_lock<std::shared_timed_mutex> Lock;

      friend class TRwLocked;
    };

    template<typename... TArgs>
    TRwLocked(TArgs &&...args) : Val(std::forward<TArgs>(args)...) {}

    // NOTE: Safety hatch to allow access to things like atomic variables.
    const TVal &UnsafeAccess() const {
      return Val;
    }

    // NOTE: Safety hatch to allow access to things like atomic variables.
    TVal &UnsafeAccess() {
      return Val;
    }

    private:
    TVal Val;
    mutable std::shared_timed_mutex Mutex;
  };
}
