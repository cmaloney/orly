/* Fixed size worker pool that processes work from a multi-consumer queue. */

#include <atomic>
#include <type_traits>

#include <base/queue.h>

namespace Base {

template <typename TTask>
class TWorkerPool {
  NO_COPY(TWorkerPool);
  NO_MOVE(TWorkerPool);
public:
  TWorkerPool(uint64_t count);

  using TResult = std::result_of_t<TTask()>;

  // Able to process tasksw
  bool IsReady() const {
    return ToProcess && !WorkersExited;
  }

  // Queue a task.
  void Queue(TTask task);

  // TODO(cmaloney): Demonstrate popping in batches is more efficient.
  std::vector<TResult> Pop();

  // Returns true iff a call to Pop is guaranteed not to block forever.
  bool HasMoreResults() const;

private:
  Base::QueueSpmc<TTask> Tasks;
  Base::QueueMpsc<TResult> Results;

  // Count of items left to process.
  std::atomic<uint64_t> ToProcess;
  std::atomic<uint64_t> ResultsAvailable;

  // Indicates the queue should shutdown.
  std::atomic<bool> Shutdown;

  std::atomic<bool> WorkersExited;

};

}  // namespace Base