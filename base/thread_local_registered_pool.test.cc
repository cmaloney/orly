/* <base/thread_local_registered_pool.test.cc>

   Unit test for <base/thread_local_registered_pool.h>

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

#include <base/thread_local_registered_pool.h>

#include <condition_variable>
#include <thread>

#include <test/kit.h>

using namespace std;
using namespace Base;

class TObj;

class TObj
    : public TThreadLocalPoolManager<TObj>::TObjBase {
  NO_COPY_SEMANTICS(TObj);
  public:

  TObj() {}

  ~TObj() {}

  bool AssertCanFree() const {
    return true;
  }

};

static __thread TThreadLocalPoolManager<TObj>::TThreadLocalRegisteredPool *LocalPool = nullptr;

FIXTURE(Typical) {
  TThreadLocalPoolManager<TObj> manager;
  thread t1([&](){
    const size_t pool_size = 20UL;
    EXPECT_FALSE(LocalPool);
    LocalPool = new TThreadLocalPoolManager<TObj>::TThreadLocalRegisteredPool(&manager, pool_size);
    try {
      EXPECT_TRUE(LocalPool);
      TObj *obj = LocalPool->Alloc();
      EXPECT_TRUE(obj);
    } catch (...) {
      delete LocalPool;
      LocalPool = nullptr;
      throw;
    }
    delete LocalPool;
    LocalPool = nullptr;
  });
  t1.join();
}

FIXTURE(BadAlloc) {
  TThreadLocalPoolManager<TObj> manager;
  bool caught_bad_alloc = false;
  thread t1([&](){
    const size_t pool_size = 20UL;
    EXPECT_FALSE(LocalPool);
    LocalPool = new TThreadLocalPoolManager<TObj>::TThreadLocalRegisteredPool(&manager, pool_size);
    try {
      EXPECT_TRUE(LocalPool);
      for (size_t i = 0; i < pool_size; ++i) {
        TObj *obj = LocalPool->Alloc();
        EXPECT_TRUE(obj);
      }
      try {
        LocalPool->Alloc();
        assert(false); /* We should be throwing a bad_alloc error */
      } catch (const std::bad_alloc &) {
        caught_bad_alloc = true;
      }
    } catch (...) {
      delete LocalPool;
      LocalPool = nullptr;
      throw;
    }
    delete LocalPool;
    LocalPool = nullptr;
  });
  t1.join();
  EXPECT_TRUE(caught_bad_alloc);
}

FIXTURE(SwapInFreePool) {
  TThreadLocalPoolManager<TObj> manager;
  thread t1([&](){
    const size_t pool_size = 20UL;
    EXPECT_FALSE(LocalPool);
    LocalPool = new TThreadLocalPoolManager<TObj>::TThreadLocalRegisteredPool(&manager, pool_size);
    try {
      EXPECT_TRUE(LocalPool);
      for (size_t i = 0; i < pool_size * 10UL; ++i) {
        TObj *obj = LocalPool->Alloc();
        EXPECT_TRUE(obj);
        LocalPool->Free(obj);
      }
    } catch (...) {
      delete LocalPool;
      LocalPool = nullptr;
      throw;
    }
    delete LocalPool;
    LocalPool = nullptr;
  });
  t1.join();
}

FIXTURE(Borrowing) {
  TThreadLocalPoolManager<TObj> manager;
  const size_t pool_size = 5UL;
  std::mutex mut;
  std::condition_variable cond;
  size_t step = 0UL;
  thread t1([&](){
    EXPECT_FALSE(LocalPool);
    LocalPool = new TThreadLocalPoolManager<TObj>::TThreadLocalRegisteredPool(&manager, pool_size);
    try {
      EXPECT_TRUE(LocalPool);
      std::vector<TObj *> my_obj_vec;
      TObj *obj = LocalPool->Alloc();
      EXPECT_TRUE(obj);
      LocalPool->Free(obj);
      for (size_t i = 0; i < pool_size - 1; ++i) {
        TObj *obj = LocalPool->Alloc();
        EXPECT_TRUE(obj);
        my_obj_vec.emplace_back(obj);
      }
      /* next phase */ {
        std::lock_guard<std::mutex> lock(mut);
        step = 1UL;
        cond.notify_one();
      }
      /* wait for next phase */ {
        std::unique_lock<std::mutex> lock(mut);
        while (step != 2) {
          cond.wait(lock);
        }
      }
      for (auto obj : my_obj_vec) {
        LocalPool->Free(obj);
      }
    } catch (...) {
      delete LocalPool;
      LocalPool = nullptr;
      throw;
    }
    delete LocalPool;
    LocalPool = nullptr;
  });
  thread t2([&](){
    EXPECT_FALSE(LocalPool);
    LocalPool = new TThreadLocalPoolManager<TObj>::TThreadLocalRegisteredPool(&manager, pool_size);
    try {
      /* wait for next phase */ {
        std::unique_lock<std::mutex> lock(mut);
        while (step != 1) {
          cond.wait(lock);
        }
      }
      EXPECT_TRUE(LocalPool);
      std::vector<TObj *> my_obj_vec;
      for (size_t i = 0; i < pool_size + 1; ++i) {
        TObj *obj = LocalPool->Alloc();
        EXPECT_TRUE(obj);
        my_obj_vec.emplace_back(obj);
      }
      /* next phase */ {
        std::lock_guard<std::mutex> lock(mut);
        step = 2UL;
        cond.notify_one();
      }
      for (auto obj : my_obj_vec) {
        LocalPool->Free(obj);
      }
    } catch (...) {
      delete LocalPool;
      LocalPool = nullptr;
      throw;
    }
    delete LocalPool;
    LocalPool = nullptr;
  });
  t1.join();
  t2.join();
}