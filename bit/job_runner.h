/* Manages a pool of threads to run jobs. */

#include <atomic>
#include <thread>

#include <base/class_traits.h>
#include <bit/job.h>

namespace Bit {

class TJob;

struct TJobRunner {
  struct TResult {
    MOVE_ONLY(TResult)
    TResult(TJob *job, TJob::TOutput &&output);

    TJob *Job;
    const TOutput Output;
  };

  TJobRunner(uint64_t worker_count);
  ~TJobRunner();

  bool IsReady() const;

  /* Adds a job to the queue of jobs to run. */
  void Queue(TJob *job);

  /* Returns true iff the job runner is guaranteed to have more results in a future WaitForResults
   * call. */
  bool HasMoreResults() const;

  /* Grab a single result. */
  TResult WaitForResult();

  private:
  void ProcessQueue();
  void Shutdown();

  // Jobs in, results out.
  moodycamel::BlockingConcurrentQueue<TRunnable> ToRun;

  // TODO(cmaloney): The pointer indirection here is fugly.
  moodycamel::BlockingConcurrentQueue<std::unique_ptr<TResult>> Results;

  // Sits in the background and pumps io from subprocesses into this process.
  Base::TPump Pump;

  std::atomic<uint64_t> InQueue = 0;
  std::atomic<uint64_t> BeingRun = 0;

  // General parameters for the runner
  bool ExitWorker = false;
  const bool PrintCmd;

  // Background job runners
  std::vector<std::unique_ptr<std::thread>> JobRunners;
};
}
