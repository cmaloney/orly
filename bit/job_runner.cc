#include <bit/job_runner.h>

using namespace Base;
using namespace Bit;
using namespace std;

TJobRunner::TJobRunner(uint64_t worker_count) : BeingRun(0) {
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
  ToRun.enqueue(make_tuple(job, job->GetCmd()));
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
  TRunnable runnable;
  for(;;) {
    ToRun.wait_dequeue(runnable);

    // nullptr job indicates it is time to exit.
    if(!get<0>(runnable)) {
      assert(get<1>(runnable).size() == 0);
      break;
    }

    // The job is now being run and will definitely produce a result.
    ++BeingRun;

    // Run the job.
    const auto &cmd = get<1>(runnable);
    if(PrintCmd) {
      // NOTE: We use '+' to make a new string (effectively as a back buffer), then a single
      // operation to write it out
      // This makes it so that the line never gets broken / split / etc. because of
      // threading.
      // TODO: Join in a way that makes the difference between ' ' and passing the arguments
      // as an array more obvious.
      cout << AsStr(Join(cmd, ' ')) + '\n';
    }

    auto subprocess = TSubprocess::New(Pump, cmd);
    int returncode = subprocess->Wait();
    Results.enqueue(std::make_unique<TResult>(get<0>(runnable), returncode, subprocess->TakeStdOutFromChild(),
                            subprocess->TakeStdErrFromChild()));
    // If the return code is non-zero exit all the workers
    if(returncode != 0) {
      Shutdown();
      break;
    }
  }
}

void TJobRunner::Shutdown() {
  ExitWorker = true;
  // Push one null job per worker onto the queue so that all the processors will
  // definitely see them.
  const auto null_job = make_tuple(nullptr, vector<string>());
  for(uint64_t i = 0; i < JobRunners.size(); ++i) {
    ToRun.enqueue(null_job);
  }
}
