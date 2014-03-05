/* <stig/durable/kit.h>

   Classes to define, manage, and share durable objects.

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
#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <utility>

#include <base/event_semaphore.h>
#include <base/no_construction.h>
#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <base/thrower.h>
#include <base/uuid.h>
#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/binary_output_stream.h>
#include <io/recorder_and_player.h>
#include <stig/indy/fiber/fiber.h>

namespace Stig {

  namespace Indy {

    /* Forward Declaration. */
    class TManager;

  }  // Indy

  namespace Durable {

    /* TAnyPtr and TPtr<> require this forward declaration. */
    class TObj;

    /* We use standard UUIDs as our unique ids. */
    using TId = Base::TUuid;

    /* We characterize time-to-live in seconds. */
    using TTtl = std::chrono::seconds;

    /* We characterize a deadline as a point in time on the system clock. */
    using TDeadline = std::chrono::time_point<std::chrono::system_clock>;

    /* We use instances of this type when waiting for the disk to act. */
    using TSem = Base::TEventSemaphore;

    /* Used to disambiguate the private constructors of TPtr<>. */
    enum TNew { New };
    enum TOld { Old };

    /* The errors we throw. */
    DEFINE_ERROR(TAlreadyExists, std::runtime_error, "durable object already exists");
    DEFINE_ERROR(TDoesntExist, std::runtime_error, "durable object doesn't exist");
    DEFINE_ERROR(TWrongType, std::runtime_error, "durable object of wrong type");

    /* The base class for all pointers to durable objects. */
    class TAnyPtr {
      public:

      /* Do-little. */
      virtual ~TAnyPtr() {}

      protected:

      /* Do-little. */
      TAnyPtr() {}

      /* Force the pointer to give up its object WITHOUT decrementing the object's pointer count.
         Return the object at which the pointer was pointing, if any. */
      virtual TObj *ForceRelease() = 0;

      /* For ForceRelease(). */
      friend class TObj;

    };  // TAnyPtr

    /* A shared pointer to a durable object. */
    template <typename TSomeObj>
    class TPtr final
        : public TAnyPtr {
      public:

      /* Default-construct as a null pointer. */
      TPtr();

      /* Move-construct from a donor pointer of our own type, leaving the donor null.
         If the donor is already null, both pointers will end up null. */
      TPtr(TPtr &&that);

      /* Copy-construct from a pointer of our own type.
         The durable will be shared between the pointers.
         If the given pointer is null, so will the new pointer be. */
      TPtr(const TPtr &that);

      /* Copy-construct from a pointer of a different, but up-castable, type.
         The durable will be shared between the pointers.
         If the given pointer is null, so will the new pointer be. */
      template <typename TOtherObj>
      TPtr(const TPtr<TOtherObj> &that);

      /* Release our hold on the durable.
         If we're the last pointer, the durable will close. */
      ~TPtr();

      /* Release our hold on our existing durable, if any, and move-assign from a donor pointer, leaving the donor null.
         If the donor is already null, both pointers will end up null. */
      TPtr &operator=(TPtr &&that);

      /* Release our hold on our existing durable, if any, and assign from a pointer of our own type.
         The given pointer's durable will be shared between the pointers.
         If the given pointer is null, so will we be. */
      TPtr &operator=(const TPtr &that);

      /* True iff. this pointer and that one point to the same object. */
      bool operator==(const TPtr &that) const;

      /* True iff. this pointer and that one point to different objects. */
      bool operator!=(const TPtr &that) const;

      /* The address of the object at which we point, which is guaranteed to be a unique hash. */
      size_t GetHash() const;

      /* Release our hold on our existing durable, if any, and assign from a pointer of a different, but up-castable, type.
         The given pointer's durable will be shared between the pointers.
         If the given pointer is null, so will we be. */
      template <typename TOtherObj>
      TPtr &operator=(const TPtr<TOtherObj> &that);

      /* True iff. we're non-null. */
      operator bool() const;

      /* The durable to which we we point, if any. */
      TSomeObj &operator*() const;

      /* The durable to which we we point, if any. */
      TSomeObj *operator->() const;

      /* The durable to which we we point, if any. */
      TSomeObj *Get() const;

      /* Release our hold on our existing durable, if any, and reset to the default-constructed state; that is, become null. */
      TPtr &Reset();

      private:

      /* Adopt the given native pointer, which must not be null.
         This constructor is used (and should only be used) by TManager::New() or TManager::TOpen() to return newly created objects. */
      TPtr(TSomeObj *some_obj, TNew) noexcept;

      /* Adopt the given native pointer, which must not be null.
         Dynamic-cast the object to our type and, if the object is of the wrong type, throw.
         This constructor is used (and should only be used) by TManager::Open() to return objects already in use. */
      TPtr(TObj *obj, TOld);

      /* See base class. */
      virtual TObj *ForceRelease() override {
        assert(this);
        auto *temp = SomeObj;
        SomeObj = nullptr;
        return temp;
      }

      /* The durable object we refer to (and keep alive), if any. */
      TSomeObj *SomeObj;

      /* For the private constructors. */
      friend class TManager;

    };  // TPtr<TSomeObj>

    /* The base class for managers of durable objects. */
    class TManager
        : public Indy::Fiber::TRunnable {
      NO_COPY_SEMANTICS(TManager);
      public:

      /* Evict and destroy all objects whose deadlines have passed.
         This will affect the cache as well as the disk. */
      void Clean();

      /* Create a new durable with the given id and ttl and return a shared pointer to it.
         If there is already a durable with the given id, throw.
         This function will block until the durable is created.
         If the durable replicates to the slave, this function will block until replication is confirmed.
         The durable will initially belong only to the calling thread; however, that thread is free to pass the shared pointer to other threads.
         If multiple threads try to modify the durable, additional synchronization will be required.
         When the last shared pointer is released, the durable will close.
         If the closed durable has a zero ttl, it will be destroyed immediately.
         If the closed durable has a non-zero ttl, it will be written immediately to disk and kept in cache, and its ttl will start running down.
         When the durable's ttl expires, the cleaner is free to remove it from disk and cache.
         The cleaner may also remove the durable from cache when space is required, but it will remain on disk at least until its ttl expires. */
      template <typename TSomeObj, typename... TArgs>
      TPtr<TSomeObj> New(const TId &id, const TTtl &ttl, TArgs &&... args);

      /* Find the durable with the given id and return a shared pointer to it.
         If there is no such durable, or the durable is of a different type, throw.
         This function will block until the durable is opened.
         The durable will be shared between all threads which ask for it.
         If multiple threads try to modify the durable, additional synchronization will be required.
         When the last shared pointer is released, the durable will close.
         If the closed durable has a zero ttl, it will be destroyed immediately.
         If the closed durable has a non-zero ttl, it will be written immediately to disk and kept in cache, and its ttl will start running down.
         When the durable's ttl expires, the cleaner is free to remove it from disk and cache.
         The cleaner may also remove the durable from cache when space is required, but it will remain on disk at least until its ttl expires. */
      template <typename TSomeObj>
      TPtr<TSomeObj> Open(const TId &id);

      /* TODO */
      void Clear();

      /* TODO */
      virtual void RunLayerCleaner() = 0;

      protected:

      /* The cache size is the maximum number of closed objects we will keep in memory. */
      TManager(size_t max_cache_size);

      /* Destroy all our objects on the way out. */
      virtual ~TManager();

      /* Override to search the disk for an object with the given id.
         If found, return true; else, return false.
         Assume that the mutex has already been obtained. */
      virtual bool CanLoad(const TId &id) = 0;

      /* Override to erase from disk all objects which have a deadline <= the given time.
         Assume that the mutex has already been obtained.
         When the task is complete, push the semaphore.  Do not delete the semaphore. */
      virtual void CleanDisk(const TDeadline &now, TSem *sem) = 0;

      /* Override to erase the given object from disk.
         If the object does not exist, there is a logic error in the program or a disk failure.
         Assume that the mutex has already been obtained.
         When the task is complete, push the semaphore.  Do not delete the semaphore. */
      virtual void Delete(const TId &id, TSem *sem) = 0;

      /* Override to save the given object to disk.
         Assume that the mutex has already been obtained.
         When the task is complete, push the semaphore.  Do not delete the semaphore. */
      virtual void Save(const TId &id, const TDeadline &deadline, const TTtl &ttl, const std::string &blob, TSem *sem) = 0;

      /* Override to search the disk for an object with the given id.
         If found, return the object's blob (via out-parameter) and return true; else, ignore the out-parameter and return false.
         Assume that the mutex has already been obtained. */
      virtual bool TryLoad(const TId &id, std::string &blob) = 0;

      private:

      /* Evict the given object from the set of openable objects, then destroy the object.
         NOTE 1: The object pointer passed to this function WILL BE BAD by the time this function returns.
         NOTE 2: This function assumes that the mutex has already been obtained. */
      void DestroyObj(TObj *obj) noexcept;

      /* If we're caching, insert the given object into the set of closed objects.
         Discard enough old objects to make room.
         Return true iff. the object is successfully cached.
         This function assumes that the mutex has already been obtained. */
      bool TryCacheObj(TObj *obj) noexcept;

      /* The maximum number of objects to keep in ClosedObjs. */
      size_t MaxCacheSize;

      /* Covers OpenableObj, ClosedObjs, and all instances of TObj::PtrCount. */
      std::mutex Mutex;

      /* All the objects currently open as well as those which are closed but cached.
         These are objects which can be found by the Open() function.
         If a closed object (that is, one with a PtrCount of zero) is in this container, it will also be in ClosedObjs. */
      std::unordered_map<TId, TObj *> OpenableObjs;

      /* All the objects currently closed but still cached.
         They are in order by their deadlines, soonest deadlines first.
         The size of this container will never exceed MaxCacheSize.
         All of the objects in this container have a PtrCount of zero and also appear in OpenableObjs. */
      std::map<std::pair<TDeadline, TId>, TObj *> ClosedObjs;

      /* For do-stuff-to-obj functions and the mutex. */
      friend class TObj;

      /* for saving replicated durables. */
      friend class Stig::Indy::TManager;

    };  // TManager

    /* The base class for durable objects. */
    class TObj {
      NO_COPY_SEMANTICS(TObj);
      public:

      /* If the object is closed, then this is the latest time at which the object is guaranteed to continue to exist.
         Once the deadline expires, the cleaner is free to destroy this object.
         If the object is open, this is unknown.
         An object which is open cannot be destroyed by the cleaner, so it has no deadline. */
      const Base::TOpt<TDeadline> &GetDeadline() const {
        assert(this);
        return Deadline;
      }

      /* The unique id of this durable object. */
      const TId &GetId() const {
        assert(this);
        return Id;
      }

      /* Overide this to provide a human-readable c-string describing what kind of durable object this is.
         Do not return null. */
      virtual const char *GetKind() const noexcept;

      /* The manager to which this durable object belongs. */
      TManager *GetManager() const {
        assert(this);
        return Manager;
      }

      /* The minimum amount of time this object will live after it is closed.
         If this value is zero, then this object will be destroyed immediately after it is closed. */
      const TTtl &GetTtl() const {
        assert(this);
        return Ttl;
      }

      /* True iff. this object has ever been saved to disk; otherwise, false. */
      bool IsOnDisk() const {
        assert(this);
        return OnDisk;
      }

      /* Make an entry in the syslog regarding this object. */
      void Log(int level, const char *action, const std::exception &ex) const noexcept;

      /* Make an entry in the syslog regarding this object. */
      void Log(int level, const char *action, const char *msg) const noexcept;

      /* Change the time-to-live.  The time must be non-negatvie and the object must be open. */
      void SetTtl(const TTtl &ttl);

      /* Override this function to stream private object state to serialized blob form.
         Call this version first, then stream your own data.
         The blob formed here must be readable by the stream-based constructor of your final type. */
      virtual void Write(Io::TBinaryOutputStream &strm) const;

      protected:

      /* Caches the construction parameters.  The pointer to the manager must not be null.
         In your final type, provide your own constructor which takes these paramters and passes them to this constructor. */
      TObj(TManager *manager, const TId &id, const TTtl &ttl);

      /* Caches the manager and id and streams in the rest of the state.
         In your final type, provide your own constructor which takes these paramters and passes them to this constructor. */
      TObj(TManager *manager, const TId &id, Io::TBinaryInputStream &strm);

      /* Do-little.  Keep the destructor of your final type private. */
      virtual ~TObj();

      /* Override to call back for each durable object at which we point.
         NOTE: Do NOT invoke any shared pointer copy-constructors or you will deadlock.
         The pointer you call back with may be altered.  In particular, you should expect it to be set null.
         The default implementation of this function returns no dependent objects. */
      virtual bool ForEachDependentPtr(const std::function<bool (TAnyPtr &)> &cb) noexcept;

      private:

      /* Increment the count of pointers currently sharing this durable object.
         This operation is syncrhonized between threads. */
      void OnPtrAcquire() noexcept;

      /* Decrement the count of pointers currently sharing this durable object.
         If this causes the count to reach zero, close the durable object.
         If the close requires an asynchronous disk operation, wait for it to complete.
         This operation is syncrhonized between threads; however, the lock will be released before waiting for the async operation, if any.
         NOTE: By the time this function returns, 'this' may be a bad pointer. */
      void OnPtrRelease() noexcept;

      /* Transition the pointer count from zero to one.
         This is done when the object is adopted by its first pointer.
         NOTE: This function assumes that the manager's mutex has already been obtained. */
      void OnPtrAdoptNew() noexcept;

      /* Increment the count of pointers currently sharing this durable object.
         This is done when the object is adopted by a pointer which is not the object's first.
         NOTE: This function assumes that the manager's mutex has already been obtained. */
      void OnPtrAdoptOld() noexcept;

      /* See accessor. */
      TManager *Manager;

      /* See accessor. */
      TId Id;

      /* The number of pointers currently sharing this durable object.
         If this count is greater than zero, it means the durable object is open.
         If this count is zero, it means the durable object is closed but being held in cache. */
      size_t PtrCount;

      /* See accessor. */
      bool OnDisk;

      /* See accessor. */
      TTtl Ttl;

      /* See accessor. */
      Base::TOpt<TDeadline> Deadline;

      /* A semaphore to use during destruction, if we trigger asynchonous events. */
      TSem *Sem;

      /* For on-ptr-event functions. */
      template <typename>
      friend class TPtr;

      /* For ~TObj() & Deadline. */
      friend class TManager;

    };  // TObj

    /*
     *  Definitions of TPtr<> members.
     */

    template <typename TSomeObj>
    TPtr<TSomeObj>::TPtr()
        : SomeObj(nullptr) {}

    template <typename TSomeObj>
    TPtr<TSomeObj>::TPtr(TPtr &&that) {
      assert(&that);
      SomeObj = that.SomeObj;
      that.SomeObj = nullptr;
    }

    template <typename TSomeObj>
    TPtr<TSomeObj>::TPtr(const TPtr &that) {
      assert(&that);
      if ((SomeObj = that.SomeObj) != nullptr) {
        SomeObj->OnPtrAcquire();
      }
    }

    template <typename TSomeObj>
    template <typename TOtherObj>
    TPtr<TSomeObj>::TPtr(const TPtr<TOtherObj> &that) {
      assert(&that);
      if ((SomeObj = that.SomeObj) != nullptr) {
        SomeObj->OnPtrAcquire();
      }
    }

    template <typename TSomeObj>
    TPtr<TSomeObj>::~TPtr() {
      assert(this);
      if (SomeObj) {
        SomeObj->OnPtrRelease();
      }
    }

    template <typename TSomeObj>
    TPtr<TSomeObj> &TPtr<TSomeObj>::operator=(TPtr &&that) {
      assert(this);
      assert(&that);
      std::swap(SomeObj, that.SomeObj);
      return *this;
    }

    template <typename TSomeObj>
    TPtr<TSomeObj> &TPtr<TSomeObj>::operator=(const TPtr &that) {
      assert(this);
      return *this = TPtr(that);
    }

    template <typename TSomeObj>
    bool TPtr<TSomeObj>::operator==(const TPtr &that) const {
      assert(this);
      return SomeObj == that.SomeObj;
    }

    template <typename TSomeObj>
    bool TPtr<TSomeObj>::operator!=(const TPtr &that) const {
      assert(this);
      return SomeObj != that.SomeObj;
    }

    template <typename TSomeObj>
    size_t TPtr<TSomeObj>::GetHash() const {
      assert(this);
      return reinterpret_cast<size_t>(SomeObj);
    }

    template <typename TSomeObj>
    template <typename TOtherObj>
    TPtr<TSomeObj> &TPtr<TSomeObj>::operator=(const TPtr<TOtherObj> &that) {
      assert(this);
      return *this = TPtr(that);
    }

    template <typename TSomeObj>
    TPtr<TSomeObj>::operator bool() const {
      assert(this);
      return SomeObj != nullptr;
    }

    template <typename TSomeObj>
    TSomeObj &TPtr<TSomeObj>::operator*() const {
      assert(this);
      return *SomeObj;
    }

    template <typename TSomeObj>
    TSomeObj *TPtr<TSomeObj>::operator->() const {
      assert(this);
      return SomeObj;
    }

    template <typename TSomeObj>
    TSomeObj *TPtr<TSomeObj>::Get() const {
      assert(this);
      return SomeObj;
    }

    template <typename TSomeObj>
    TPtr<TSomeObj> &TPtr<TSomeObj>::Reset() {
      assert(this);
      return *this = TPtr();
    }

    template <typename TSomeObj>
    TPtr<TSomeObj>::TPtr(TSomeObj *some_obj, TNew) noexcept
        : SomeObj(some_obj) {
      assert(some_obj);
      some_obj->OnPtrAdoptNew();
    }

    template <typename TSomeObj>
    TPtr<TSomeObj>::TPtr(TObj *obj, TOld) {
      assert(this);
      assert(obj);
      SomeObj = dynamic_cast<TSomeObj *>(obj);
      if (!SomeObj) {
        THROW_ERROR(TWrongType) << "expected \"" << typeid(TSomeObj).name() << "\", got \"" << typeid(obj).name() << '"';
      }
      obj->OnPtrAdoptOld();
    }

    /*
     *  Definitions of TManager templatized members.
     */

    template <typename TSomeObj, typename... TArgs>
    TPtr<TSomeObj> TManager::New(const TId &id, const TTtl &ttl, TArgs &&... args) {
      assert(this);
      /* Lock the manager and create the requested slot among the openable objects.
         If the slot already exists, throw. */
      std::lock_guard<std::mutex> lock(Mutex);
      auto iter = OpenableObjs.insert(std::pair<TId, TObj *>(id, nullptr)).first;
      TObj *&openable_obj = iter->second;
      if (openable_obj) {
        THROW_ERROR(TAlreadyExists) << "in cache" << Base::EndOfPart << "id = " << id;
      }
      try {
        /* If the disk store already has an object with the given id, throw. */
        if (CanLoad(id)) {
          THROW_ERROR(TAlreadyExists) << "on disk" << Base::EndOfPart << "id = " << id;
        }
        /* Construct the new object and keep it in the openable set. */
        TSomeObj *some_obj = new TSomeObj(this, id, ttl, std::forward<TArgs>(args)...);

        if (ttl.count() > 0) {
          /* save the object so it gets replicated if it has a non-zero ttl */ {
            TDeadline deadline = TDeadline::clock::now() + ttl;
            std::string blob;
            /* extra */ {
              auto recorder = std::make_shared<Io::TRecorder>();
              Io::TBinaryOutputOnlyStream strm(recorder);
              some_obj->Write(strm);
              strm.Flush();
              recorder->CopyOut(blob);
            }
            TSem sem;
            Save(id, deadline, ttl, blob, &sem);
          }
        }

        openable_obj = some_obj;
        return TPtr<TSomeObj>(some_obj, Stig::Durable::New);
      } catch (...) {
        /* We already had the object on disk or the object's constructor failed.
           Either way, we need to dispose of the slot we made before continuing to handle the error. */
        OpenableObjs.erase(iter);
        throw;
      }
    }

    template <typename TSomeObj>
    TPtr<TSomeObj> TManager::Open(const TId &id) {
      assert(this);
      /* Lock the manager and find/create the requested slot among the openable objects. */
      std::lock_guard<std::mutex> lock(Mutex);
      auto iter = OpenableObjs.insert(std::pair<TId, TObj *>(id, nullptr)).first;
      TObj *&openable_obj = iter->second;
      if (openable_obj) {
        /* We found an object with the given id. */
        TPtr<TSomeObj> ptr(openable_obj, Stig::Durable::Old);
        const auto &deadline = openable_obj->GetDeadline();
        if (deadline) {
          /* The object is being re-opened from a closed state, so remove it from the set of closed objects. */
          size_t erased_from_closed = ClosedObjs.erase(std::make_pair(*deadline, id));
          assert(erased_from_closed == 1);
          openable_obj->Deadline.Reset();
        }
        return ptr;
      }
      try {
        /* Load the object from disk and keep it in the openable set. */
        std::string blob;
        if (!TryLoad(id, blob)) {
          THROW_ERROR(TDoesntExist) << "id = " << id;
        }
        Io::TBinaryInputOnlyStream strm(std::make_shared<Io::TPlayer>(std::make_shared<Io::TRecorder>(blob)));
        TSomeObj *some_obj = new TSomeObj(this, id, strm);
        openable_obj = some_obj;
        return TPtr<TSomeObj>(some_obj, Stig::Durable::New);
      } catch (...) {
        /* We could not find the object on disk or the object's constructor failed.
           Either way, we need to dispose of the slot we made before continuing to handle the error. */
        OpenableObjs.erase(iter);
        throw;
      }
    }

  }  // Durable

}  // Stig


namespace std {

  /* A standard hasher for Stig::Durable::TPtr. */
  template <typename TSomeObj>
  struct hash<Stig::Durable::TPtr<TSomeObj>> {
    typedef size_t result_type;
    typedef Stig::Durable::TPtr<TSomeObj> argument_type;
    size_t operator()(const Stig::Durable::TPtr<TSomeObj> &that) const {
      return that.GetHash();
    }
  };

}  // std
