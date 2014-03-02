/* <stig/indy/disk/util/cache.test.cc>

   Unit test for <stig/indy/disk/util/cache.h>. */

#include <stig/indy/disk/util/cache.h>

#include <chrono>
#include <thread>

#include <sched.h>

#include <base/timer.h>
#include <stig/indy/disk/sim/mem_engine.h>

#include <test/kit.h>

using namespace std;
using namespace std::chrono;
using namespace Base;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;

FIXTURE(Typical) {
  const size_t cache_size = 4096;
  const size_t num_lru = 4UL;
  const size_t page_id = 4UL;
  const TScheduler::TPolicy scheduler_policy(4, 10, milliseconds(10));
  TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  Sim::TMemEngine mem_engine(&scheduler,
                             64 /* disk space: 64 MB */,
                             16,
                             4096 /* page cache slots: 1GB */,
                             1 /* num page lru */,
                             16 /* block cache slots: 1GB */,
                             1 /* num block lru */);
  TCache<4096> cache(mem_engine.GetVolMan(), cache_size, num_lru);
  TCache<4096>::TSlot *data_slot;
  TCache<4096>::TSlot *slot = cache.Get(page_id, data_slot);
  cache.Release(slot, page_id);
}

FIXTURE(SingleThreadPressureLRU) {
  const size_t cache_size = 4096;
  const size_t num_lru = 4UL;
  const TScheduler::TPolicy scheduler_policy(4, 10, milliseconds(10));
  TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  Sim::TMemEngine mem_engine(&scheduler,
                             64 /* disk space: 64 MB */,
                             16,
                             4096 /* page cache slots: 1GB */,
                             1 /* num page lru */,
                             16 /* block cache slots: 1GB */,
                             1 /* num block lru */);
  TCache<4096> cache(mem_engine.GetVolMan(), cache_size, num_lru);
  for (size_t i = 0; i < cache_size * 24; ++i ) {
    const size_t page_id = i * 3UL;
    TCache<4096>::TSlot *data_slot;
    TCache<4096>::TSlot *slot = cache.Get(page_id, data_slot);
    cache.Release(slot, page_id);
  }
}
