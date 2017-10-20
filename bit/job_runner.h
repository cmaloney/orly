/* Manages a pool of threads to run jobs. */

#include <atomic>
#include <thread>

#include <base/class_traits.h>
#include <bit/job.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#include <concurrentqueue/blockingconcurrentqueue.h>
#pragma clang diagnostic pop

namespace Bit {

// Runs jobs. Automatically shuts down running when a job has errored, and
// cannot be resumed.
// NOTE: Could be a template on the queue input + output, but that just forces
// everything to be in the header and it isn't currently used generically, so
// that is overhead not producing value.
struct TJobRunner {
  struct TResult {
    MOVE_ONLY(TResult)
    TResult() = default;
    TResult(TJob *job, std::unique_ptr<TJob::TOutput> &&output);

    TJob *Job = nullptr;
    std::unique_ptr<TJob::TOutput> Output;
  };

  TJobRunner(uint64_t worker_count, TFileEnvironment *file_environment);
  ~TJobRunner();

  bool IsReady() const;

  /* Adds a job to the queue of jobs to run. */
  void Queue(TJob *job);

  /* Returns true iff the job runner is guaranteed to have more results in a future WaitForResults
   * call. */
  bool HasMoreResults() const;

  void Shutdown();

  /* Grab a single result. */
  TResult WaitForResult();

  private:
  void ProcessQueue();

  TFileEnvironment *const FileEnvironment;

  // Jobs in, results out.
  moodycamel::BlockingConcurrentQueue<TJob*> ToRun;

  // TODO(cmaloney): The pointer indirection here is fugly.
  moodycamel::BlockingConcurrentQueue<TResult> Results;

  uint64_t InQueue = 0;
  std::atomic<uint64_t> BeingRun;

  // General parameters for the runner
  std::atomic<bool> ExitWorker;

  // Background job runners
  std::vector<std::unique_ptr<std::thread>> JobRunners;
};
}
