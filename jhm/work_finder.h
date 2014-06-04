/* <jhm/work_finder.h>

   Finds and then runs all the work needed to get out all needed files.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include <base/fd.h>
#include <base/subprocess.h>

namespace Jhm {

  class TJob;
  class TFile;

  // TODO: This class' fundamental 'Do data associated subprocesses up to n at a time' should be extracted.
  class TJobRunner {
    NO_MOVE(TJobRunner);
    NO_COPY(TJobRunner);
    public:
    class TResult {
      public:
      MOVE_ONLY(TResult);
      TResult(TJob *job, int exit_code, Base::TFd &&stdout, Base::TFd &&stderr)
          : ExitCode(exit_code), Job(job), Stdout(stdout), Stderr(stderr) {}

      int ExitCode;
      TJob *Job;
      Base::TFd Stdout, Stderr;
    };

    using TResults = std::vector<TResult>;

    TJobRunner(uint32_t worker_count, bool print_cmd);
    ~TJobRunner();

    void Queue(TJob *job);

    TResults WaitForResults();

    /* Returns true IFF there are no more jobs running, and none of the jobs run exited with a non-zero exit code */
    uint64_t GetNumberJobsFailed() const {
      return NumberJobsFailed;
    }

    bool IsWorking() const {
      return Working;
    }

    private:

    void Worker();

    std::atomic<bool> Working;
    bool ExitWorker = false;
    uint64_t NumberJobsFailed = 0;
    const bool PrintCmd;
    const uint32_t WorkerCount;
    TResults Results;
    std::mutex ResultsMutex;
    std::condition_variable NewResults;
    std::mutex NewResultsMutex;
    std::queue<TJob *> ToRun;
    mutable std::mutex ToRunMutex;
    std::thread QueueRunner;
    std::mutex NewWorkMutex;
    std::condition_variable NewWork;
    Base::TPump pump;
  };

  /* Finds all the currently buildable leaves and queues them for the subproc_runner to fire off. */
  class TWorkFinder {
    NO_COPY(TWorkFinder);
    NO_MOVE(TWorkFinder);

    public:
    TWorkFinder(uint32_t worker_count,
                bool print_cmd,
                std::function<std::unordered_set<TJob *>(TFile *)> &&get_jobs_producing_file)
        : GetJobsProducingFile(std::move(get_jobs_producing_file)), Runner(worker_count, print_cmd) {}

    /* Adds the jobs needed to produce the file. Returns true if there is work to be done for the file to exist.
       Adds to the ToFinish multimap of the given job if job is specified. */
    bool AddNeededFile(TFile *file, TJob *job = nullptr);

    bool FinishAll();

    private:
    bool IsBuildable(TFile *file);
    bool IsDone(TJob *job) const;

    void ProcessReady();
    void ProcessResult(TJobRunner::TResult &result);

    bool Queue(TJob *job);

    /* Returns the producer for a file.
        if file.IsSrc() -> nullptr
        if file is unproducable -> nullptr
        if file is found to have multiple producers -> throw

        NOTE: A job which we never try to get the producer explicitly for can have multiple producers just fine. */
    TJob *TryGetProducer(TFile *file);

    // List of jobs which are ready to go.
    // TODO: Should really be a hyper-efficient work queue for n workers to pull out of in parallel.
    std::queue<TJob *> Ready;

    // Map from producer to consumer job. When the last instance of a consumer job is removed from the multimap, it's
    // time to
    // check for more dependencies.
    // TODO: This should really be some sort of min-heap tree thing.
    std::unordered_multimap<TJob *, TJob *> ToFinish;

    // Keeps track of the full set of jobs which have ever been queued to run.
    std::unordered_set<TJob *> All, Running, Finished;

    std::unordered_map<TJob *, uint64_t> Waiting;

    // Set of output files with jobs which can theoretically be reached via some arbitrary chain of other jobs from some
    // input file.
    std::unordered_map<TFile *, TJob *> Producers;


    std::function<std::unordered_set<TJob*> (TFile *)> GetJobsProducingFile;

    // Runs the work queuPops things off work queue
    // TODO: runs itself in a seperate process group (setpgid) so it can easily wait for all child processes
    // TODO: Forcefully deletes all bad output
    // Launch the IO pump, wait/manage all the subprocesses, and notify the work finder / env when things are completed.
    // Calls a standardized callback on completion of each command.
    TJobRunner Runner;
  };
}