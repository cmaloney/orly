/* <base/potato.h>

   A hot potato that can be fought over by shared and exclusive users.

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
#include <memory>

#include <pthread.h>

#include <base/no_copy_semantics.h>

namespace Base {

  /* A hot potato that can be fought over by shared and exclusive users. */
  class TPotato {
    NO_COPY_SEMANTICS(TPotato);
    public:

    /* The base class for shared and exclusive locks.  Any lock will keep a potato alive. */
    class TLock {
      NO_COPY_SEMANTICS(TLock);
      public:

      /* The potato to which this lock is attached. */
      const std::shared_ptr<TPotato> &GetPotato() const {
        assert(this);
        return Potato;
      }

      protected:

      /* Do-little. */
      TLock(const std::shared_ptr<TPotato> &potato)
          : Potato(potato) {}

      /* Do-little. */
      virtual ~TLock();

      /* Release the lock. */
      void PreDtor();

      private:

      /* See accessor. */
      std::shared_ptr<TPotato> Potato;

    };  // Lock

    /* Keeps the potato alive and holds an exclusive lock on it. */
    class TExclusiveLock
        : public TLock {
      NO_COPY_SEMANTICS(TExclusiveLock);
      public:

      /* Block until the lock is granted. */
      TExclusiveLock(const std::shared_ptr<TPotato> &potato);

      /* Release the lock. */
      ~TExclusiveLock();

    };  // TExclusiveLock

    /* Keeps the potato alive and holds a shared lock on it. */
    class TSharedLock
        : public TLock {
      NO_COPY_SEMANTICS(TSharedLock);
      public:

      /* Block until the lock is granted. */
      TSharedLock(const std::shared_ptr<TPotato> &potato);

      /* Release the lock. */
      ~TSharedLock();

    };  // TSharedLock

    /* NOTE: Because of the way std::shared_ptr<> works, we need to make the constructor and destructor public.
       But for the LOVE OF ALL THAT'S HOLY, don't construct and destroy potatoes on your own.  Use the New()
       function for construction and let the shared_ptr<> semantics decide when to destroy. */
    TPotato();

    /* NOTE: See constructor for dire warnings. */
    ~TPotato();

    /* Make me a brand new potato.  All shiny. */
    static std::shared_ptr<TPotato> New() {
      return std::make_shared<TPotato>();
    }

    private:

    /* The the pthreads object which we use to do our actual thread-blocking. */
    pthread_rwlock_t Lockable;

  };  // TPotato

}  // Base
