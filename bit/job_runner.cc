#include <bit/job_runner.h>

using namespace Base;
using namespace Bit;
using namespace std;

TJobRunner::TJobRunner(uint64_t worker_count) : BeingRun(0), ExitWorker(false) {
  JobRunners.reserve(worker_count);
  for(uint64_t i = 0; i < worker_count; ++i) {
    JobRunners.push_back(
        make_unique<thread>(bind(&TJobRunner::ProcessQueue, this)));
  }
}

TJobRunner::~TJobRunner() {
  Shutdown();
  for(auto &thread_ptr : JobRunners) {
    thread_ptr->join();
    thread_ptr.reset();
  }
}

bool TJobRunner::IsReady() const { return !ExitWorker; }

void TJobRunner::Queue(TJob *job) {
  // We use null jobs to indicate "end of queue" and terminate processing.
  // So passing in a null job is really bad.
  assert(job);

  // Note: We call GetCmd() to make the string here because if we done
  // in the QueueRunner we'd cross data ownership in threads.
  ++InQueue;
  ToRun.enqueue(job);
}

bool TJobRunner::HasMoreResults() const { return (!ExitWorker ? InQueue : uint64_t(BeingRun)) > 0; }

TJobRunner::TResult TJobRunner::WaitForResult() {
  assert(HasMoreResults());

  std::unique_ptr<TResult> result_ptr;
  Results.wait_dequeue(result_ptr);
  --BeingRun;
  --InQueue;

  TResult result = std::move(*result_ptr.get());

  return result;
}

void TJobRunner::ProcessQueue() {
  // Pop a single job off the ToRun queue, run it, repeat ad-nauesum.
  // if
  TJob *job = nullptr;
  for(;;) {
    ToRun.wait_dequeue(job);

    // nullptr job indicates it is time to exit.
    if(!job) {
      break;
    }

    // The job is now being run and will definitely produce a result.
    ++BeingRun;

    // Run the job.
    // TODO(cmaloney): Capture exceptions and report back in results if one
    // occurs rather than letting it stop everything.
    TJob::TOutput output = job->Run();
    bool has_error = (output.Result == TJob::TOutput::Error);
    Results.enqueue(std::make_unique<TResult>(job, std::move(output)));

    if (has_error) {
      Shutdown();
    }
  }
}

void TJobRunner::Shutdown() {
  ExitWorker = true;
  // Push one null job per worker onto the queue so that all the processors will
  // definitely see them.
  // If this is entered by multiple threads simultaneously, excess jobs will be
  // pushed but a non-empty queue at shutdown isn't an error, so that is fine.
  for(uint64_t i = 0; i < JobRunners.size(); ++i) {
    ToRun.enqueue(nullptr);
  }
}
