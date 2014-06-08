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

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <jhm/job_runner.h>

namespace Jhm {

  class TJob;
  class TFile;

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

    /* Runs jobs to make all the needed files, queuing more jobs as needed. Returns true on success*/
    bool FinishAll();

    bool IsBuildable(TFile *file);
    bool IsFileDone(TFile *file);

    private:
    /* Writes a single-line status message. */
    void WriteStatusLine() const;

    bool IsDone(TJob *job) const;

    void ProcessReady();

    /* Process the given result. Returns true if the result of the job is failure / we're shutting down now. */
    bool ProcessResult(TJobRunner::TResult &result);

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