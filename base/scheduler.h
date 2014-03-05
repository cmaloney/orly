/* <base/scheduler.h>

   The scheduler runs jobs for you on a background thread.  It manages a pool of worker threads which dynamically grows and shrinks.
   The shceduler can also perform a clean shutdown, blocking new jobs from starting while waiting for existing jobs to finish.
   It can also perform a forced shutdown, interrupting existing jobs and bringing all work to a halt quickly.

   Each job run by the scheduler must remain interruptable by signal.  This means you must not block indefinitely on things other than
   file descriptors (such as condition variables) and you must not mask out the signal used by the scheduler to cause interrupts.
   (See TScheduler::Signum.)  If you go deaf to signals for any reason, you will become uninterruptable and could cause a shutdown
   to hang.  The worker threads also mask out all signals except for the one used by the scheduler, so you shouldn't expect to hear any
   other signals.

   This module installs a handler for its signal during data segment initialization.  The handler is required to prevent the signal
   from shutting down the process.  Don't remove this handler.  It you do, the process is in danger of being shut down when a scheduler
   shuts down.

   It's ok for a job to create its own scheduler and manange a sub-pool of threads.  This will create a threading model that is tree-
   shaped.  The threads in the tree will shutdown from the leaves to the root.  It's also ok for a process to use a single scheduler
   for all its needs.  This creates a single tier of threads all of which attempt to shut down simultaneously.

   If a job throws an exception, the worker will log it as an error but keep running.

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

#pragma once

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <base/shutting_down.h>
#include <signal/handler_installer.h>
#include <signal/set.h>

namespace Base {

  /* A scheduler of background jobs. */
  class TScheduler {
    NO_COPY_SEMANTICS(TScheduler);
    public:

    /* A function to be scheduled. */
    using TJob = std::function<void ()>;

    /* A function to be scheduled by RunUntilCtrlC(). */
    using TMainJob = std::function<void (TScheduler *)>;

    /* Destribes a set of rules for managing workers. */
    class TPolicy {
      public:

      /* We characterize timeouts in milliseconds. */
      using TTimeout = std::chrono::milliseconds;

      /* Minimum and maximum worker count and idle timeout all set to zero. */
      TPolicy();

      /* Minimum and maximum worker count and idle timeout all set to the given values.
         If real-time is true, then the scheduler will run its threads using the SCHED_FIFO policy and a a priority one
         less than the system maximum.  Note that the process must be running with superuser privileges for this to be allowed. */
      TPolicy(size_t min_worker_count, size_t max_worker_count, const TTimeout &idle_timeout, bool is_real_time = false);

      /* The amount of time a worker will wait before self-destructing. */
      const TTimeout &GetIdleTimeout() const {
        assert(this);
        return IdleTimeout;
      }

      /* The maximum number of workers. */
      size_t GetMaxWorkerCount() const {
        assert(this);
        return MaxWorkerCount;
      }

      /* The minimum number of workers. */
      size_t GetMinWorkerCount() const {
        assert(this);
        return MinWorkerCount;
      }

      /* True if we're going to run threads with real-time priority. */
      bool IsRealTime() const {
        assert(this);
        return RealTime;
      }

      /* Starts a scheduler with this policy, then schedules the given job, passing it the scheduler.
         This thread will then block until it receives ctrl-c, then it will shut down the scheduler
         and return.  It's common to call this function from main(). */
      void RunUntilCtrlC(TMainJob &&main_job) const;

      /* The policy of no workers. */
      static const TPolicy Shutdown;

      private:

      /* See accessors. */
      size_t MinWorkerCount, MaxWorkerCount;

      /* See accessor. */
      TTimeout IdleTimeout;

      /* See accessor. */
      bool RealTime;

    };  // TScheduler::TPolicy

    /* Start with the minimum number of workers required by the policy. */
    TScheduler(const TPolicy &policy = TPolicy::Shutdown);

    /* Does a Shutdown(0) when it goes. */
    ~TScheduler();

    /* A copy of our current policy.  This function is thread-safe. */
    TPolicy GetPolicy() const;

    /* Set the minimum and maximum number of workers and the idle timeout.
       If the number of workers is currently less than the new minimum, new workers will start.
       If the number of workers is currently greaterthan the new maximum, existing workers will shut down when they are
       done with their current jobs.  This function is thread-safe. */
    void SetPolicy(const TPolicy &policy);

    /* Pushes the job onto the queue of jobs waiting to be performed.  Higher priority jobs go first.
       If there is an idle worker, it will be reused.  If there is no idle worker, and the maximum worker count has
       not yet been reached, a new worker will be constructed.  The maximum number of workers is zero (as during a
       shutdown), the job will be refused and this function will return false.  Otherwise, this function returns true.
       This function is thread-safe. */
    bool Schedule(TJob &&job, int priority = 0);

    /* Changes the minimum and maximum number of threads and the idle timeout to zero, causing all workers to shut down
       when they are done with their current jobs.  If this can be completed before the given timeout expires then the shutdown
       has been clean and this funtion returns true.  If one or more workers are still busy after the timeout expires, however,
       then this function interrupts those workers, forcing their self-destruction.  This is a forced shutdown, and this function
       returns false.  This function is thread-safe. */
    bool Shutdown(const std::chrono::milliseconds &timeout);

    private:

    /* The number of the signal we use when interrupting workers. */
    static const int Signum;

    /* The a set of signal bits including all signals EXCEPT the one we use for interrupting workers.
       The worker threads use this to mask out all other signals. */
    static const Signal::TSet MaskSet;

    /* The handler installer for our signal.  The handler we install does nothing, but it prevents
       the process from shutting down on signal delivery. */
    static const Signal::THandlerInstaller HandlerInstaller;

    /* An item in our job queue. */
    class TQueueItem {
      public:

      /* Cache the job and its priority. */
      template <typename TArg>
      TQueueItem(TArg &&job, int priority)
          : Job(std::forward<TArg>(job)), Priority(priority) {}

      /* Higher priority jobs go first. */
      bool operator<(const TQueueItem &that) const {
        assert(this);
        assert(&that);
        return Priority > that.Priority;
      }

      /* The job to be done. */
      mutable TJob Job;

      /* The priority at which it sorts. */
      int Priority;

    };  // TScheduler::TQueueItem

    /* A background worker. */
    class TWorker {
      NO_COPY_SEMANTICS(TWorker);
      public:

      /* Send the interrupt signal to each worker. */
      static void Interrupt(TScheduler *scheduler);

      /* Launch workers until we reach the given minimum number. */
      static void Launch(TScheduler *scheduler, std::unique_lock<std::mutex> &lock, size_t min_worker_count);

      private:

      /* Launches the background thread. */
      TWorker(TScheduler *scheduler);

      /* Detaches the background thread so the worker can self-destruct safely. */
      ~TWorker();

      /* Runs the background work loop. */
      void ThreadMain();

      /* The scheduler of which we are a part. */
      TScheduler *Scheduler;

      /* Linkage in the scheduler's list of workers. */
      TWorker *NextWorker, *PrevWorker;

      /* The thread running ThreadMain(). */
      std::thread Thread;

    };  // TScheduler::TWorker

    /* Constructor delegated to by the public constructor.  Also used directly by RunUntilCtrlC(). */
    TScheduler(const TPolicy &policy, const TOpt<pthread_t> &ctrl_c_thread, TMainJob *main_job = nullptr);

    /* Try to pop a job from the queue and block until we get one or until we should shut down.
       Return true if we got a job, false if the worker should shut down.  If, in the latter case, the
       worker is the only remaining worker, and if we're running under RunUntilCtrlC(), then send
       ctrl-c to the thread which started this scheduler. */
    bool TryPopJob(std::unique_lock<std::mutex> &lock, TJob &job);

    /* Covers all the member variables in this object. */
    mutable std::mutex Mutex;

    /* See accessor. */
    TPolicy Policy;

    /* A linked list of our current workers. */
    TWorker *FirstWorker, *LastWorker;

    /* The number of workers in our linked list. */
    size_t WorkerCount;

    /* The number of workers currently waiting for jobs. */
    size_t IdleCount;

    /* The number of job pushers currently waiting for idle workers to wake up. */
    size_t WakingCount;

    /* The queue of jobs waiting to be done. */
    std::priority_queue<TQueueItem> JobQueue;

    /* Notifies one worker when a job is pushed, or all workers when our scheduling policy changes. */
    std::condition_variable PolicyChangedOrJobPushed;

    /* Notifies all when a worker joins to or parts from our list of workers. */
    std::condition_variable WorkerListChanged;

    /* Notifies one job pusher when a worker stops being idle. */
    std::condition_variable WorkerAwoke;

    /* The thread, if any, to which we should direct a ctrl-c if we quiesce. */
    TOpt<pthread_t> CtrlCThread;

    /* True if we've reached a state of permanant quiescence. */
    bool PermanantlyQuiescent;

    /* A cached copy of the value returned by sched_get_priority_max(SCHED_FIFO). */
    static const int MaxPriority;

  };  // TScheduler

}  // Base
