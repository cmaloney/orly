/* <stig/indy/disk/util/corruption_detector.test.cc>

   Unit test for <stig/indy/disk/util/corruption_detector.h>.

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

#include <stig/indy/disk/util/corruption_detector.h>

#include <random>

#include <string.h>

#include <base/scheduler.h>
#include <base/timer.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Indy::Disk::Util;

FIXTURE(Typical) {
  const size_t offset = 512;
  size_t *data = nullptr;
  size_t *data_cpy = nullptr;
  const size_t data_size = getpagesize();
  const size_t user_data_per_block = (data_size - sizeof(size_t)) / sizeof(size_t);
  Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&data), getpagesize(), std::max(static_cast<int>(data_size), getpagesize())));
  Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&data_cpy), getpagesize(), std::max(static_cast<int>(data_size), getpagesize())));
  std::mt19937_64 engine;
  for (size_t i = 0; i < user_data_per_block; ++i) {
    data[i] = engine();
  }
  data[user_data_per_block] = offset;
  memcpy(data_cpy, data, data_size);
  TCorruptionDetector::WriteMurmur(data, data_size, offset);
  TCorruptionDetector::TryReadMurmur(data, data_size, offset);
  EXPECT_EQ(memcmp(data, data_cpy, data_size - sizeof(size_t)), 0);
  free(data_cpy);
  free(data);
}

FIXTURE(ManyRandom) {
  #ifndef NDEBUG
  const size_t num_iter = 5000UL;
  #else
  const size_t num_iter = 500000UL;
  #endif
  size_t passed = 0UL;
  size_t *data = nullptr;
  size_t *data_cpy = nullptr;
  const size_t data_size = getpagesize();
  const size_t user_data_per_block = (data_size - sizeof(size_t)) / sizeof(size_t);
  Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&data), getpagesize(), std::max(static_cast<int>(data_size), getpagesize())));
  Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&data_cpy), getpagesize(), std::max(static_cast<int>(data_size), getpagesize())));
  std::mt19937_64 engine;
  for (size_t iter = 0; iter < num_iter; ++iter) {
    const size_t offset = iter * 512;
    for (size_t i = 0; i < user_data_per_block; ++i) {
      data[i] = engine();
    }
    data[user_data_per_block] = offset;
    memcpy(data_cpy, data, data_size);
    TCorruptionDetector::WriteMurmur(data, data_size, offset);
    if (TCorruptionDetector::TryReadMurmur(data, data_size, offset)) {
      if (memcmp(data, data_cpy, data_size - sizeof(size_t)) == 0) {
        ++passed;
      }
    }
  }
  EXPECT_EQ(passed, num_iter);
  free(data_cpy);
  free(data);
}

void IntroduceRandomBitError(size_t *buf, size_t data_size) {
  size_t bit = rand() % (data_size * 8);
  size_t mask = 1 << bit % (sizeof(size_t) * 8);
  buf[bit / (sizeof(size_t) * 8)] ^= mask;
}

void IntroduceRandomBitError(size_t *buf, size_t data_size, std::mt19937_64 &engine) {
  size_t bit = engine() % (data_size * 8);
  size_t mask = 1 << bit % (sizeof(size_t) * 8);
  buf[bit / (sizeof(size_t) * 8)] ^= mask;
}

FIXTURE(ParallelErrorChecking) {
  #ifndef NDEBUG
  const size_t num_per_cpu = 5000UL;
  #else
  const size_t num_per_cpu = 500000UL;
  #endif
  int num_cpu = sysconf(_SC_NPROCESSORS_ONLN);
  const TScheduler::TPolicy job_policy(num_cpu, num_cpu, milliseconds(10));
  TScheduler job_scheduler(job_policy);
  int completed = 0UL;
  std::mutex mut;
  std::condition_variable cond;
  size_t total_passed = 0UL;
  size_t total_errored = 0UL;
  double rtt = 0.0;
  auto runner = [&mut, &cond, &completed, &total_passed, &total_errored, &rtt](size_t seed, size_t num_iter) {
    size_t passed = 0UL;
    size_t errored = 0UL;
    std::mt19937_64 engine;
    engine.seed(seed);
    size_t *data = nullptr;
    size_t *data_cpy = nullptr;
    const size_t data_size = getpagesize();
    const size_t user_data_per_block = (data_size - sizeof(size_t)) / sizeof(size_t);
    Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&data), getpagesize(), std::max(static_cast<int>(data_size), getpagesize())));
    Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&data_cpy), getpagesize(), std::max(static_cast<int>(data_size), getpagesize())));
    for (size_t iter = 0; iter < num_iter; ++iter) {
      const size_t offset = iter * 512;
      for (size_t i = 0; i < user_data_per_block; ++i) {
        data[i] = engine();
      }
      data[user_data_per_block] = offset;
      memcpy(data_cpy, data, data_size);
      TCorruptionDetector::WriteMurmur(data, data_size, offset);
      IntroduceRandomBitError(data, data_size, engine);
      IntroduceRandomBitError(data, data_size, engine);
      IntroduceRandomBitError(data, data_size, engine);
      if (TCorruptionDetector::TryReadMurmur(data, data_size, offset)) {
        if (memcmp(data, data_cpy, data_size - sizeof(size_t)) == 0) {
          ++passed;
        }
      } else {
        ++errored;
      }
    }
    free(data);
    free(data_cpy);
    std::lock_guard<std::mutex> lock(mut);
    ++completed;
    total_passed += passed;
    total_errored += errored;
    cond.notify_one();
  };
  for (int i = 0; i < num_cpu; ++i) {
    job_scheduler.Schedule(std::bind(runner, i, num_per_cpu));
  }
  std::unique_lock<std::mutex> lock(mut);
  while (completed != num_cpu) {
    cond.wait(lock);
  }
  const size_t num_iter = num_per_cpu * num_cpu;
  EXPECT_EQ(total_passed, 0UL);
  EXPECT_EQ(total_errored, num_iter);
}