/* <base/scheduler.cc>

   Implements <base/scheduler.h>.

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

#include <base/scheduler.h>

#include <algorithm>

#include <sched.h>

#include <base/debug_log.h>
#include <base/error_utils.h>
#include <base/zero.h>
#include <io/input_consumer.h>
#include <signal/masker.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Signal;

TScheduler::TPolicy::TPolicy()
    : MinWorkerCount(0), MaxWorkerCount(0), IdleTimeout(0), RealTime(false) {}

TScheduler::TPolicy::TPolicy(size_t min_worker_count, size_t max_worker_count, const milliseconds &idle_timeout, bool is_real_time)
    : MinWorkerCount(min_worker_count), MaxWorkerCount(max_worker_count), IdleTimeout(idle_timeout), RealTime(is_real_time) {
  assert(min_worker_count <= max_worker_count);
}

void TScheduler::TPolicy::RunUntilCtrlC(TMainJob &&main_job) const {
  assert(this);
  assert(&main_job);
  ShuttingDown = false;
  try {
    TMasker masker(*TSet(TSet::Full));
    THandlerInstaller handler(
        SIGINT,
        [](int) {
          ShuttingDown = true;
        }
    );
    TScheduler scheduler(*this, pthread_self(), &main_job);
    sigsuspend(&*TSet(TSet::Exclude, { SIGINT }));
  } catch (const exception &ex) {
    syslog(LOG_ERR, "run_until_c; standard exception; %s", ex.what());
  } catch (...) {
    syslog(LOG_ERR, "run_until_c; non-standard exception");
  }
}

const TScheduler::TPolicy TScheduler::TPolicy::Shutdown;

TScheduler::TScheduler(const TPolicy &policy)
    : TScheduler(policy, *TOpt<pthread_t>::Unknown) {
  ShuttingDown = false;
}

TScheduler::~TScheduler() {
  assert(this);
  ShuttingDown = true;
  Shutdown(milliseconds(0));
}

TScheduler::TPolicy TScheduler::GetPolicy() const {
  assert(this);
  lock_guard<mutex> lock(Mutex);
  return Policy;
}

void TScheduler::SetPolicy(const TPolicy &policy) {
  assert(this);
  assert(&policy);
  unique_lock<mutex> lock(Mutex);
  Policy = policy;
  PolicyChangedOrJobPushed.notify_all();
  TWorker::Launch(this, lock, policy.GetMinWorkerCount());
}

bool TScheduler::Schedule(TJob &&job, int priority) {
  assert(this);
  assert(&job);
  unique_lock<mutex> lock(Mutex);
  bool success = (Policy.GetMaxWorkerCount() > 0);
  if (success) {
    JobQueue.emplace(move(job), priority);
    if (IdleCount > WakingCount) {
      PolicyChangedOrJobPushed.notify_one();
      ++WakingCount;
      WorkerAwoke.wait(lock);
      --WakingCount;
    } else if (WorkerCount < Policy.GetMaxWorkerCount()) {
      TWorker::Launch(this, lock, WorkerCount + 1);
    }
  }
  return success;
}

bool TScheduler::Shutdown(const milliseconds &timeout) {
  assert(this);
  ShuttingDown = true;
  bool is_clean = true;
  unique_lock<mutex> lock(Mutex);
  if (WorkerCount) {
    Policy = TPolicy::Shutdown;
    do {
      PolicyChangedOrJobPushed.notify_all();
      if (WorkerListChanged.wait_for(lock, timeout) == cv_status::timeout) {
        TWorker::Interrupt(this);
        is_clean = false;
      }
    } while (WorkerCount);
  }
  return is_clean;
}

const int TScheduler::Signum = SIGUSR1;

const TSet TScheduler::MaskSet(TSet::Exclude, { Signum });

const THandlerInstaller TScheduler::HandlerInstaller(Signum);

void TScheduler::TWorker::Interrupt(TScheduler *scheduler) {
  assert(scheduler);
  for (TWorker *worker = scheduler->FirstWorker; worker; worker = worker->NextWorker) {
    pthread_kill(worker->Thread.native_handle(), Signum);
  }
}

void TScheduler::TWorker::Launch(TScheduler *scheduler, unique_lock<mutex> &lock, size_t min_worker_count) {
  assert(scheduler);
  assert(min_worker_count <= scheduler->Policy.GetMaxWorkerCount());
  while (scheduler->WorkerCount < min_worker_count && !scheduler->PermanantlyQuiescent) {
    new TWorker(scheduler);
    scheduler->WorkerListChanged.wait(lock);
  }
}

TScheduler::TWorker::TWorker(TScheduler *scheduler)
    : Scheduler(scheduler), NextWorker(nullptr), PrevWorker(nullptr),
      Thread(&TWorker::ThreadMain, this) {}

TScheduler::TWorker::~TWorker() {
  assert(this);
  Thread.detach();
}

void TScheduler::TWorker::ThreadMain() {
  assert(this);
  try {
    if (Scheduler->Policy.IsRealTime()) {
      struct sched_param sp;
      Zero(sp);
      sp.sched_priority = MaxPriority - 1;
      IfNe0(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp));
    }
    TMasker masker(*MaskSet);
    bool is_linked = false;
    for (;;) {
      TJob job;
      /* critical section */ {
        unique_lock<mutex> lock(Scheduler->Mutex);
        /* If we just started, link to the scheduler. */
        if (!is_linked) {
          PrevWorker = Scheduler->LastWorker;
          (PrevWorker ? PrevWorker->NextWorker : Scheduler->FirstWorker) = this;
          Scheduler->LastWorker = this;
          ++(Scheduler->WorkerCount);
          Scheduler->WorkerListChanged.notify_all();
          is_linked = true;
        }
        /* Try to get a job.  If we can't, unlink from the queue and self-destruct. */
        if (!Scheduler->TryPopJob(lock, job)) {
          (PrevWorker ? PrevWorker->NextWorker : Scheduler->FirstWorker) = NextWorker;
          (NextWorker ? NextWorker->PrevWorker : Scheduler->LastWorker) = PrevWorker;
          --(Scheduler->WorkerCount);
          Scheduler->WorkerListChanged.notify_all();
          delete this;
          break;
        }
      }  // critical section
      /* Do the job we got. */
      try {
        try {
          job();
        } catch (const system_error &error) {
          if (!WasInterrupted(error)) {
            throw;
          }
          syslog(LOG_INFO, "worker %p; interrupted", this);
        }
      } catch (const exception &ex) {
        syslog(LOG_ERR, "worker %p; job threw standard exception; %s", this, ex.what());
      } catch (...) {
        syslog(LOG_ERR, "worker %p; job threw non-standard exception", this);
      }
    }  // forever
  } catch (const exception &ex) {
    syslog(LOG_EMERG, "worker %p; top-level standard exception; %s", this, ex.what());
  } catch (...) {
    syslog(LOG_EMERG, "worker %p; top-level non-standard exception", this);
  }
}

