#include <bit/job_runner.h>

#include <bit/file_info.h>
#include <util/path.h>

using namespace Base;
using namespace Bit;
using namespace std;

TJobRunner::TResult::TResult(TJob *job, std::unique_ptr<TJob::TOutput> &&output)
    : Job(job), Output(std::move(output)) {
  assert(job);
  assert(Output);
}

TJobRunner::TJobRunner(uint64_t worker_count, TFileEnvironment *file_environment)
    : FileEnvironment(file_environment), BeingRun(0), ExitWorker(false) {
  JobRunners.reserve(worker_count);
  for (uint64_t i = 0; i < worker_count; ++i) {
    JobRunners.push_back(make_unique<thread>(bind(&TJobRunner::ProcessQueue, this)));
  }
}

TJobRunner::~TJobRunner() {
  Shutdown();
  for (auto &thread_ptr : JobRunners) {
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

  TResult result;
  Results.wait_dequeue(result);
  --BeingRun;
  --InQueue;

  return result;
}

#include <base/as_str.h>
#include <iostream>

void TJobRunner::ProcessQueue() {
  // Pop a single job off the ToRun queue, run it, repeat ad-nauesum.
  // if
  TJob *job = nullptr;
  for (;;) {
    ToRun.wait_dequeue(job);

    // nullptr job indicates it is time to exit.
    if (!job) {
      break;
    }

    // The job is now being run and will definitely produce a result.
    ++BeingRun;

    // Make sure the output directory exists.
    // TODO(cmaloney): This does a lot of stat calls and string manipulation...
    // Make sure we only call it once per RelPath directory.
    for(const TFileInfo *file: job->GetOutput()) {
      Util::EnsureDirExists(file->CmdPath.c_str(), true);
    }

    // Run the job.
    // TODO(cmaloney): Capture exceptions and report back in results if one
    // occurs rather than letting it stop everything.
    TResult result(job, std::make_unique<TJob::TOutput>(job->Run(FileEnvironment)));
    bool has_error = (result.Output->Result == TJob::TOutput::Error);
    Results.enqueue(std::move(result));

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
  for (uint64_t i = 0; i < JobRunners.size(); ++i) {
    ToRun.enqueue(nullptr);
  }
}
