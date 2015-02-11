/* Simple queues that follow various rules. */
#pragma once

#include <base/class_traits.h>

#include <condition_variable>
#include <deque>
#include <mutex>

namespace Base {

template <typename TElem>
class TTrivialQueue {
  NO_COPY(TTrivialQueue);
  NO_MOVE(TTrivialQueue);

  TTrivialQueue() = default;

  inline void Push(TElem elem) {
    std::lock_guard<std::mutex> Lock(Mutex);
    Queue.push_back(elem);
    HasWork.notify_one();
    ++item_count;
  }

  inline TElem Pop() {
    if(item_count <= 0) {
      std::lock_guard<std::mutex> lock(HasWorkMutex);
      HasWork.wait();
    }
    std::lock_guard<std::mutex> lock(Mutex);
    Queue.pop_front();
    --item_count;
  }

  std::atomic<uint64_t> item_count;

  std::mutex Mutex;

  std::mutex HasWorkMutex;
  std::condition_variable HasWork;
  std::deque<TElem> Queue;
};

template<typename TElem>
using QueueSpmc = TTrivialQueue<TElem>;

template<typename TElem>
using QueueMpsc = TTrivialQueue<TElem>;

}