TScheduler::TScheduler(const TPolicy &policy, const TOpt<pthread_t> &ctrl_c_thread, TMainJob *main_job)
    : Policy(policy), FirstWorker(nullptr), LastWorker(nullptr), WorkerCount(0), IdleCount(0), WakingCount(0),
      CtrlCThread(ctrl_c_thread), PermanantlyQuiescent(false) {
  if (main_job) {
    JobQueue.emplace(bind(move(*main_job), this), 0);
  }
  size_t min_worker_count = max(policy.GetMinWorkerCount(), static_cast<size_t>(main_job ? 1 : 0));
  if (min_worker_count) {
    unique_lock<mutex> lock(Mutex);
    TWorker::Launch(this, lock, min_worker_count);
  }
}

bool TScheduler::TryPopJob(unique_lock<mutex> &lock, TJob &job) {
  assert(this);
  assert(&lock);
  assert(&job);
  bool success;
  do {
    /* If there are too many workers, leave without a job.
       But if there aren't too many workers, and there's a job to do, take the job. */
    success = (WorkerCount <= Policy.GetMaxWorkerCount());
    if (!success || !JobQueue.empty()) {
      break;
    }
    /* If we're running under RunUntilCtrlC() and we're the last non-idle thread, then going idle would
       result in a permanently quiescent scheduler.  Rather than allow that, we shut the whole queue down. */
    if (CtrlCThread && IdleCount + 1 == WorkerCount) {
      PermanantlyQuiescent = true;
      pthread_kill(*CtrlCThread, SIGINT);
      success = false;
      syslog(LOG_INFO, "scheduler %p; permanently quiescent; synthesized ctrl-c", this);
    } else {
      /* Go idle, waiting for a job to show up or for a policy change. */
      ++IdleCount;
      if (WorkerCount > Policy.GetMinWorkerCount()) {
        /* There are more than the minimum number of threads, so only wait for a finite period of time.
           If nothing happens, leave without a job. */
        success = (PolicyChangedOrJobPushed.wait_for(lock, Policy.GetIdleTimeout()) == cv_status::no_timeout);
        if (WakingCount) {
          success = true;
        }
      } else {
        /* The number of threads is already minimal, so wait indefinitely. */
        PolicyChangedOrJobPushed.wait(lock);
        assert(success);
      }
      /* Awake from being idle.  If a job pusher woke us, unblock him. */
      --IdleCount;
      if (success) {
        WorkerAwoke.notify_one();
      }
    }
  } while (success);
  /* If we're taking a job, pop it off the queue. */
  if (success) {
    swap(job, JobQueue.top().Job);
    JobQueue.pop();
  }
  return success;
}

const int TScheduler::MaxPriority = sched_get_priority_max(SCHED_FIFO);
