/* <stig/spa/flux_capacitor/api.h>

   TODO

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

#include <chrono>
#include <future>
#include <sys/epoll.h>
#include <thread>

#include <base/assert_true.h>
#include <base/error.h>
#include <base/opt.h>
#include <base/os_error.h>
#include <base/ref_counted.h>
#include <base/zero.h>
#include <inv_con/ordered_list.h>
#include <inv_con/unordered_multimap.h>
#include <multi_event/multi_event.h>
#include <stig/any_honcho.h>
#include <stig/spa/flux_capacitor/flux_capacitor.h>
#include <stig/uuid.h>

namespace Stig {

  /* Forward Declaration. */
  class THoncho;

  namespace Spa {

    /* Forward Decl for friendship */
    class TService;

    namespace FluxCapacitor {

      /* TODO */
      class TSessionError : public Base::TFinalError<TSessionError> {
        public:

        /* TODO */
        TSessionError(const Base::TCodeLocation &loc, const char *msg) {
          PostCtor(loc, msg);
        }

      };  // TSessionError

      /* TODO */
      class TSharedPovError : public Base::TFinalError<TSharedPovError> {
        public:

        /* Constructor. */
        TSharedPovError(const Base::TCodeLocation &loc, const char *msg) {
          PostCtor(loc, msg);
        }
      };  // TSharedPovError

      /* TODO */
      class TPrivatePovError : public Base::TFinalError<TPrivatePovError> {
        public:

        /* Constructor. */
        TPrivatePovError(const Base::TCodeLocation &loc, const char *msg) {
          PostCtor(loc, msg);
        }
      };  // TPrivatePovError

      /* TODO */
      class TNotifierError : public Base::TFinalError<TNotifierError> {
        public:

        /* Constructor. */
        TNotifierError(const Base::TCodeLocation &loc, const char *msg) {
          PostCtor(loc, msg);
        }
      };  // TNotifierError

      //NOTE: This is a notification about an update.
      /* TODO */
      enum TNotifierState {Failed, Succeeded, Indeterminate};

      /* Forward declarations */
      class TPrivatePovObj;

      /* TODO */
      class TTetrisHandler {
        NO_COPY_SEMANTICS(TTetrisHandler);

        enum TTask {TaskRun, TaskDrain, TaskStop};
        public:

        /* TODO */
        void Stop() {
          assert(this);

          SetTask(TaskStop);
        }

        /* TODO */
        void AddPov(TParentPov *pov) {
          assert(this);
          epoll_event event;
          Base::Zero(event);
          event.events = EPOLLIN;
          event.data.ptr = pov;
          Base::TOsError::IfLt0(
              HERE,
              epoll_ctl(EpollFD, EPOLL_CTL_ADD, pov->GetTetrisWaitHandle(), &event));
        }

        /* TODO */
        void RemovePov(TParentPov *pov) {
          assert(this);
          epoll_ctl(EpollFD, EPOLL_CTL_DEL, pov->GetTetrisWaitHandle(), 0);
        }

        /* Has the calling thread "wait" until the tetris handler has run out of updates to process. */
        void Drain() {
          assert(this);

          SetTask(TaskDrain);
        }

        private:

        /* TODO */
        TTetrisHandler() : Running(true), Task(TaskRun) {
          Base::TOsError::IfLt0(HERE, EpollFD = epoll_create1(0));
          Base::TOsError::IfLt0(HERE, pipe(Interrupt));
          HandlerThread = std::thread(std::bind(&TTetrisHandler::HandlerFunc, this));
        }

        /* TODO */
        ~TTetrisHandler() {
          if(Running) {
            Stop();
          }
          std::lock_guard<std::mutex> lock(Mutex);
          HandlerThread.join();
          close(Interrupt[0]);
          close(Interrupt[1]);
          close(EpollFD);
        }

        /* TODO */
        void Callback(std::unordered_map<TUpdate *, int> &m) {
          assert(this);
          assert(&m);
          for (auto iter = m.begin(); iter != m.end(); ++iter) {
            iter->second = 1;
          }
        }

        void SetTask(TTask task) {
          if(!Running) {
            return;
          }

          /* Lock Scope */ {
            std::lock_guard<std::mutex> lock(TaskMutex);
            Task = task;
          }
          WakeupHandler();
          Done.get_future().wait();
          Done = std::promise<void>();
        }

        /* TODO */
        void HandlerFunc() {
          assert(this);
          epoll_event event;
          Base::Zero(event);
          event.events = EPOLLIN;
          event.data.ptr = this;
          Base::TOsError::IfLt0(
              HERE,
              epoll_ctl(EpollFD, EPOLL_CTL_ADD, Interrupt[0], &event));
          TTask task = TaskRun;
          int timeout = -1;
          while(task != TaskStop) {

            /* Lock Scope */ {
              std::lock_guard<std::mutex> lock(TaskMutex);
              task = Task;
            }

            if(task == TaskStop) {
              break;
            } else if (task == TaskDrain) {
              timeout = 0;
            } else {
              timeout = -1;
            }

            std::lock_guard<std::mutex> lock(Mutex);

            epoll_event *events = new epoll_event[1];
            int ready_count;
            //Handle EINTR correctly, so that we can be debugged.
            for(;;) {
              ready_count = epoll_wait(EpollFD, events, 1, timeout);
              if(ready_count < 0) {
                if(errno == EINTR) {
                  errno = 0;
                  continue;
                }

                throw Base::TOsError(HERE);
              }
              break;
            }
            if (ready_count == 1) {
              if (events[0].data.ptr == this) {
                char c;
                read(Interrupt[0], &c, 1);
              } else {
                TParentPov *parent = reinterpret_cast<TParentPov *>(events[0].data.ptr);
                parent->PlayTetris(0);
              }
            } else {
              if(task != TaskDrain) {
                //TODO: Should this be a continue, making it so we ignore system stupidity, and don't hose flux?
                throw Base::TImpossibleError(HERE, "pthread returned a ready count of 0... This means pthread returned"
                                       "because of a timeout, but we didn't tell pthread to use a timeout...");
              }
              Done.set_value();
              /* Lock Scope */ {
                std::lock_guard<std::mutex>  lock(TaskMutex);
                Task = TaskRun;
              }
            }
          }
          Running = false;
          Done.set_value();
        }

        void WakeupHandler() {
          char temp = '\0';
          ssize_t result = write(Interrupt[1], &temp, 1);
          Base::TOsError::IfLt0(HERE, result);
          assert(result == 1);
        }

        /* TODO */
        std::thread HandlerThread;

        /* TODO */
        std::mutex Mutex;

        /* TODO */
        bool Running;

        /* TODO */
        std::mutex TaskMutex;

        /* TODO */
        TTask Task;

        std::promise<void> Done;

        /* TODO */
        int EpollFD;

        /* TODO */
        int Interrupt[2];

        friend class ::Stig::THoncho;

      };  // TTetristHandler

      /* TODO */
      template <typename TVal>
      class TStore {
        NO_COPY_SEMANTICS(TStore);
        public:

        /* TODO */
        TStore() : Running(true), Draining(false) {
          CollectorThread = std::thread(std::bind(&TStore<TVal>::Collect, this));
        }

        /* TODO */
        ~TStore() {
          assert(this);
          if (!Draining) {
            Drain();
          }
        }

        /* TODO */
        void Drain() {
          assert(this);
          {
            std::unique_lock<std::mutex> lock(Mutex);
            Running = false;
          }
          Cond.notify_all();
          CollectorThread.join();
          {
            std::unique_lock<std::mutex> lock(Mutex);
            Draining = true;
            auto first = StoreData.GetTimeCollection()->TryGetFirstMember();
            while (first) {
              delete first->GetVal();
              delete first;
              first = StoreData.GetTimeCollection()->TryGetFirstMember();
            }
          }
        }

        /* TODO */
        void AddVal(TVal *val) {
          assert(this);
          if (Running) {
            std::lock_guard<std::mutex> lock(Mutex);
            auto cache_member = Cache.GetUUIDCollection()->TryGetFirstMember(val->GetUUID());
            if (!cache_member->Decr()) {
              delete cache_member;
              std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
              time += std::chrono::duration<size_t, std::milli>(val->GetTtl() * 1000);
              new TDataMember(&StoreData, time, val->GetUUID(), val);
              Cond.notify_all();
            }
          } else {
            /* Did mommy ever put your hand on the stove? Here is that same feeling...
               We're relying on this only being entered while the lock is being held by drain.
               This works because we're in the cleanup routine of service which should be single threaded.
               BE CAREFUL! */
            auto cache_member = Cache.GetUUIDCollection()->TryGetFirstMember(val->GetUUID());
            if (!cache_member->Decr()) {
              delete cache_member;
              std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
              time += std::chrono::duration<size_t, std::milli>(val->GetTtl() * 1000);
              new TDataMember(&StoreData, time, val->GetUUID(), val);
            }
          }
        }

        /* TODO */
        void AddToCache(TVal *val) {
          assert(this);
          std::lock_guard<std::mutex> lock(Mutex);
          auto new_member = new TCacheMember(&Cache, val->GetUUID(), val);
          new_member->Incr();
        }

        /* TODO */
        typename TVal::TPtr Rendezvous(const TUUID &uuid) {
          assert(this);
          std::lock_guard<std::mutex> lock(Mutex);
          auto cache_member = Cache.GetUUIDCollection()->TryGetFirstMember(uuid);
          if (cache_member) {
            cache_member->Incr();
            return TVal::NewHandle(cache_member->GetVal());
          }
          TVal *val = 0;
          auto member = StoreData.GetUUIDCollection()->TryGetFirstMember(uuid);
          if (member) {
            val = member->GetVal();
            delete member;
          }
          auto new_member = new TCacheMember(&Cache, uuid, val);
          new_member->Incr();
          return TVal::NewHandle(val);
        }


        bool ForEach(const std::function<bool (TVal *)> &cb) const {
          assert(this);
          assert(&cb);
          assert(cb);

          //TODO //Joris can you verify this looks okay.
          std::lock_guard<std::mutex> lock(Mutex); /* TODO: this is far from ideal*/
          return StoreData.GetUUIDCollection()->ForEachUniqueMember([&cb](TDataMember *data){
            return cb(data->GetVal());
          });
        }

        private:

        /* TODO */
        class TDataMember;

        /* TODO */
        class TStoreData {
          NO_COPY_SEMANTICS(TStoreData);
          public:

          /* TODO */
          typedef InvCon::OrderedList::TCollection<TStoreData, TDataMember, std::chrono::time_point<std::chrono::system_clock>> TTimeCollection;

          /* TODO */
          typedef InvCon::UnorderedMultimap::TCollection<TStoreData, TDataMember, TUUID> TUUIDCollection;

          /* TODO */
          TStoreData() : TimeCollection(this), UUIDCollection(this) {}

          /* TODO */
          ~TStoreData() {
            assert(this);
            TimeCollection.DeleteEachMember();
            UUIDCollection.DeleteEachMember();
          }

          /* TODO */
          TTimeCollection *GetTimeCollection() const {
            assert(this);
            return &TimeCollection;
          }

          /* TODO */
          TUUIDCollection *GetUUIDCollection() const {
            assert(this);
            return &UUIDCollection;
          }

          private:

          /* TODO */
          mutable typename TTimeCollection::TImpl TimeCollection;

          /* TODO */
          mutable typename TUUIDCollection::TImpl UUIDCollection;

        };  // TStoreData

        /* TODO */
        class TDataMember {
          NO_COPY_SEMANTICS(TDataMember);
          public:

          /* TODO */
          typedef InvCon::OrderedList::TMembership<TDataMember, TStoreData, std::chrono::time_point<std::chrono::system_clock>> TTimeListMembership;

          /* TODO */
          typedef InvCon::UnorderedMultimap::TMembership<TDataMember, TStoreData, TUUID> TUUIDMembership;

          /* TODO */
          TDataMember(TStoreData *store_data, std::chrono::time_point<std::chrono::system_clock> time, const TUUID &uuid, TVal *val)
              : TimeListMembership(this, time, store_data->GetTimeCollection()), UUIDMembership(this, uuid, store_data->GetUUIDCollection()), Val(val) {}

          /* TODO */
          ~TDataMember() {
            assert(this);
            TimeListMembership.Remove();
            UUIDMembership.Remove();
          }

          /* TODO */
          const std::chrono::time_point<std::chrono::system_clock> &GetTime() const {
            assert(this);
            return TimeListMembership.GetKey();
          }

          /* TODO */
          const TUUID &GetUUID() const {
            assert(this);
            return UUIDMembership.GetKey();
          }

          /* TODO */
          TVal *GetVal() const {
            assert(this);
            return Val;
          }

          /* TODO */
          TTimeListMembership *GetTTimeListMembership() {
            assert(this);
            return &TimeListMembership;
          }

          /* TODO */
          TUUIDMembership *GetUUIDMembership() {
            assert(this);
            return &UUIDMembership;
          }

          private:

          /* TODO */
          typename TTimeListMembership::TImpl TimeListMembership;

          /* TODO */
          typename TUUIDMembership::TImpl UUIDMembership;

          /* TODO */
          TVal *Val;

        };  // TDataMember

        /* TODO */
        class TCacheMember;

        /* TODO */
        class TCache {
          NO_COPY_SEMANTICS(TCache);
          public:

          /* TODO */
          typedef InvCon::UnorderedMultimap::TCollection<TCache, TCacheMember, TUUID> TUUIDCollection;

          /* TODO */
          TCache() : UUIDCollection(this) {}

          /* TODO */
          ~TCache() {
            assert(this);
            UUIDCollection.DeleteEachMember();
          }

          /* TODO */
          TUUIDCollection *GetUUIDCollection() const {
            assert(this);
            return &UUIDCollection;
          }

          private:

          /* TODO */
          mutable typename TUUIDCollection::TImpl UUIDCollection;

        };  // TCache

        /* TODO */
        class TCacheMember {
          NO_COPY_SEMANTICS(TCacheMember);
          public:

          /* TODO */
          typedef InvCon::UnorderedMultimap::TMembership<TCacheMember, TCache, TUUID> TUUIDMembership;

          /* TODO */
          TCacheMember(TCache *cache, const TUUID &uuid, TVal *val) : UUIDMembership(this, uuid, cache->GetUUIDCollection()), Val(val), Outstanding(0) {}

          /* TODO */
          ~TCacheMember() {
            assert(this);
            UUIDMembership.Remove();
          }

          /* TODO */
          TVal *GetVal() const {
            assert(this);
            return Val;
          }

          /* TODO */
          TUUIDMembership *GetUUIDMembership() {
            assert(this);
            return &UUIDMembership;
          }

          /* TODO */
          size_t Decr() {
            assert(this);
            return --Outstanding;
          }

          /* TODO */
          void Incr() {
            assert(this);
            ++Outstanding;
          }

          private:

          /* TODO */
          typename TUUIDMembership::TImpl UUIDMembership;

          /* TODO */
          TVal *Val;

          /* TODO */
          size_t Outstanding;

        };  // TCacheMember

        /* TODO */
        void Collect() {
          assert(this);
          std::unique_lock<std::mutex> lock(Mutex);
          while (Running) {
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto first = StoreData.GetTimeCollection()->TryGetFirstMember();
            while (first) {
              if (first->GetTime() <= now) {
                delete first->GetVal();
                delete first;
              } else {
                break;
              }
              first = StoreData.GetTimeCollection()->TryGetFirstMember();
            }
            if (first) {
              Cond.wait_until(lock, first->GetTime());
            } else {
              Cond.wait(lock);
            }
          }
        }

        /* TODO */
        TStoreData StoreData;

        /* TODO */
        TCache Cache;

        /* TODO */
        std::thread CollectorThread;

        /* TODO */
        mutable std::mutex Mutex;

        /* TODO */
        std::condition_variable Cond;

        /* TODO */
        volatile bool Running;

        /* TODO */
        bool Draining;

      };  // TStore

      /* TODO */
      class TSessionObj {
        NO_COPY_SEMANTICS(TSessionObj);
        public:

        /* TODO */
        class TSessionHandle : public Base::TRefCounted {
          NO_COPY_SEMANTICS(TSessionHandle);
          public:

          /* TODO */
          typedef Base::TRefCounted::TPtr<TSessionHandle> TPtr;

          /* TODO */
          static TPtr New(const Base::TOpt<TUUID> &acct, int ttl) {
            TSessionObj *session_obj = new TSessionObj(acct, ttl);
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSessionObjStore().AddToCache(session_obj);
            return AsPtr(new TSessionHandle(session_obj));
          }

          /* TODO */
          static TPtr Rendezvous(const TUUID &uuid) {
            return Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSessionObjStore().Rendezvous(uuid);
          }

          /* TODO */
          static TPtr NewHandle(TSessionObj *session_obj) {
            if (!session_obj) {
              throw TSessionError(HERE, "No Session with the given UUID. Possibly timed out.");
            }
            return AsPtr(new TSessionHandle(session_obj));
          }

          TSessionObj *GetSessionObj() const {
            assert(this);

            return SessionObj;
          }

          /* TODO */
          const TUUID &GetUUID() const {
            assert(this);
            return SessionObj->GetUUID();
          }

          /* TODO */
          const Base::TOpt<TUUID> &GetAcct() const {
            assert(this);
            return SessionObj->GetAcct();
          }

          private:

          /* TODO */
          TSessionHandle(TSessionObj *session_obj) : SessionObj(session_obj) {
          }

          /* TODO */
          ~TSessionHandle() {
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSessionObjStore().AddVal(SessionObj);
          }

          /* TODO */
          TSessionObj *SessionObj;

        };  // TSessionHandle

        /* TODO */
        typedef TSessionHandle::TPtr TPtr;

        /* TODO */
        static TPtr NewHandle(TSessionObj *session_obj) {
          return TSessionHandle::NewHandle(session_obj);
        }

        static bool ForEachSession(const std::function<bool (TSessionObj*)> &cb);

        /* TODO */
        TSessionObj(const Base::TOpt<TUUID> &acct, int ttl) : Acct(acct), Ttl(ttl) {}

        /* TODO */
        ~TSessionObj();

        /* TODO */
        void CleanupNotifiers(const std::unordered_map<TUUID, TUUID> &notifier_map);

        /* TODO */
        const Base::TOpt<TUUID> &GetAcct() const {
          assert(this);
          return Acct;
        }

        /* TODO */
        int GetTtl() const {
          assert(this);
          return Ttl;
        }

        /* TODO */
        const TUUID &GetUUID() const {
          assert(this);
          return UUID;
        }

        /* TODO */
        void MakeNotifiers(const TPrivatePovObj* ppov, const std::unordered_set<TUUID> &notify_povs, const TUUID &update_id, std::unordered_map<TUUID, TUUID> &out);


        /* TODO */
        void OnPromote(TUUID uuid, const TPov *pov);

        /* TODO */
        void OnPovFail(TPov *pov);

        /* TODO */
        void Poll(const std::unordered_set<TUUID> &notifiers,
              Base::TOpt<std::chrono::milliseconds> timeout,
              std::unordered_map<TUUID, TNotifierState> &out);

        private:

        /* TODO */
        class TNotifier {
          NO_COPY_SEMANTICS(TNotifier);
          public:

          /* TODO */
          TNotifier(TPov *pov, TUUID update_id);

          /* TODO */
          void Fire(TNotifierState state);

          MultiEvent::TEvent::TPtr GetEvent() const;

          /* TODO */
          const TUUID &GetId() const;

          /* TODO */
          TPov *GetPov() const;

          /* TODO */
          TNotifierState GetState() const;

          /* TODO */
          const TUUID &GetUpdateId() const;

          /* TODO */
          const Base::TOpt<TNotifierState> &TryGetState() const;

          private:
          /* TODO */
          MultiEvent::TEvent::TPtr Event;

          /* TODO */
          TUUID Id;

          /* TODO */
          TPov *Pov;

          /* TODO */
          Base::TOpt<TNotifierState> State;

          /* TODO */
          TUUID UpdateId;
        };

        void RemoveNotifiers(const std::vector<const TNotifier*> &notifiers);

        /* TODO */
        TUUID UUID;

        /* TODO */
        Base::TOpt<TUUID> Acct;

        /* TODO */
        int Ttl;

        /* TODO: Eliminate the lock as much as possible. We are guaranteed to enter non-identical items. */
        /* TODO: Lock the currently in use notifiers from being watched by anyone else. Currently handled by Service. */

        /* TODO */
        /* TODO: Could/should be a read/write lock (<base/potato.h> */
        std::recursive_mutex NotifierLock;

        /* TODO */
        std::unordered_map<TUUID, TNotifier*> Notifiers; //notifier id -> notifier (Poll)

        /* TODO */
        std::unordered_set<TUUID> WaitingNotifiers; //The set of notifiers currently being waited upon.

        /* TODO */
        std::unordered_map<TUUID, std::unordered_map<const TPov*, TNotifier*>> NotifiersByUpdate; //update -> pov -> notifier (OnPromote)

        /* TODO */
        std::unordered_multimap<TPov*, TNotifier*> NotifiersByPov; //pov -> notifier (TChildPov::OnFail)

        /* TODO */
        static TStore<TSessionObj> Store;

        /* Access to the Store for drainage. */
        friend class Stig::Spa::TService;

      };  // TSessionObj

      /* TODO */
      class TSharedPovObj {
        NO_COPY_SEMANTICS(TSharedPovObj);
        public:

        /* TODO */
        class TSharedPovHandle : public Base::TRefCounted {
          NO_COPY_SEMANTICS(TSharedPovHandle);
          public:

          /* TODO */
          typedef Base::TRefCounted::TPtr<TSharedPovHandle> TPtr;

          /* TODO */
          static TPtr New(int ttl, TGlobalPov *global, const TChildPov::TOnFail &on_fail, bool paused) {
            TSharedPovObj *shared_pov_obj = new TSharedPovObj(global, ttl, on_fail, paused);
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSharedPovObjStore().AddToCache(shared_pov_obj);
            return AsPtr(new TSharedPovHandle(shared_pov_obj));
          }

          /* TODO */
          static TPtr New(const TUUID &parent, int ttl, const TChildPov::TOnFail &on_fail, bool paused) {
            TSharedPovObj *shared_pov_obj = new TSharedPovObj(parent, ttl, on_fail, paused);
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSharedPovObjStore().AddToCache(shared_pov_obj);
            return AsPtr(new TSharedPovHandle(shared_pov_obj));
          }

          /* TODO */
          static TPtr Rendezvous(const TUUID &uuid) {
            return Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSharedPovObjStore().Rendezvous(uuid);
          }

          /* TODO */
          static TPtr NewHandle(TSharedPovObj *shared_pov_obj) {
            if (!shared_pov_obj) {
              throw TSharedPovError(HERE, "No Shared Pov with the given UUID. Possibly timed out.");
            }
            return AsPtr(new TSharedPovHandle(shared_pov_obj));
          }

          /* TODO */
          TSharedPov *GetSharedPov() const {
            assert(this);
            return SharedPovObj->SharedPov;
          }

          /* TODO */
          const TUUID &GetUUID() const {
            assert(this);
            return SharedPovObj->UUID;
          }

          private:

          /* TODO */
          TSharedPovHandle(TSharedPovObj *Shared_pov_obj) : SharedPovObj(Shared_pov_obj) {}

          /* TODO */
          ~TSharedPovHandle() {
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSharedPovObjStore().AddVal(SharedPovObj);
          }

          /* TODO */
          TSharedPovObj *SharedPovObj;

        };  // TSharedPovHandle

        /* TODO */
        typedef TSharedPovHandle::TPtr TPtr;

        /* TODO */
        TSharedPovObj(TParentPov *parent, int ttl, const TChildPov::TOnFail &on_fail, bool paused)
            : SharedPov(new TSharedPov(parent, on_fail, paused)), Ttl(ttl) {
          Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetTetrisHandler().AddPov(SharedPov);
        }

        /* TODO */
        TSharedPovObj(const TUUID &parent, int ttl, const TChildPov::TOnFail &on_fail, bool paused)
            : ParentPtr(TSharedPovHandle::Rendezvous(parent)),
              SharedPov(new TSharedPov(ParentPtr->GetSharedPov(), on_fail, paused)), Ttl(ttl) {
          Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetTetrisHandler().AddPov(SharedPov);
        }

        /* TODO */
        ~TSharedPovObj() {
          assert(this);
          Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetTetrisHandler().RemovePov(SharedPov);
          delete SharedPov;
        }

        /* TODO */
        int GetTtl() const {
          assert(this);
          return Ttl;
        }

        /* TODO */
        const TUUID &GetUUID() const {
          assert(this);
          return UUID;
        }

        /* TODO */
        static TPtr NewHandle(TSharedPovObj *shared_pov_obj) {
          return TSharedPovHandle::NewHandle(shared_pov_obj);
        }

        private:

        /* TODO */
        TSharedPovHandle::TPtr ParentPtr;

        /* TODO */
        TSharedPov *SharedPov;

        /* TODO */
        TUUID UUID;

        /* TODO */
        int Ttl;

        /* TODO */
        static TStore<TSharedPovObj> Store;

        /* Access to the Store for drainage. */
        friend class Stig::Spa::TService;

      };  // TSharedPovObj

      /* TODO */
      class TPrivatePovObj {
        NO_COPY_SEMANTICS(TPrivatePovObj);
        public:

        /* TODO */
        class TPrivatePovHandle : public Base::TRefCounted {
          NO_COPY_SEMANTICS(TPrivatePovHandle);
          public:

          /* TODO */
          typedef Base::TRefCounted::TPtr<TPrivatePovHandle> TPtr;

          /* TODO */
          static TPtr New(const TUUID &session, int ttl, TGlobalPov *global, const TChildPov::TOnFail &on_fail, bool paused) {
            TPrivatePovObj *private_pov_obj = new TPrivatePovObj(session, global, ttl, on_fail, paused);
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetPrivatePovObjStore().AddToCache(private_pov_obj);
            return AsPtr(new TPrivatePovHandle(private_pov_obj));
          }

          /* TODO */
          static TPtr New(const TUUID &session, const TUUID &parent, int ttl, const TChildPov::TOnFail &on_fail, bool paused) {
            TPrivatePovObj *private_pov_obj = new TPrivatePovObj(session, parent, ttl, on_fail, paused);
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetPrivatePovObjStore().AddToCache(private_pov_obj);
            return AsPtr(new TPrivatePovHandle(private_pov_obj));
          }

          /* TODO */
          static TPtr Rendezvous(const TUUID &uuid) {
            return Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetPrivatePovObjStore().Rendezvous(uuid);
          }

          /* TODO */
          static TPtr NewHandle(TPrivatePovObj *private_pov_obj) {
            if (!private_pov_obj) {
              throw TPrivatePovError(HERE, "No Private Pov with the given UUID. Possibly timed out.");
            }
            return AsPtr(new TPrivatePovHandle(private_pov_obj));
          }

          /* TODO */
          const TSync &GetSync() const {
            assert(this);
            return PrivatePovObj->PrivatePov->GetSync();
          }

          /* TODO */
          const TUUID &GetUUID() const {
            assert(this);
            return PrivatePovObj->UUID;
          }


          TSessionObj::TSessionHandle::TPtr GetSession() const {
            assert(this);
            return PrivatePovObj->SessionPtr;
          }


          /* TODO */
          TPrivatePov *GetPrivatePov() const {
            assert(this);
            return PrivatePovObj->PrivatePov;
          }

          TPrivatePovObj *GetObj() const {
            assert(this);
            return PrivatePovObj;
          }

          private:

          /* TODO */
          TPrivatePovHandle(TPrivatePovObj *private_pov_obj) : PrivatePovObj(private_pov_obj) {
          }

          /* TODO */
          ~TPrivatePovHandle() {
            Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetPrivatePovObjStore().AddVal(PrivatePovObj);
          }

          /* TODO */
          TPrivatePovObj *PrivatePovObj;

        };  // TPrivatePovHandle

        /* TODO */
        typedef TPrivatePovHandle::TPtr TPtr;

        /* TODO */
        TPrivatePovObj(const TUUID &session, TParentPov *parent, int ttl, const TChildPov::TOnFail &on_fail, bool paused)
          : SessionPtr(TSessionObj::TSessionHandle::Rendezvous(session)),
            PrivatePov(new TPrivatePov(parent, on_fail, paused)), Ttl(ttl) {}

        /* TODO */
        TPrivatePovObj(const TUUID &session, const TUUID &parent, int ttl, const TChildPov::TOnFail &on_fail, bool paused)
          : ParentPtr(TSharedPovObj::TSharedPovHandle::Rendezvous(parent)),
            SessionPtr(TSessionObj::TSessionHandle::Rendezvous(session)),
            PrivatePov(new TPrivatePov(ParentPtr->GetSharedPov(), on_fail, paused)), Ttl(ttl) {}

        /* TODO */
        ~TPrivatePovObj() {
          assert(this);
          delete PrivatePov;
        }

        /* TODO */
        int GetTtl() const {
          assert(this);
          return Ttl;
        }

        /* TODO */
        const TUUID &GetUUID() const {
          assert(this);
          return UUID;
        }

        /* TODO */
        static TPtr NewHandle(TPrivatePovObj *private_pov_obj) {
          return TPrivatePovHandle::NewHandle(private_pov_obj);
        }

        TPrivatePov *GetNative() const {
          assert(this);
          return PrivatePov;
        }

        private:

        /* TODO */
        TSharedPovObj::TSharedPovHandle::TPtr ParentPtr;

        /* TODO */
        TSessionObj::TSessionHandle::TPtr SessionPtr;

        /* TODO */
        TPrivatePov *PrivatePov;

        /* TODO */
        TUUID UUID;

        /* TODO */
        int Ttl;

        /* TODO */
        static TStore<TPrivatePovObj> Store;

        /* Access to the Store for drainage. */
        friend class Stig::Spa::TService;

      };  // TPrivatePovObj

    }  // FluxCapacitor

  }  // Spa

}  // Stig