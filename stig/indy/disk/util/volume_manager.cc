/* <stig/indy/disk/util/volume_manager.cc>

   Implements <stig/indy/disk/util/volume_manager.h>.

   Copyright 2010-2014 Tagged

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/indy/disk/util/volume_manager.h>

#include <iostream> /* TODO GET RID OF */

#include <linux/fs.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

#include <base/booster.h>
#include <base/sigma_calc.h>
#include <base/timer.h>
#include <base/zero.h>
#include <stig/indy/disk/util/corruption_detector.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy::Disk::Util;

Base::TThreadLocalPoolManager<TDiskController::TEvent> TDiskController::TEvent::DiskEventPoolManager;
__thread Base::TThreadLocalPoolManager<TDiskController::TEvent>::TThreadLocalRegisteredPool *TDiskController::TEvent::LocalEventPool = nullptr;

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        void TDiskController::Report(std::stringstream &/*ss*/, double /*elapsed_time*/) const {
          /* this is where we can report any controller or device specific metrics */
        }

        /* TODO */
        class TGroupRequest {
          NO_COPY_SEMANTICS(TGroupRequest);
          public:

          /* TODO */
          TGroupRequest(size_t total_num_request,
                        const TIOCallback &cb)
              : NumFinished(0UL),
                NumRequest(total_num_request),
                Cb(cb),
                Result(Success),
                ErrStr(nullptr) {}

          /* TODO */
          inline void Callback(TDiskResult disk_result, const char *err_str) {
            assert(this);
            size_t prev = std::atomic_fetch_add(&NumFinished, 1UL);
            switch (Result) {
              case Success : {
                switch (disk_result) {
                  case Success : {
                    break;
                  }
                  case Error : {
                    Result = disk_result;
                    ErrStr = err_str;
                    break;
                  }
                  case DiskFailure : {
                    Result = disk_result;
                    break;
                  }
                  case ServerShutdown : {
                    Result = disk_result;
                    break;
                  }
                }
                break;
              }
              default : {
                /* if we're already in an error state, stay in it. */
                break;
              }
            }
            if ((prev + 1UL) == NumRequest) {
              Cb(Result, ErrStr);
              delete this;
            }
          }

          private:

          /* TODO */
          std::atomic<size_t> NumFinished;

          /* TODO */
          size_t NumRequest;

          /* TODO */
          const TIOCallback Cb;

          /* TODO */
          TDiskResult Result;

          /* TODO */
          const char *ErrStr;

        };  // TGroupRequest

      }

    }

  }

}

TDiskController::TDiskController()
    : DeviceCollection(this)
#ifndef NDEBUG
    ,NextId(0UL)
#endif
{
}

TDiskController::~TDiskController() {
}

void TDiskController::QueueRunner(std::vector<TPersistentDevice *> device_vec, bool no_realtime, size_t core) {
  assert(this);
  const size_t max_aio_num = 64;
  size_t inflight = 0UL;

  io_context_t ctxp(0);
  try {
    Base::IfWeird(io_setup(max_aio_num, &ctxp));
  } catch (const std::exception &ex) {
    syslog(LOG_ERR, "Error in io_setup: [%s]", ex.what());
    throw;
  }
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(core, &mask);
  Base::IfLt0(sched_setaffinity(syscall(SYS_gettid), sizeof(cpu_set_t), &mask));
  TOpt<TBooster> booster;
  if (!no_realtime) {
    booster.MakeKnown(SCHED_FIFO);
  }
  const size_t rt_not_to_exceed_depth = 32UL;
  const size_t medium_not_to_exceed_depth = 6UL;
  const size_t low_not_to_exceed_depth = 2UL;
  struct iocb *ioq[std::max(rt_not_to_exceed_depth, std::max(medium_not_to_exceed_depth, low_not_to_exceed_depth)) * device_vec.size()];
  struct io_event *io_ev = 0;
  io_ev = new struct io_event[max_aio_num];
  try {
    memset(io_ev, 0, max_aio_num * sizeof(struct io_event));
    size_t num_laps_without_work = 0UL;
    const timespec lap_wait{0, 10000UL};
    const size_t laps_before_sleep = 5UL;
    for (;;) {
      ++num_laps_without_work;
      if (num_laps_without_work > laps_before_sleep) {
        nanosleep(&lap_wait, NULL);
      }
      /* wait for a queue to be ready */
      for (TPersistentDevice *ready_device : device_vec) {
        TEvent *cur_tail = __sync_lock_test_and_set(&ready_device->IncomingEventQueue, nullptr);
        if (cur_tail) {
          num_laps_without_work = 0UL;
          /* there were inbound events scheduled against this device. Process these events (which are in reverse order) and put them into their
             Realtime / Medium / Low priority queues. */
          TEvent *append_after_rt = ready_device->RealTimePrioEventQueue.TryGetLastMember();
          TEvent *append_after_m = ready_device->MediumPrioEventQueue.TryGetLastMember();
          TEvent *append_after_l = ready_device->LowPrioEventQueue.TryGetLastMember();
          size_t num_dequeue = 0UL;
          for (TEvent *cur_event = cur_tail; cur_event; cur_event = cur_event->NextEvent, ++num_dequeue) {
            switch (cur_event->Iocb.aio_reqprio) {
              case RealTimePriority: {
                if (append_after_rt) {
                  cur_event->DeviceMembership.Insert(&append_after_rt->DeviceMembership, InvCon::Fwd);
                } else {
                  cur_event->DeviceMembership.Insert(&ready_device->RealTimePrioEventQueue, InvCon::Rev);
                }
                break;
              }
              case MediumPriority: {
                if (append_after_m) {
                  cur_event->DeviceMembership.Insert(&append_after_m->DeviceMembership, InvCon::Fwd);
                } else {
                  cur_event->DeviceMembership.Insert(&ready_device->MediumPrioEventQueue, InvCon::Rev);
                }
                break;
              }
              case LowPriority: {
                if (append_after_l) {
                  cur_event->DeviceMembership.Insert(&append_after_l->DeviceMembership, InvCon::Fwd);
                } else {
                  cur_event->DeviceMembership.Insert(&ready_device->LowPrioEventQueue, InvCon::Rev);
                }
                break;
              }
            }
          }
        }
      }
      size_t ioq_pos = 0UL;
      for (TPersistentDevice *ready_device : device_vec) {
        if (!ready_device->RealTimePrioEventQueue.IsEmpty() || !ready_device->MediumPrioEventQueue.IsEmpty() || !ready_device->LowPrioEventQueue.IsEmpty()) {
          num_laps_without_work = 0UL;
          size_t num_removed = 0UL;
          size_t cur_inflight = ready_device->Inflight.load();
          for (TEvent *member = ready_device->RealTimePrioEventQueue.TryGetFirstMember(); member && (cur_inflight + num_removed) < rt_not_to_exceed_depth/*num_removed < max_to_submit*/; member = ready_device->RealTimePrioEventQueue.TryGetFirstMember(), ++num_removed) {
            ioq[ioq_pos] = &(member->Iocb);
            ++ioq_pos;
            member->DeviceMembership.Remove();
          }
          for (TEvent *member = ready_device->MediumPrioEventQueue.TryGetFirstMember(); member && (cur_inflight + num_removed) < medium_not_to_exceed_depth/*num_removed < max_to_submit*/; member = ready_device->MediumPrioEventQueue.TryGetFirstMember(), ++num_removed) {
            ioq[ioq_pos] = &(member->Iocb);
            ++ioq_pos;
            member->DeviceMembership.Remove();
          }
          for (TEvent *member = ready_device->LowPrioEventQueue.TryGetFirstMember(); member && (cur_inflight + num_removed) < low_not_to_exceed_depth/*num_removed < max_to_submit*/; member = ready_device->LowPrioEventQueue.TryGetFirstMember(), ++num_removed) {
            ioq[ioq_pos] = &(member->Iocb);
            ++ioq_pos;
            member->DeviceMembership.Remove();
          }
          if (num_removed) {
            ready_device->Inflight += num_removed;
            #ifndef NDEBUG
            /* register all these events in the outstanding set */ {
              std::lock_guard<std::mutex> lock(OutstandingIdMutex);
              for (size_t i = 0; i < num_removed; ++i) {
                const size_t this_id = ++NextId;
                reinterpret_cast<TEvent *>(ioq[i]->data)->RequestId = this_id;
                OutstandingIdSet.insert(this_id);
              }
            }
            #endif
          }
        }
      }

      if (ioq_pos > 0) {
        inflight += ioq_pos;
        int ret = io_submit(ctxp, ioq_pos, ioq);
        if (ret < 0) {
          syslog(LOG_ERR, "Error in io_submit; nr=[%ld]", ioq_pos);
          for (size_t nr = 0; nr < ioq_pos; ++nr) {
            switch (ioq[nr]->aio_lio_opcode) {
              case IO_CMD_PREAD: {
                syslog(LOG_INFO, "ioq[%ld] IO_CMD_PREAD, offset=[%lld], nbytes=[%ld]", nr, ioq[nr]->u.c.offset, ioq[nr]->u.c.nbytes);
                break;
              }
              case IO_CMD_PWRITE: {
                syslog(LOG_INFO, "ioq[%ld] IO_CMD_PWRITE, offset=[%lld], nbytes=[%ld]", nr, ioq[nr]->u.c.offset, ioq[nr]->u.c.nbytes);
                break;
              }
              case IO_CMD_FSYNC: {
                throw;
                break;
              }
              case IO_CMD_FDSYNC: {
                throw;
                break;
              }
              case IO_CMD_POLL: {
                throw;
                break;
              }
              case IO_CMD_NOOP: {
                throw;
                break;
              }
              case IO_CMD_PREADV: {
                syslog(LOG_INFO, "ioq[%ld] IO_CMD_PREADV, nr=[%d], offset=[%lld], size=[%ld]", nr, ioq[nr]->u.v.nr, ioq[nr]->u.v.offset, ioq[nr]->u.v.vec[0].iov_len);
                break;
              }
              case IO_CMD_PWRITEV: {
                syslog(LOG_INFO, "ioq[%ld] IO_CMD_PWRITEV, nr=[%d], offset=[%lld], size=[%ld]", nr, ioq[nr]->u.v.nr, ioq[nr]->u.v.offset, ioq[nr]->u.v.vec[0].iov_len);
                break;
              }
            }
          }
          ThrowSystemError(-ret);
        }
        if (ret != static_cast<int>(ioq_pos)) {
          syslog(LOG_ERR, "io_submit did not sumbit as as many events as requested [%ld] vs. [%d]", ioq_pos, ret);
          throw;
        }
      }

      if (inflight > 0) {
        int num_popped = io_getevents(ctxp, 1, max_aio_num, io_ev, NULL);
        if (num_popped > 0) {
          inflight -= num_popped;
        }
        try {

          #ifndef NDEBUG
          /* check and remove all these events in the outstanding set */ {
            std::lock_guard<std::mutex> lock(OutstandingIdMutex);
            for (int i = 0; i < num_popped; ++i) {
              TEvent *compl_event = reinterpret_cast<TEvent *>(io_ev[i].data);
              const size_t request_id = compl_event->RequestId;
              auto pos = OutstandingIdSet.find(request_id);
              if (pos == OutstandingIdSet.end()) {
                syslog(LOG_ERR, "Completing request id [%ld] that was not outstanding.", request_id);
                throw std::runtime_error("Completing request id that was not outstanding");
              }
              OutstandingIdSet.erase(pos);
            }
          }
          #endif

          for (int i = 0; i < num_popped; ++i) {
            TEvent *compl_event = reinterpret_cast<TEvent *>(io_ev[i].data);
            const struct iocb &io = compl_event->Iocb;
            --compl_event->Device->Inflight;
            try {
              switch (compl_event->Kind) {
                case TEvent::TriggeredRead: {
                  if (likely(io_ev[i].res == io.u.c.nbytes && io_ev[i].res2 == 0)) {
                    bool passed_corruption_check = compl_event->Device->CheckCorruptCheck(compl_event->BufKind, io.u.c.buf, io.u.c.offset, io.u.c.nbytes);
                    if (likely(passed_corruption_check)) {
                      compl_event->TriggerOp->Callback(Success, "");
                    } else {
                      stringstream ss;
                      ss << compl_event->CodeLocation;
                      syslog(LOG_ERR, "Corrupt data Reading @ [%lld] in device [%s] from [%s]", io.u.c.offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                      if (compl_event->AbortOnError) {
                        abort();
                      }
                      compl_event->TriggerOp->Callback(Error, "Corrupt Data");
                    }
                  } else {
                    stringstream ss;
                    ss << compl_event->CodeLocation;
                    syslog(LOG_ERR, "Disk Error res=[%ld], res2=[%ld] Reading @ [%lld] in device [%s] from [%s]", io_ev[i].res, io_ev[i].res2, io.u.c.offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                    if (compl_event->AbortOnError) {
                      abort();
                    }
                    compl_event->TriggerOp->Callback(Error, "Disk Error");
                  }
                  break;
                }
                case TEvent::TriggeredReadV: {
                  long long offset = io_ev[i].obj->u.v.offset;
                  int nr = io_ev[i].obj->u.v.nr;
                  size_t expected_size = 0UL;
                  for (int j = 0; j < nr; ++j) {
                    expected_size += io_ev[i].obj->u.v.vec[j].iov_len;
                  }
                  if (likely(io_ev[i].res == expected_size && io_ev[i].res2 == 0)) {
                    assert(io_ev[i].obj->aio_lio_opcode == IO_CMD_PREADV);
                    assert(static_cast<size_t>(nr) == compl_event->TriggerVOp.IoVCnt);
                    bool passed_corruption_check = true;
                    long long processed = 0UL;
                    for (int n = 0; n < nr; ++n) {
                      if (unlikely(!compl_event->Device->CheckCorruptCheck(compl_event->BufKind, io_ev[i].obj->u.v.vec[n].iov_base, offset + processed, io_ev[i].obj->u.v.vec[n].iov_len))) {
                        passed_corruption_check = false;
                        break;
                      }
                      processed += io_ev[i].obj->u.v.vec[n].iov_len;
                    }
                    if (likely(passed_corruption_check)) {
                      compl_event->TriggerVOp.Trigger->Callback(Success, "");
                    } else {
                      stringstream ss;
                      ss << compl_event->CodeLocation;
                      syslog(LOG_ERR, "Corrupt data Reading @ [%lld] in device [%s] from [%s]", offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                      if (compl_event->AbortOnError) {
                        abort();
                      }
                      compl_event->TriggerVOp.Trigger->Callback(Error, "Corrupt Data");
                    }
                  } else {
                    stringstream ss;
                    ss << compl_event->CodeLocation;
                    syslog(LOG_ERR, "Disk Error res=[%ld], res2=[%ld] Reading @ [%lld] in device [%s] from [%s]", io_ev[i].res, io_ev[i].res2, offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                    if (compl_event->AbortOnError) {
                      abort();
                    }
                    compl_event->TriggerVOp.Trigger->Callback(Error, "Disk Error");
                  }
                  break;
                }
                case TEvent::TriggeredWrite: {
                  if (likely(io_ev[i].res == io.u.c.nbytes && io_ev[i].res2 == 0)) {
                    compl_event->TriggerOp->Callback(Success, "");
                  } else {
                    stringstream ss;
                    ss << compl_event->CodeLocation;
                    syslog(LOG_ERR, "Disk Error res=[%ld], res2=[%ld] Writing @ [%lld] in device [%s] from [%s]", io_ev[i].res, io_ev[i].res2, io.u.c.offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                    if (compl_event->AbortOnError) {
                      abort();
                    }
                    compl_event->TriggerOp->Callback(Error, "Disk Error");
                  }
                  break;
                }
                case TEvent::CallbackRead: {
                  if (likely(io_ev[i].res == io.u.c.nbytes && io_ev[i].res2 == 0)) {
                    bool passed_corruption_check = compl_event->Device->CheckCorruptCheck(compl_event->BufKind, io.u.c.buf, io.u.c.offset, io.u.c.nbytes);
                    if (likely(passed_corruption_check)) {
                      compl_event->CallbackOp(Success, "");
                    } else {
                      stringstream ss;
                      ss << compl_event->CodeLocation;
                      syslog(LOG_ERR, "Corrupt data Reading @ [%lld] in device [%s] from [%s]", io.u.c.offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                      if (compl_event->AbortOnError) {
                        abort();
                      }
                      compl_event->CallbackOp(Error, "Corrupt Data");
                    }
                  } else {
                    stringstream ss;
                    ss << compl_event->CodeLocation;
                    syslog(LOG_ERR, "Disk Error res=[%ld], res2=[%ld] Reading @ [%lld] in device [%s] from [%s]", io_ev[i].res, io_ev[i].res2, io.u.c.offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                    if (compl_event->AbortOnError) {
                      abort();
                    }
                    compl_event->CallbackOp(Error, "Disk Error");
                  }
                  break;
                }
                case TEvent::CallbackReadV: {
                  long long offset = io_ev[i].obj->u.v.offset;
                  int nr = io_ev[i].obj->u.v.nr;
                  size_t expected_size = 0UL;
                  for (int j = 0; j < nr; ++j) {
                    expected_size += io_ev[i].obj->u.v.vec[j].iov_len;
                  }
                  if (likely(io_ev[i].res == expected_size && io_ev[i].res2 == 0)) {
                    assert(io_ev[i].obj->aio_lio_opcode == IO_CMD_PREADV);
                    assert(static_cast<size_t>(nr) == compl_event->TriggerVOp.IoVCnt);
                    bool passed_corruption_check = true;
                    long long processed = 0UL;
                    for (int n = 0; n < nr; ++n) {
                      if (unlikely(!compl_event->Device->CheckCorruptCheck(compl_event->BufKind, io_ev[i].obj->u.v.vec[n].iov_base, offset + processed, io_ev[i].obj->u.v.vec[n].iov_len))) {
                        passed_corruption_check = false;
                        break;
                      }
                      processed += io_ev[i].obj->u.v.vec[n].iov_len;
                    }
                    if (likely(passed_corruption_check)) {
                      compl_event->CallbackVOp.GroupRequest->Callback(Success, "");
                    } else {
                      stringstream ss;
                      ss << compl_event->CodeLocation;
                      syslog(LOG_ERR, "Corrupt data Reading @ [%lld] in device [%s] from [%s]", offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                      if (compl_event->AbortOnError) {
                        abort();
                      }
                      compl_event->CallbackVOp.GroupRequest->Callback(Error, "Corrupt Data");
                    }
                  } else {
                    stringstream ss;
                    ss << compl_event->CodeLocation;
                    syslog(LOG_ERR, "Disk Error res=[%ld], res2=[%ld] Reading @ [%lld] in device [%s] from [%s]", io_ev[i].res, io_ev[i].res2, offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                    if (compl_event->AbortOnError) {
                      abort();
                    }
                    compl_event->CallbackVOp.GroupRequest->Callback(Error, "Disk Error");
                  }
                  break;
                }
                case TEvent::CallbackWrite: {
                  if (likely(io_ev[i].res == io.u.c.nbytes && io_ev[i].res2 == 0)) {
                    compl_event->CallbackOp(Success, "");
                  } else {
                    stringstream ss;
                    ss << compl_event->CodeLocation;
                    syslog(LOG_ERR, "Disk Error res=[%ld], res2=[%ld] Writing @ [%lld] in device [%s] from [%s]", io_ev[i].res, io_ev[i].res2, io.u.c.offset, compl_event->Device->GetDevicePath(), ss.str().c_str());
                    if (compl_event->AbortOnError) {
                      abort();
                    }
                    compl_event->CallbackOp(Error, "Disk Error");
                  }
                  break;
                }
              }
            } catch (const std::exception &ex) {
              std::cerr << "Caught error while completing io : [" << compl_event->CodeLocation << "] Kind [";
              switch(compl_event->Kind) {
                case TEvent::TriggeredRead: {
                  std::cerr << "TriggeredRead";
                  break;
                }
                case TEvent::TriggeredReadV: {
                  std::cerr << "TriggeredReadV";
                  break;
                }
                case TEvent::TriggeredWrite: {
                  std::cerr << "TriggeredWrite";
                  break;
                }
                case TEvent::CallbackRead: {
                  std::cerr << "CallbackRead";
                  break;
                }
                case TEvent::CallbackReadV: {
                  std::cerr << "CallbackReadV";
                  break;
                }
                case TEvent::CallbackWrite: {
                  std::cerr << "CallbackWrite";
                  break;
                }
              }
              std::cerr << "]" << std::endl;
              throw;
            }
          }
          /* now that we're done completing all the events, reset them and give them back to their pool. */
          for (int i = 0; i < num_popped; ++i) {
            TEvent *compl_event = reinterpret_cast<TEvent *>(io_ev[i].data);
            compl_event->Reset(true);
          }
        } catch (const std::exception &ex) {
          syslog(LOG_ERR, "QueueRunner caught error while iterating over completion events");
          throw;
        }
      }
    }
  } catch (const std::exception &ex) {
    syslog(LOG_ERR, "QueueRunner caught error [%s]", ex.what());
    throw;
  } catch (...) {
    throw;
  }
}

void TDiskController::TEvent::Init(TPersistentDevice *device,
                                   Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                                   const Base::TCodeLocation &code_location /* DEBUG */,
                                   TKind kind,
                                   DiskPriority priority,
                                   const TOffset logical_start_offset,
                                   TCompletionTrigger &trigger,
                                   TBufKind buf_kind,
                                   void *buf,
                                   const TOffset offset,
                                   long long nbytes,
                                   bool abort_on_error) {
  Device = device;
  NextEvent = nullptr;
  EventPool = pool;
  Kind = kind;
  BufKind = buf_kind;
  LogicalStartOffset = logical_start_offset;
  AbortOnError = abort_on_error;
  #ifndef NDEBUG
  RequestId = -1;
  #endif
  CodeLocation = code_location;
  this->TriggerOp = &trigger;

  switch (Kind) {
    case TriggeredRead: {
      io_prep_pread(&Iocb, device->GetDiskFd(), buf, nbytes, offset);
      break;
    }
    case TriggeredWrite: {
      io_prep_pwrite(&Iocb, device->GetDiskFd(), buf, nbytes, offset);
      break;
    }
    default: {
      throw std::logic_error("Invalid event kind for given constructor.");
    }
  }
  CommonInit(priority);
}

void TDiskController::TEvent::Init(TPersistentDevice *device,
                                   Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                                   const Base::TCodeLocation &code_location /* DEBUG */,
                                   TKind kind,
                                   DiskPriority priority,
                                   const TOffset logical_start_offset,
                                   TCompletionTrigger &trigger,
                                   TBufKind buf_kind,
                                   const std::vector<void *> &buf_vec,
                                   const TOffset offset,
                                   long long nbytes,
                                   bool abort_on_error) {
  assert(buf_vec.size() <= MaxSegmentsPerIO);
  if (unlikely(buf_vec.size() > MaxSegmentsPerIO)) {
    throw std::logic_error("Maximum Segments Per IO threshold exceeded.");
  }
  Device =device;
  NextEvent = nullptr;
  EventPool = pool;
  Kind = kind;
  BufKind = buf_kind;
  LogicalStartOffset = logical_start_offset;
  AbortOnError = abort_on_error;
  #ifndef NDEBUG
  RequestId = -1;
  #endif
  CodeLocation = code_location;
  TriggerVOp.Trigger = &trigger;
  CallbackVOp.IoVCnt = 0UL;

  const size_t buf_size = GetPhysicalSize(buf_kind);
  assert(static_cast<size_t>(nbytes) == buf_vec.size() * buf_size);
  switch (Kind) {
    case TriggeredReadV: {
      TriggerVOp.IoVCnt = buf_vec.size();
      for (size_t i = 0; i < buf_vec.size(); ++i) {
        TriggerVOp.IoVec[i].iov_base = buf_vec[i];
        TriggerVOp.IoVec[i].iov_len = buf_size;
      }
      io_prep_preadv(&Iocb, device->GetDiskFd(), TriggerVOp.IoVec, TriggerVOp.IoVCnt, offset);
      break;
    }
    default: {
      throw std::logic_error("Invalid event kind for given constructor.");
    }
  }
  CommonInit(priority);
}

void TDiskController::TEvent::Init(TPersistentDevice *device,
                                   Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                                   const Base::TCodeLocation &code_location /* DEBUG */,
                                   TKind kind,
                                   DiskPriority priority,
                                   const TOffset logical_start_offset,
                                   const TIOCallback &io_cb,
                                   TBufKind buf_kind,
                                   void *buf,
                                   const TOffset offset,
                                   long long nbytes,
                                   bool abort_on_error) {
  Device = device;
  NextEvent = nullptr;
  EventPool = pool;
  Kind = kind;
  BufKind = buf_kind;
  LogicalStartOffset = logical_start_offset;
  AbortOnError = abort_on_error;
  #ifndef NDEBUG
  RequestId = -1;
  #endif
  CodeLocation = code_location;
  new (&CallbackOp) TIOCallback(io_cb);

  switch (Kind) {
    case CallbackRead: {
      io_prep_pread(&Iocb, device->GetDiskFd(), buf, nbytes, offset);
      break;
    }
    case CallbackWrite: {
      io_prep_pwrite(&Iocb, device->GetDiskFd(), buf, nbytes, offset);
      break;
    }
    default: {
      throw std::logic_error("Invalid event kind for given constructor.");
    }
  }
  CommonInit(priority);
}

void TDiskController::TEvent::Init(TPersistentDevice *device,
                                   Base::TThreadLocalPoolManager<TEvent>::TThreadLocalRegisteredPool *pool,
                                   const Base::TCodeLocation &code_location /* DEBUG */,
                                   TKind kind,
                                   DiskPriority priority,
                                   const TOffset logical_start_offset,
                                   TGroupRequest *group_request,
                                   TBufKind buf_kind,
                                   const std::vector<void *> &buf_vec,
                                   const TOffset offset,
                                   long long nbytes,
                                   bool abort_on_error) {
  assert(buf_vec.size() <= MaxSegmentsPerIO);
  if (unlikely(buf_vec.size() > MaxSegmentsPerIO)) {
    throw std::logic_error("Maximum Segments Per IO threshold exceeded.");
  }
  Device = device;
  NextEvent = nullptr;
  EventPool = pool;
  Kind = kind;
  BufKind = buf_kind;
  LogicalStartOffset = logical_start_offset;
  AbortOnError = abort_on_error;
  #ifndef NDEBUG
  RequestId = -1;
  #endif
  CodeLocation = code_location;
  CallbackVOp.GroupRequest = group_request;
  CallbackVOp.IoVCnt = 0UL;

  const size_t buf_size = GetPhysicalSize(buf_kind);
  assert(static_cast<size_t>(nbytes) == buf_vec.size() * buf_size);
  switch (Kind) {
    case CallbackReadV: {
      CallbackVOp.IoVCnt = buf_vec.size();
      for (size_t i = 0; i < buf_vec.size(); ++i) {
        CallbackVOp.IoVec[i].iov_base = buf_vec[i];
        CallbackVOp.IoVec[i].iov_len = buf_size;
      }
      io_prep_preadv(&Iocb, device->GetDiskFd(), CallbackVOp.IoVec, TriggerVOp.IoVCnt, offset);
      break;
    }
    default: {
      throw std::logic_error("Invalid event kind for given constructor.");
    }
  }
  CommonInit(priority);
}

void TDiskController::TEvent::Reset(bool back_to_pool) {
  switch (Kind) {
    case TriggeredRead: {
      break;
    }
    case TriggeredReadV: {
      CallbackVOp.IoVCnt = 0UL;
      break;
    }
    case TriggeredWrite: {
      break;
    }
    case CallbackRead: {
      CallbackOp.~TIOCallback();
      break;
    }
    case CallbackReadV: {
      CallbackVOp.IoVCnt = 0UL;
      break;
    }
    case CallbackWrite: {
      CallbackOp.~TIOCallback();
      break;
    }
  }
  if (back_to_pool) {
    EventPool->Free(this);
  }
}

TDiskController::TEvent::~TEvent() {
  Reset(false);
}

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        class TVolume::TStrategy {
          NO_COPY_SEMANTICS(TStrategy);
          public:

          /* TODO */
          typedef size_t TLogicalExtentStart;

          /* An object that gets filled in to represent all the sequential IO to be performed to a device. The kind of IO (R/W) must be the same. */
          class TDeviceRequest {
            public:

            /* TODO */
            TDeviceRequest()
                : NumReq(0UL), PhysicalOffsetStart(0UL), TotalBytes(0UL) {}

            /* TODO */
            void AddRequest(void *buf, size_t physical_offset_on_device, size_t num_bytes, size_t max_segments_per_io, size_t max_sector_kb_per_io) {
              const size_t op_pos = std::max(NumReq / max_segments_per_io, TotalBytes / (max_sector_kb_per_io * 1024));
              if (NumReq > 0) {
                assert(physical_offset_on_device == PhysicalOffsetStart + TotalBytes);
              } else {
                PhysicalOffsetStart = physical_offset_on_device;
              }
              if (op_pos < VecPerOp.size()) {
                assert(op_pos == VecPerOp.size() - 1UL);
                VecPerOp[op_pos].push_back(buf);
                assert(VecPerOp[op_pos].size() <= max_segments_per_io);
                assert(VecPerOp[op_pos].size() * num_bytes <= max_sector_kb_per_io * 1024);
              } else {
                VecPerOp.emplace_back(std::vector<void *>{buf});
              }
              ++NumReq;
              TotalBytes += num_bytes;

            }

            /* TODO */
            inline size_t GetNumRequest() const {
              assert(this);
              return NumReq;
            }

            /* TODO */
            inline size_t GetTotalNumBytes() const {
              assert(this);
              return TotalBytes;
            }

            /* TODO */
            inline size_t GetNumIops() const {
              assert(this);
              return VecPerOp.size();
            }

            private:

            /* TODO */
            size_t NumReq;

            /* TODO */
            size_t PhysicalOffsetStart;

            /* TODO */
            size_t TotalBytes;

            /* TODO : more efficient */
            std::vector<std::vector<void *>> VecPerOp;

            /* TODO */
            friend class TStrategy;

          };  // TDeviceRequest

          /* TODO */
          virtual ~TStrategy() {
            assert(this);
            /* acquire discard lock */ {
              std::lock_guard<std::mutex> lock(DiscardMapLock);
              free(DiscardMapBuf);
            }  // release discard lock
            /* acquire block lock */ {
              std::lock_guard<std::mutex> lock(BlockMapLock);
              free(BlockMapBuf);
            }  // release block lock
          }

          /* TODO */
          virtual void DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger) = 0;
          virtual void DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger, const TIOCallback &cb) = 0;

          /* TODO */
          virtual void DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger) = 0;
          virtual void DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger, const TIOCallback &cb) = 0;

          /* TODO */
          virtual void DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array, size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger) = 0;
          virtual void DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array, size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger, const TIOCallback &cb) = 0;

          /* TODO */
          virtual void DelegateAppendTouchedDevicesToSet(TDeviceSet &device_set, const TBlockRange &block_range) const = 0;

          /* TODO */
          void DiscardAll() {
            for (const auto &device_set : DeviceVec) {
              for (TDevice *device : device_set) {
                device->DiscardAll();
              }
            }
          }

          /* TODO */
          std::pair<size_t, size_t> AppendUsage(std::stringstream &ss) const;

          /* TODO */
          inline void TryAllocateSequentialBlocks(size_t num_blocks, const std::function<void (const TBlockRange &block_range)> &cb);

          /* TODO */
          void FreeSequentialBlocks(const TBlockRange &block_range);

          /* TODO */
          void MarkBlockRangeUsed(const TBlockRange &block_range);

          /* TODO */
          bool Init(const TExtentSet &extent_set);

          /* TODO */
          inline const std::vector<TLogicalExtent> &GetLogicalExtentVec() const {
            assert(this);
            return ExtentVec;
          }

          protected:

          /* TODO */
          TStrategy(TVolume *volume, Base::TScheduler *scheduler)
              : Volume(volume),
                Scheduler(scheduler),
                BlockMapBuf(nullptr),
                CachedStart(0UL),
                DiscardMapBuf(nullptr),
                BlocksUsed(0UL),
                DiscardBlockWaiting(0UL),
                SuperBytes(PhysicalBlockSize),
                NumBlocks(((volume->GetDesc().NumLogicalExtent / volume->GetDesc().ReplicationFactor) * volume->GetDesc().DeviceDesc.Capacity) / PhysicalBlockSize),
                NumBlocksPerExtent(NumBlocks / Volume->GetDesc().NumLogicalExtent),
                BlockMapByteSize(ceil(static_cast<double>(NumBlocks) / 8)),
                BlockMapBufByteSize(ceil(static_cast<double>(NumBlocks) / (getpagesize() * 64)) * getpagesize()) {
            assert(NumBlocks % Volume->GetDesc().NumLogicalExtent == 0UL);
            try {
              std::lock_guard<std::mutex> lock(BlockMapLock);
              Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&BlockMapBuf), getpagesize(), BlockMapByteSize));
              Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&DiscardMapBuf), getpagesize(), BlockMapByteSize));
              Base::IfLt0(mlock(BlockMapBuf, BlockMapByteSize));
              Base::IfLt0(mlock(DiscardMapBuf, BlockMapByteSize));
              memset(BlockMapBuf, 0, BlockMapByteSize);
              memset(DiscardMapBuf, 0, BlockMapByteSize);
            } catch (...) {
              free(DiscardMapBuf);
              DiscardMapBuf = nullptr;
              free(BlockMapBuf);
              BlockMapBuf = nullptr;
              throw;
            }
          }

          void PostCtor() {
            assert(this);
            Scheduler->Schedule(bind(&TVolume::TStrategy::DiscardRunner, this));
          }

          /* TODO */
          template <typename... TArgs>
          TGroupRequest *NewGroupRequest(size_t total_num_request, TArgs &...args);

          /* TODO */
          template <typename... TArgs>
          void Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, size_t device_num,
                     const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, const TOffset logical_start_offset,
                     TArgs &...args);

          /* TODO */
          template <typename... TArgs>
          void Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, size_t device_num,
                    const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

          /* TODO */
          template <typename... TArgs>
          void SubmitRequest(size_t device_num, const TDeviceRequest &device_request, TGroupRequest *group_request,
                             const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                             DiskPriority priority, bool abort_on_error, TArgs &...args);

          /* TODO */
          void AppendTouchedDevicesToSet(TDeviceSet &device_set, size_t device_num) const;

          /* Used for debugging. Verify that we have this block reserved. */
          inline bool CheckBufBlock(size_t block_id) const;
          inline bool CheckDiscardBuf(size_t block_id) const;

          /* TODO */
          TVolume *Volume;

          /* TODO */
          std::vector<TLogicalExtent> ExtentVec;

          /* TODO */
          std::vector<TDeviceSet> DeviceVec;

          //private:

          /* TODO */
          inline size_t GetBlock();

          /* TODO */
          size_t TryGetSeqBlocks(const size_t num, size_t &start_out);

          /* TODO */
          inline void PutBack(size_t block_id);

          /* TODO */
          inline size_t ReserveBlock(std::unique_lock<std::mutex> &lock);

          /* TODO */
          inline size_t TryReserveSeqBlocks(std::unique_lock<std::mutex> &lock, const size_t num_blocks, size_t &out_start);

          /* Set an individual bit to (on / off) */
          inline void SetBlockBuf(size_t block_id, bool on);

          /* Set a segment (8 * sizeof(size_t)) of bits to (on / off). The starting block id must be a multiple of the segment size. */
          inline void SetBlockBufSegment(size_t starting_block_id, bool on);

          /* Set a range of bits to (on / off) */
          inline void SetBlockBufRange(size_t block_id, const size_t num_blocks, const bool on);

          /* TODO */
          inline void SetDiscardRange(const size_t block_id, const size_t num_blocks, const bool on);

          /* TODO */
          inline void SetDiscardBuf(size_t block_id, bool on);

          /* TODO */
          virtual size_t GetBlockMapBytesPerDiscardRange() const = 0;

          /* TODO */
          virtual void DoDiscard(const TBlockRange &block_range) const = 0;

          /* TODO */
          inline void CheckRange(const TOffset start_offset, long long nbytes, const TDevice *device) {
            assert(this);
            if (unlikely(start_offset + SuperBytes + nbytes > device->Desc.Capacity)) {
              throw std::logic_error("Accessing range on device past capacity");
            }
          }

          /* TODO */
          void DiscardRunner();

          Base::TScheduler *Scheduler;

          /* TODO */
          size_t *BlockMapBuf;
          std::mutex BlockMapLock;
          size_t CachedStart;
          size_t *DiscardMapBuf;
          std::mutex DiscardMapLock;

          /* TODO */
          size_t BlocksUsed;

          /* The number of blocks waiting to be discarded */
          size_t DiscardBlockWaiting;

          /* TODO */
          Base::TFd DiscardEpollFd;
          Base::TEventSemaphore DiscardSem;
          epoll_event DiscardEvent;
          bool DiscardRan;
          std::condition_variable DiscardCond;

          /* TODO */
          const size_t SuperBytes;

          /* TODO */
          const size_t NumBlocks;
          const size_t NumBlocksPerExtent;
          const size_t BlockMapByteSize;
          const size_t BlockMapBufByteSize;

        };

        /* TODO */
        template <typename... TArgs>
        TGroupRequest *TVolume::TStrategy::NewGroupRequest(size_t /*total_num_request*/, TArgs &.../*args*/) {
          throw std::logic_error("Should not be reached");
        }

        /* TODO */
        template <>
        TGroupRequest *TVolume::TStrategy::NewGroupRequest<Stig::Indy::Disk::TCompletionTrigger>(size_t /*total_num_request*/,
                                                                                                 TCompletionTrigger &/*trigger*/) {
          return nullptr;
        }

        /* TODO */
        template <>
        TGroupRequest *TVolume::TStrategy::NewGroupRequest<Stig::Indy::Disk::TCompletionTrigger, const TIOCallback>(size_t total_num_request,
                                                                                                                    TCompletionTrigger &trigger,
                                                                                                                    const TIOCallback &cb) {
          trigger.WaitForOneMore();
          return new TGroupRequest(total_num_request, cb);
        }

        template <typename... TArgs>
        void TVolume::TStrategy::Write(const Base::TCodeLocation &/*code_location*/ /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/,
                                       void */*buf*/, size_t /*device_num*/, const TOffset /*start_offset*/, long long /*nbytes*/,
                                       DiskPriority /*priority*/, bool /*abort_on_error*/ , const TOffset /*logical_start_offset*/,
                                       TArgs &.../*args*/) {
          throw std::logic_error("Should not be reached");
        }

        template <>
        void TVolume::TStrategy::Write<Stig::Indy::Disk::TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                             uint8_t util_src, void *buf, size_t device_num,
                                                                             const TOffset start_offset, long long nbytes, DiskPriority priority,
                                                                             bool abort_on_error, const TOffset logical_start_offset,
                                                                             TCompletionTrigger &trigger) {
          assert(this);
          assert(device_num < DeviceVec.size());
          const TDeviceSet &dev_set = DeviceVec[device_num];
          trigger.WaitForMore(dev_set.size());
          for (auto device : dev_set) {
            assert(start_offset + nbytes <= device->Desc.Capacity);
            device->Write(code_location, buf_kind, util_src, buf, start_offset + SuperBytes, nbytes, priority, abort_on_error, logical_start_offset,
                          trigger);
          }
        }

        template <>
        void TVolume::TStrategy::Write<Stig::Indy::Disk::TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */,
                                                                                                TBufKind buf_kind, uint8_t util_src, void *buf,
                                                                                                size_t device_num, const TOffset start_offset,
                                                                                                long long nbytes, DiskPriority priority,
                                                                                                bool abort_on_error,
                                                                                                const TOffset logical_start_offset,
                                                                                                TCompletionTrigger &trigger, const TIOCallback &cb) {
          assert(this);
          assert(device_num < DeviceVec.size());
          const TDeviceSet &dev_set = DeviceVec[device_num];
          trigger.WaitForMore(dev_set.size());
          for (auto device : dev_set) {
            assert(start_offset + nbytes <= device->Desc.Capacity);
            device->Write(code_location, buf_kind, util_src, buf, start_offset + SuperBytes, nbytes, priority, abort_on_error, logical_start_offset,
                          cb);
          }
        }

        template <typename... TArgs>
        void TVolume::TStrategy::Read(const Base::TCodeLocation &/*code_location*/ /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/,
                                      void */*buf*/, size_t /*device_num*/, const TOffset /*start_offset*/, long long /*nbytes*/,
                                      DiskPriority /*priority*/, bool /*abort_on_error*/, TArgs &.../*args*/) {
          throw std::logic_error("Should not be reached");
        }

        template <>
        void TVolume::TStrategy::Read<Stig::Indy::Disk::TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                            uint8_t util_src, void *buf, size_t device_num,
                                                                            const TOffset start_offset, long long nbytes, DiskPriority priority,
                                                                            bool abort_on_error, TCompletionTrigger &trigger) {
          assert(this);
          assert(device_num < DeviceVec.size());
          const TDeviceSet &dev_set = DeviceVec[device_num];
          trigger.WaitForOneMore();
          auto device = *dev_set.begin(); /* TODO: we can be smarter about choosing which device to read from */
          CheckRange(start_offset, nbytes, device);
          assert(start_offset + SuperBytes + nbytes <= device->Desc.Capacity);
          device->Read(code_location, buf_kind, util_src, buf, start_offset + SuperBytes, nbytes, priority, abort_on_error, trigger);
        }

        template <>
        void TVolume::TStrategy::Read<Stig::Indy::Disk::TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */,
                                                                                               TBufKind buf_kind, uint8_t util_src, void *buf,
                                                                                               size_t device_num, const TOffset start_offset,
                                                                                               long long nbytes, DiskPriority priority,
                                                                                               bool abort_on_error, TCompletionTrigger &trigger,
                                                                                               const TIOCallback &cb) {
          assert(this);
          assert(device_num < DeviceVec.size());
          const TDeviceSet &dev_set = DeviceVec[device_num];
          trigger.WaitForOneMore();
          auto device = *dev_set.begin(); /* TODO: we can be smarter about choosing which device to read from */
          CheckRange(start_offset, nbytes, device);
          assert(start_offset + SuperBytes + nbytes <= device->Desc.Capacity);
          device->Read(code_location, buf_kind, util_src, buf, start_offset + SuperBytes, nbytes, priority, abort_on_error, cb);
        }

        template <typename... TArgs>
        void TVolume::TStrategy::SubmitRequest(size_t /*device_num*/, const TDeviceRequest &/*device_request*/, TGroupRequest */*group_request*/,
                                               const Base::TCodeLocation &/*code_location*/ /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/,
                                               DiskPriority /*priority*/, bool /*abort_on_error*/, TArgs &.../*args*/) {
          throw std::logic_error("Should not be reached");
        }

        template <>
        void TVolume::TStrategy::SubmitRequest<Stig::Indy::Disk::TCompletionTrigger>(size_t device_num, const TDeviceRequest &device_request,
                                                                                     TGroupRequest */*group_request*/,
                                                                                     const Base::TCodeLocation &code_location /* DEBUG */,
                                                                                     TBufKind buf_kind, uint8_t util_src, DiskPriority priority,
                                                                                     bool abort_on_error, TCompletionTrigger &trigger) {
          assert(this);
          assert(device_num < DeviceVec.size());
          const TDeviceSet &dev_set = DeviceVec[device_num];
          auto device = *dev_set.begin(); /* TODO: we can be smarter about choosing which device to read from */
          CheckRange(device_request.PhysicalOffsetStart, device_request.TotalBytes, device);
          assert(device_request.PhysicalOffsetStart + SuperBytes + device_request.TotalBytes <= device->Desc.Capacity);
          size_t bytes_in = 0UL;
          const size_t bytes_per_segment = device_request.TotalBytes / device_request.NumReq;
          for (const auto &buf_vec : device_request.VecPerOp) {
            trigger.WaitForOneMore();
            const size_t num_bytes_in_op = buf_vec.size() * bytes_per_segment;
            device->ReadV(code_location, buf_kind, util_src, buf_vec, device_request.PhysicalOffsetStart + SuperBytes + bytes_in, num_bytes_in_op,
                          priority, abort_on_error, trigger);
            bytes_in += num_bytes_in_op;
          }
        }

        template <>
        void TVolume::TStrategy::SubmitRequest<Stig::Indy::Disk::TCompletionTrigger, const TIOCallback>(size_t device_num,
                                                                                                        const TDeviceRequest &device_request,
                                                                                                        TGroupRequest *group_request,
                                                                                                        const Base::TCodeLocation &code_location /* DEBUG */,
                                                                                                        TBufKind buf_kind, uint8_t util_src,
                                                                                                        DiskPriority priority, bool abort_on_error,
                                                                                                        TCompletionTrigger &/*trigger*/,
                                                                                                        const TIOCallback &/*cb*/) {
          assert(this);
          assert(device_num < DeviceVec.size());
          const TDeviceSet &dev_set = DeviceVec[device_num];
          auto device = *dev_set.begin(); /* TODO: we can be smarter about choosing which device to read from */
          CheckRange(device_request.PhysicalOffsetStart, device_request.TotalBytes, device);
          assert(device_request.PhysicalOffsetStart + SuperBytes + device_request.TotalBytes <= device->Desc.Capacity);
          size_t bytes_in = 0UL;
          const size_t bytes_per_segment = device_request.TotalBytes / device_request.NumReq;
          for (const auto &buf_vec : device_request.VecPerOp) {
            const size_t num_bytes_in_op = buf_vec.size() * bytes_per_segment;
            device->ReadV(code_location, buf_kind, util_src, buf_vec, device_request.PhysicalOffsetStart + SuperBytes + bytes_in, num_bytes_in_op, priority, abort_on_error, group_request);
            bytes_in += num_bytes_in_op;
          }
        }

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig

inline size_t TVolume::TStrategy::GetBlock() {
  assert(this);
  size_t *buf_ref = BlockMapBuf + CachedStart;
  constexpr size_t full = -1;
  for (size_t i = CachedStart; i < BlockMapByteSize / sizeof(size_t); ++i, ++buf_ref) {
    if (*buf_ref != full) {
      size_t x = *buf_ref;
      for (size_t j = 0; j < 64; ++j) {
        if ((x >> j) % 2 == 0) {
          const size_t block_id = (i * 64) + j;
          if (block_id < NumBlocks) {
            SetBlockBuf(block_id, true);
            return block_id;
          }
        }
      }
    }
  }
  /* scan from the beginning */
  buf_ref = BlockMapBuf;
  for (size_t i = 0; i < CachedStart; ++i, ++buf_ref) {
    if (*buf_ref != full) {
      size_t x = *buf_ref;
      for (size_t j = 0; j < 64; ++j) {
        if ((x >> j) % 2 == 0) {
          const size_t block_id = (i * 64) + j;
          if (block_id < NumBlocks) {
            SetBlockBuf(block_id, true);
            return block_id;
          }
        }
      }
    }
  }
  throw std::runtime_error("Could not find block on disk. Out of disk space to provide a reserve block.");
}

size_t TVolume::TStrategy::TryGetSeqBlocks(const size_t num_blocks, size_t &start_out) {
  assert(this);
  size_t *buf_ref = BlockMapBuf + CachedStart;
  constexpr size_t full = -1;
  constexpr size_t empty = 0UL;
  constexpr size_t num_per_seg = 8 * sizeof(size_t);
  size_t cur_alloced = 0UL;
  for (size_t i = CachedStart; i < BlockMapByteSize / sizeof(size_t) && cur_alloced < num_blocks; ++i, ++buf_ref) {
    if (*buf_ref == empty && num_blocks - cur_alloced >= num_per_seg && likely(((i + 1) * num_per_seg) <= NumBlocks)) {
      const size_t block_id = (i * num_per_seg);
      if (cur_alloced == 0UL) { /* first entry */
        SetBlockBufSegment(block_id, true);
        start_out = block_id;
        cur_alloced += num_per_seg;
      } else if (start_out + cur_alloced == block_id) {
        SetBlockBufSegment(block_id, true);
        cur_alloced += num_per_seg;
      } else {
        assert(cur_alloced > 0UL);
        return cur_alloced;
      }
    } else if (*buf_ref != full) {
      size_t x = *buf_ref;
      for (size_t j = 0; j < 64 && cur_alloced < num_blocks; ++j) {
        if ((x >> j) % 2 == 0) {
          const size_t block_id = (i * num_per_seg) + j;
          if (likely(block_id < NumBlocks)) {
            if (cur_alloced == 0UL) { /* first entry */
              SetBlockBuf(block_id, true);
              start_out = block_id;
              ++cur_alloced;
            } else if (start_out + cur_alloced == block_id) {
              SetBlockBuf(block_id, true);
              ++cur_alloced;
            } else {
              assert(cur_alloced > 0UL);
              return cur_alloced;
            }
          }
        }
      }
    }
  }
  if (cur_alloced > 0UL) {
    return cur_alloced;
  }
  /* scan from the beginning */
  buf_ref = BlockMapBuf;
  for (size_t i = 0; i < CachedStart; ++i, ++buf_ref) {
    if (*buf_ref == empty && num_blocks - cur_alloced >= num_per_seg && likely(((i + 1) * num_per_seg) <= NumBlocks)) {
      const size_t block_id = (i * num_per_seg);
      if (cur_alloced == 0UL) { /* first entry */
        SetBlockBufSegment(block_id, true);
        start_out = block_id;
        cur_alloced += num_per_seg;
      } else if (start_out + cur_alloced == block_id) {
        SetBlockBufSegment(block_id, true);
        cur_alloced += num_per_seg;
      } else {
        assert(cur_alloced > 0UL);
        return cur_alloced;
      }
    } else if (*buf_ref != full) {
      size_t x = *buf_ref;
      for (size_t j = 0; j < 64 && cur_alloced < num_blocks; ++j) {
        if ((x >> j) % 2 == 0) {
          const size_t block_id = (i * num_per_seg) + j;
          if (likely(block_id < NumBlocks)) {
            if (cur_alloced == 0UL) { /* first entry */
              SetBlockBuf(block_id, true);
              start_out = block_id;
              ++cur_alloced;
            } else if (start_out + cur_alloced == block_id) {
              SetBlockBuf(block_id, true);
              ++cur_alloced;
            } else {
              assert(cur_alloced > 0UL);
              return cur_alloced;
            }
          }
        }
      }
    }
  }
  return cur_alloced;
}

inline void TVolume::TStrategy::PutBack(size_t block_id) {
  assert(this);
  SetBlockBuf(block_id, false);
}

inline size_t TVolume::TStrategy::ReserveBlock(std::unique_lock<std::mutex> &lock) {
  assert(this);
  for (;;) {
    try {
      return GetBlock();
    } catch (const std::exception &ex) {
      if (DiscardBlockWaiting > 0) {
        DiscardRan = false;
        DiscardSem.Push();
        while (!DiscardRan) {
          DiscardCond.wait(lock);
        }
        continue;
      } else {
        syslog(LOG_EMERG, "%s", ex.what());
        throw;
      }
    }
  }
  throw;
}

inline size_t TVolume::TStrategy::TryReserveSeqBlocks(std::unique_lock<std::mutex> &lock, const size_t num_blocks, size_t &out_start) {
  assert(this);
  for (;;) {
    size_t alloced = TryGetSeqBlocks(num_blocks, out_start);
    if (unlikely(alloced == 0)) {
      if (DiscardBlockWaiting > 0) {
        DiscardRan = false;
        DiscardSem.Push();
        while (!DiscardRan) {
          DiscardCond.wait(lock);
        }
        continue;
      } else {
        syslog(LOG_EMERG, "out of disk space");
        abort();
      }
    } else {
      return alloced;
    }
  }
  throw;
}

inline void TVolume::TStrategy::SetBlockBuf(size_t block_id, bool on) {
  assert(this);
  if (unlikely(block_id >= NumBlocks)) {
    syslog(LOG_EMERG, "Out of disk space.");
    throw std::runtime_error("Out of disk space.");
  }
  constexpr size_t num_per_seg = 8UL * sizeof(size_t);
  size_t index = block_id / num_per_seg;
  size_t *offset = BlockMapBuf + index;
  size_t pos = block_id % num_per_seg;
  size_t u = 1;
  size_t mask = u << pos;
  if (on) {
    assert(!CheckBufBlock(block_id));
    *offset |= mask;
    assert(CheckBufBlock(block_id));
    CachedStart = block_id < (NumBlocks - 1) ? index : 0;
    ++BlocksUsed;
  } else {
    assert(CheckBufBlock(block_id));
    *offset &= ~mask;
    assert(!CheckBufBlock(block_id));
    --BlocksUsed;
  }
}

inline void TVolume::TStrategy::SetBlockBufSegment(size_t starting_block_id, bool on) {
  assert(this);
  constexpr size_t num_per_seg = 8UL * sizeof(size_t);
  assert(starting_block_id % num_per_seg == 0UL);
  if (unlikely(starting_block_id + num_per_seg > NumBlocks)) {
    syslog(LOG_EMERG, "Out of disk space.");
    throw std::runtime_error("Out of disk space.");
  }
  size_t index = starting_block_id / num_per_seg;
  size_t *offset = BlockMapBuf + index;
  if (on) {
    assert(*offset == 0UL);
    *offset = -1UL;
    assert(*offset == -1UL);
    CachedStart = (starting_block_id + num_per_seg) < (NumBlocks) ? (index + 1UL) : 0;
    BlocksUsed += num_per_seg;
  } else {
    assert(*offset == -1UL);
    *offset = 0UL;
    assert(*offset == 0UL);
    BlocksUsed -= num_per_seg;
  }
}

inline void TVolume::TStrategy::SetBlockBufRange(size_t block_id, const size_t num_blocks, const bool on) {
  assert(this);
  assert(block_id < NumBlocks);
  const size_t end = block_id + num_blocks;
  constexpr size_t num_per_seg = sizeof(size_t) * 8UL;
  if (unlikely(end > NumBlocks)) {
    syslog(LOG_EMERG, "Out of disk space.");
    throw std::runtime_error("Out of disk space.");
  }
  size_t pos = block_id;
  for (; pos < end && pos % num_per_seg != 0; ++pos) {
    SetBlockBuf(pos, on);
  }
  const size_t index = pos / num_per_seg;
  size_t *offset = BlockMapBuf + index;
  for (; (pos < end) && ((end - pos) >= num_per_seg); pos += num_per_seg, ++offset) {
    assert(pos % num_per_seg == 0UL);
    #ifndef NDEBUG
    for (size_t i = 0; i < num_per_seg; ++i) {
      assert(CheckBufBlock(pos + i) == !on);
    }
    #endif
    if (on) {
      *offset = -1UL;
      BlocksUsed += num_per_seg;
    } else {
      *offset = 0UL;
      BlocksUsed -= num_per_seg;
    }
    #ifndef NDEBUG
    for (size_t i = 0; i < num_per_seg; ++i) {
      assert(CheckBufBlock(pos + i) == on);
    }
    #endif
  }
  for (; pos < end; ++pos) {
    SetBlockBuf(pos, on);
  }
  assert(pos == end);
}

inline void TVolume::TStrategy::SetDiscardRange(const size_t block_id, const size_t num_blocks, const bool on) {
  assert(this);
  assert(block_id < NumBlocks);
  const size_t end = block_id + num_blocks;
  constexpr size_t num_per_seg = sizeof(size_t) * 8UL;
  size_t pos = block_id;
  for (; pos < end && pos % num_per_seg != 0; ++pos) {
    SetDiscardBuf(pos, on);
  }
  const size_t index = pos / num_per_seg;
  size_t *offset = DiscardMapBuf + index;
  for (; (pos < end) && ((end - pos) >= num_per_seg); pos += num_per_seg, ++offset) {
    assert(pos % num_per_seg == 0UL);
    #ifndef NDEBUG
    for (size_t i = 0; i < num_per_seg; ++i) {
      assert(CheckDiscardBuf(pos + i) == !on);
    }
    #endif
    if (on) {
      *offset = -1UL;
      DiscardBlockWaiting += num_per_seg;
    } else {
      *offset = 0UL;
      DiscardBlockWaiting -= num_per_seg;
    }
    #ifndef NDEBUG
    for (size_t i = 0; i < num_per_seg; ++i) {
      assert(CheckDiscardBuf(pos + i) == on);
    }
    #endif
  }
  for (; pos < end; ++pos) {
    SetDiscardBuf(pos, on);
  }
  assert(pos == end);
}

inline void TVolume::TStrategy::SetDiscardBuf(size_t block_id, bool on) {
  assert(this);
  assert(block_id < NumBlocks);
  constexpr size_t num_per_seg = 8UL * sizeof(size_t);
  size_t index = block_id / num_per_seg;
  size_t *offset = DiscardMapBuf + index;
  size_t pos = block_id % num_per_seg;
  size_t u = 1;
  size_t mask = u << pos;
  if (on) {
    assert(!CheckDiscardBuf(block_id));
    *offset |= mask;
    assert(CheckDiscardBuf(block_id));
    ++DiscardBlockWaiting;
  } else {
    assert(CheckDiscardBuf(block_id));
    *offset &= ~mask;
    assert(!CheckDiscardBuf(block_id));
    --DiscardBlockWaiting;
  }
}

inline bool TVolume::TStrategy::CheckBufBlock(size_t block_id) const {
  assert(this);
  assert(block_id < NumBlocks);
  constexpr size_t num_per_seg = 8UL * sizeof(size_t);
  size_t index = block_id / num_per_seg;
  size_t *offset = BlockMapBuf + index;
  size_t pos = block_id % num_per_seg;
  size_t u = 1;
  size_t mask = u << pos;
  return (*offset & mask) == mask;
}

inline bool TVolume::TStrategy::CheckDiscardBuf(size_t block_id) const {
  assert(this);
  assert(block_id < NumBlocks);
  constexpr size_t num_per_seg = 8UL * sizeof(size_t);
  size_t index = block_id / num_per_seg;
  size_t *offset = DiscardMapBuf + index;
  size_t pos = block_id % num_per_seg;
  size_t u = 1;
  size_t mask = u << pos;
  return (*offset & mask) == mask;
}

/* TODO */
class TVolume::TStripedStrategy
    : public TVolume::TStrategy {
  NO_COPY_SEMANTICS(TStripedStrategy);
  public:

  /* TODO */
  TStripedStrategy(TVolume *volume, Base::TScheduler *scheduler)
      : TStrategy(volume, scheduler) {
    PostCtor();
  }

  /* TODO */
  virtual ~TStripedStrategy() {}

  /* TODO */
  virtual void DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger) override;
  virtual void DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger,
                             const TIOCallback &cb) override;

  /* TODO */
  virtual void DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                            TCompletionTrigger &trigger) override;
  virtual void DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger,
                            const TIOCallback &cb) override;

  /* TODO */
  virtual void DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                             size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger) override;
  virtual void DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                             size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger, const TIOCallback &cb) override;

  /* TODO */
  virtual void DelegateAppendTouchedDevicesToSet(TDeviceSet &device_set, const TBlockRange &block_range) const override;

  /* TODO */
  virtual size_t GetBlockMapBytesPerDiscardRange() const override;

  /* TODO */
  virtual void DoDiscard(const TBlockRange &block_range) const override;

  private:

  /* TODO */
  enum TOp {R, W};

  /* TODO */
  template <typename... TArgs>
  void DoStripe(TOp op, const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

  /* TODO */
  template <typename... TArgs>
  void DoStripeV(TOp op, const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array, size_t num_buf,
                 const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

};  // TStripedStrategy

/* TODO */
class TVolume::TChainedStrategy
    : public TVolume::TStrategy {
  NO_COPY_SEMANTICS(TChainedStrategy);
  public:

  /* TODO */
  TChainedStrategy(TVolume *volume, Base::TScheduler *scheduler)
      : TStrategy(volume, scheduler) {
    PostCtor();
  }

  /* TODO */
  virtual ~TChainedStrategy() {}

  /* TODO */
  virtual void DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger) override;
  virtual void DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger,
                             const TIOCallback &cb) override;

  /* TODO */
  virtual void DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                            TCompletionTrigger &trigger) override;
  virtual void DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger,
                            const TIOCallback &cb) override;

  /* TODO */
  virtual void DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                             size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger) override;
  virtual void DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                             size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                             TCompletionTrigger &trigger, const TIOCallback &cb) override;

  /* TODO */
  virtual void DelegateAppendTouchedDevicesToSet(TDeviceSet &device_set, const TBlockRange &block_range) const override;

  /* TODO */
  virtual size_t GetBlockMapBytesPerDiscardRange() const override;

  /* TODO */
  virtual void DoDiscard(const TBlockRange &block_range) const override;

  private:

  /* TODO */
  enum TOp {R, W};

  /* TODO */
  template <typename... TArgs>
  void DoChain(TOp op, const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
               const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args);

};  // TChainedStrategy

void TVolume::TStrategy::AppendTouchedDevicesToSet(TDeviceSet &device_set, size_t device_num) const {
  assert(this);
  assert(device_num < DeviceVec.size());
  const TDeviceSet &my_dev_set = DeviceVec[device_num];
  /* can use insert(begin, end) if we're sure it tries to insert all elements even if the first one is rejected due to key collision... */
  for (TDevice *dev : my_dev_set) {
    device_set.insert(dev);
  }
}

std::pair<size_t, size_t> TVolume::TStrategy::AppendUsage(std::stringstream &ss) const {
  assert(this);
  const size_t bytes_used = BlocksUsed * Util::PhysicalBlockSize;
  const size_t total_bytes = NumBlocks * Util::PhysicalBlockSize;
  ss << "Volume_" << Volume->GetVolumeId() << " = " << bytes_used << " / " << total_bytes << std::endl;
  return make_pair(bytes_used, total_bytes);
}

inline void TVolume::TStrategy::TryAllocateSequentialBlocks(size_t num_blocks, const std::function<void (const TBlockRange &block_range)> &cb) {
  assert(this);
  assert(num_blocks > 0);
  std::unique_lock<std::mutex> lock(BlockMapLock);
  size_t init_block = -1UL;
  const size_t total_alloced = TryReserveSeqBlocks(lock, num_blocks, init_block);
  const size_t logical_extent_num = init_block / NumBlocksPerExtent;
  assert(ExtentVec[logical_extent_num].Start % PhysicalBlockSize == 0UL);
  const size_t start_block = (init_block % NumBlocksPerExtent) + (ExtentVec[logical_extent_num].Start / PhysicalBlockSize);
  const size_t start_offset = start_block * PhysicalBlockSize;
  const size_t blocks_to_give = std::min(total_alloced, (ExtentVec[logical_extent_num].Span - (start_offset - ExtentVec[logical_extent_num].Start)) / PhysicalBlockSize);
  size_t left_to_give = total_alloced - blocks_to_give;
  #ifndef NDEBUG
  size_t actually_provided_blocks = blocks_to_give;
  auto check_range = [this](size_t start_block, size_t num_blocks) -> bool {
    bool found_extent = false;
    const size_t start_offset = start_block * PhysicalBlockSize;
    const size_t end_block = (start_block + num_blocks) - 1;
    const size_t end_offset = (end_block + 1) * PhysicalBlockSize;
    for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
      const TLogicalExtent &extent = ExtentVec[extent_num];
      if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
        found_extent = true;
      }
    }
    return found_extent;
  };
  #endif
  assert(check_range(start_block, blocks_to_give));
  cb(make_pair(start_block, blocks_to_give));
  for (size_t i = logical_extent_num + 1UL; left_to_give && i < ExtentVec.size(); ++i) {
    const size_t num_blocks_in_extent = ExtentVec[i].Span / PhysicalBlockSize;
    const size_t num_to_give = std::min(left_to_give, num_blocks_in_extent);
    left_to_give -= num_to_give;
    assert(check_range(ExtentVec[i].Start / PhysicalBlockSize, num_to_give));
    cb(make_pair(ExtentVec[i].Start / PhysicalBlockSize, num_to_give));
    #ifndef NDEBUG
    actually_provided_blocks += num_to_give;
    #endif
  }
  #ifndef NDEBUG
  assert(actually_provided_blocks == total_alloced);
  #endif
}
void TVolume::TStrategy::FreeSequentialBlocks(const TBlockRange &block_range) {
  assert(this);
  const size_t &starting_block = block_range.first;
  const size_t &num_seq_blocks = block_range.second;
  const size_t start_offset = starting_block * PhysicalBlockSize;
  const size_t end_offset = (starting_block + num_seq_blocks) * PhysicalBlockSize;
  /* acquire discard lock */ {
    std::lock_guard<std::mutex> lock(DiscardMapLock);

    for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
      const TLogicalExtent &extent = ExtentVec[extent_num];
      if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
        assert((start_offset - extent.Start) % PhysicalBlockSize == 0);
        size_t block_to_free = ((start_offset - extent.Start) / PhysicalBlockSize) + (extent_num * NumBlocksPerExtent);
        SetDiscardRange(block_to_free, num_seq_blocks, true);
        return;
      }
    }
    syslog(LOG_ERR, "Trying to free block range from [%ld] for [%ld] blocks, not supported accross multiple logical extents", block_range.first, block_range.second);
    throw std::logic_error("Free Sequential Blocks not supported accross multiple logical extents");
  }  // release discard lock
}

void TVolume::TStrategy::MarkBlockRangeUsed(const TBlockRange &block_range) {
  assert(this);
  const size_t &starting_block = block_range.first;
  const size_t &num_seq_blocks = block_range.second;
  const size_t start_offset = starting_block * PhysicalBlockSize;
  const size_t end_offset = (starting_block + num_seq_blocks) * PhysicalBlockSize;
  /* acquire block lock */ {
    std::lock_guard<std::mutex> lock(BlockMapLock);

    for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
      const TLogicalExtent &extent = ExtentVec[extent_num];
      if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
        assert((start_offset - extent.Start) % PhysicalBlockSize == 0);
        size_t block_to_set = ((start_offset - extent.Start) / PhysicalBlockSize) + (extent_num * NumBlocksPerExtent);
        for (size_t i = 0; i < num_seq_blocks; ++i, ++block_to_set) {
          if (CheckBufBlock(block_to_set)) {
            syslog(LOG_ERR, "Trying to mark block [%ld] as used in volume [%ld] which is already in use. Corruption error.", block_to_set, Volume->GetVolumeId());
            throw std::runtime_error("Trying to mark block as used which is already in use. Corruption error.");
          }
          SetBlockBuf(block_to_set, true);
        }
        return;
      }
    }
    syslog(LOG_ERR, "Trying to mark block range used from [%ld] for [%ld] blocks, not supported accross multiple logical extents", block_range.first, block_range.second);
    throw std::logic_error("MarkBlockRangeUsed not supported accross multiple logical extents");
  }  // release block lock
}

void TVolume::TStrategy::DiscardRunner() {
  assert(this);
  constexpr size_t empty = 0UL;
  constexpr size_t num_blocks_per_buf = sizeof(size_t) * 8;
  const size_t num_devices = Volume->GetDesc().NumLogicalExtent;
  const size_t min_discard_blocks = Volume->GetDesc().MinDiscardAllocBlocks;

  const size_t bytes_of_block_map_per_discard_range = std::min(GetBlockMapBytesPerDiscardRange(), static_cast<size_t>(ceil(static_cast<double>(min_discard_blocks) / 8UL)));
  uint8_t *const FullDiscard = reinterpret_cast<uint8_t *>(alloca(bytes_of_block_map_per_discard_range));
  memset(FullDiscard, 255, bytes_of_block_map_per_discard_range);
  for (size_t i = 0; i < bytes_of_block_map_per_discard_range; ++i) {
    assert(FullDiscard[i] == 255);
  }
  std::vector<std::pair<size_t, size_t>> blocks_from_vec;
  DiscardEpollFd = epoll_create1(0);
  Zero(DiscardEvent);
  DiscardEvent.events = EPOLLIN;
  DiscardEvent.data.fd = DiscardSem.GetFd();
  IfLt0(epoll_ctl(DiscardEpollFd, EPOLL_CTL_ADD, DiscardSem.GetFd(), &DiscardEvent));
  epoll_event event;
  int timeout = 30000;
  for (;;) {
    for (;;) {
      int ret = epoll_wait(DiscardEpollFd, &event, 1, timeout);
      if (ret < 0 && errno == EINTR) {
        if (Base::ShuttingDown) {
          throw std::runtime_error("Discard() runner shutting down.");
        } else {
          continue;
        }
      } else {
        Base::IfLt0(ret);
        assert(ret < 2);
        if (ret == 1) {
          DiscardSem.Pop();
        }
        break;
      }
    }
    assert(blocks_from_vec.size() == 0);
    /* acquire Discard lock */ {
      std::lock_guard<std::mutex> lock(DiscardMapLock);
      if (DiscardBlockWaiting > min_discard_blocks) {
        uint8_t *buf_ref = reinterpret_cast<uint8_t *>(DiscardMapBuf);
        assert(((num_devices * NumBlocksPerExtent) / 8UL) % bytes_of_block_map_per_discard_range == 0UL);
        const size_t max_iter = ((num_devices * NumBlocksPerExtent) / 8UL) / bytes_of_block_map_per_discard_range;
        for (size_t i = 0; i < max_iter; ++i, buf_ref += bytes_of_block_map_per_discard_range) {
          if (memcmp(FullDiscard, buf_ref, bytes_of_block_map_per_discard_range) == 0) {
            const size_t starting_block_id = i * bytes_of_block_map_per_discard_range * 8UL;
            blocks_from_vec.emplace_back(starting_block_id, bytes_of_block_map_per_discard_range * 8UL);
          }
        }
      }
      if (blocks_from_vec.empty() && (static_cast<double>(BlocksUsed) / NumBlocks) > Volume->GetDesc().HighUtilizationThreshold && DiscardBlockWaiting > 0UL) { /* our disk is getting close to full, start passing discard blocks to the free workset */
        std::lock_guard<std::mutex> block_lock(BlockMapLock);
        size_t *buf_ref = DiscardMapBuf;
        for (size_t i = 0; i < BlockMapByteSize / sizeof(size_t); ++i, ++buf_ref) {
          if (*buf_ref != empty) {
            size_t x = *buf_ref;
            for (size_t j = 0; j < num_blocks_per_buf; ++j) {
              if ((x >> j) % 2 == 1) {
                const size_t block_to_free = (i * num_blocks_per_buf) + j;
                SetDiscardBuf(block_to_free, false);
                SetBlockBuf(block_to_free, false);
              }
            }
          }
        }
      }
    }  // release Discard lock
    for (const auto &range : blocks_from_vec) {
      assert(range.second % min_discard_blocks == 0);
      assert(range.second >= min_discard_blocks);
      DoDiscard(range);
      std::lock_guard<std::mutex> discard_lock(DiscardMapLock);
      std::lock_guard<std::mutex> map_lock(BlockMapLock);
      SetDiscardRange(range.first, range.second, false);
      SetBlockBufRange(range.first, range.second, false);
    }
    blocks_from_vec.clear();
    /* acquire DiscardRan lock */ {
      std::lock_guard<std::mutex> lock(BlockMapLock);
      DiscardRan = true;
    }  // release DiscardRan lock
    DiscardCond.notify_one();
  }
}

size_t TVolume::TStripedStrategy::GetBlockMapBytesPerDiscardRange() const {
  assert(this);
  const size_t num_logical_block_per_stripe = Volume->GetDesc().NumLogicalBlockPerStripe;
  const size_t logical_block_size = Volume->GetDesc().DeviceDesc.LogicalBlockSize;
  const size_t bytes_per_stripe = num_logical_block_per_stripe * logical_block_size;
  const size_t phys_blocks_per_stripe = bytes_per_stripe / PhysicalBlockSize;
  if (unlikely(phys_blocks_per_stripe % 8UL != 0)) {
    throw std::runtime_error("Stripe factor must be a multiple of 8 Stig blocks.");
  }
  return phys_blocks_per_stripe / 8UL;
}

size_t TVolume::TChainedStrategy::GetBlockMapBytesPerDiscardRange() const {
  assert(this);
  if (unlikely(NumBlocksPerExtent % 8UL != 0)) {
    throw std::runtime_error("Extent size must be a multiple of 8 Stig blocks.");
  }
  return NumBlocksPerExtent / 8UL;
}

void TVolume::TStripedStrategy::DoDiscard(const TBlockRange &block_range) const {
  assert(this);
  const size_t num_devices = Volume->GetDesc().NumLogicalExtent;
  const size_t num_logical_block_per_stripe = Volume->GetDesc().NumLogicalBlockPerStripe;
  const size_t logical_block_size = Volume->GetDesc().DeviceDesc.LogicalBlockSize;
  const size_t bytes_per_stripe = num_logical_block_per_stripe * logical_block_size;
  const size_t phys_blocks_per_stripe = bytes_per_stripe / PhysicalBlockSize;
  const size_t starting_stripe = block_range.first / phys_blocks_per_stripe;
  const size_t device_num = starting_stripe % num_devices;
  assert(starting_stripe == (block_range.first + block_range.second - 1) / phys_blocks_per_stripe);
  assert(block_range.second * PhysicalBlockSize <= bytes_per_stripe);
  const size_t start_block_on_device = ((starting_stripe / num_devices) * phys_blocks_per_stripe) + (block_range.first % phys_blocks_per_stripe);
  const TDeviceSet &device_set = DeviceVec[device_num];
  for (const auto &device : device_set) {
    device->DiscardRange(SuperBytes /* super block */ + start_block_on_device * PhysicalBlockSize, block_range.second * PhysicalBlockSize);
  }
}

void TVolume::TChainedStrategy::DoDiscard(const TBlockRange &block_range) const {
  assert(this);
  const size_t device_num = block_range.first / NumBlocksPerExtent;
  const size_t start_block_on_device = block_range.first % NumBlocksPerExtent;
  assert((start_block_on_device + block_range.second) < (ExtentVec[device_num].Span / PhysicalBlockSize));
  const TDeviceSet &device_set = DeviceVec[device_num];
  for (const auto &device : device_set) {
    device->DiscardRange(SuperBytes /* super block */ + start_block_on_device * PhysicalBlockSize, block_range.second * PhysicalBlockSize);
  }
}

void TVolume::TStripedStrategy::DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                              const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                              TCompletionTrigger &trigger) {
  DoStripe(W, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger);
}

void TVolume::TStripedStrategy::DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                              const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                              TCompletionTrigger &trigger, const TIOCallback &cb) {
  DoStripe(W, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
}

void TVolume::TStripedStrategy::DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                             TCompletionTrigger &trigger) {
  assert(this);
  DoStripe(R, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger);
}

void TVolume::TStripedStrategy::DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                             TCompletionTrigger &trigger, const TIOCallback &cb) {
  assert(this);
  DoStripe(R, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
}

void TVolume::TStripedStrategy::DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                                              void **buf_array, size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority,
                                              bool abort_on_error, TCompletionTrigger &trigger) {
  assert(this);
  DoStripeV(R, code_location, buf_kind, util_src, buf_array, num_buf, start_offset, nbytes, priority, abort_on_error, trigger);
}

void TVolume::TStripedStrategy::DelegateReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                                              void **buf_array, size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority,
                                              bool abort_on_error, TCompletionTrigger &trigger, const TIOCallback &cb) {
  assert(this);
  DoStripeV(R, code_location, buf_kind, util_src, buf_array, num_buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
}

void TVolume::TStripedStrategy::DelegateAppendTouchedDevicesToSet(TDeviceSet &device_set, const TBlockRange &block_range) const {
  assert(this);
  const size_t &starting_block = block_range.first;
  const size_t &num_seq_blocks = block_range.second;
  const size_t start_offset = starting_block * PhysicalBlockSize;
  const size_t end_offset = (starting_block + num_seq_blocks) * PhysicalBlockSize;
  const size_t nbytes = num_seq_blocks * PhysicalBlockSize;
  const size_t num_logical_block_per_stripe = Volume->GetDesc().NumLogicalBlockPerStripe;
  const size_t num_devices = Volume->GetDesc().NumLogicalExtent;
  const size_t logical_block_size = Volume->GetDesc().DeviceDesc.LogicalBlockSize;
  const size_t bytes_per_stripe = num_logical_block_per_stripe * logical_block_size;
  size_t devices_added = 0UL;
  for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
    const TLogicalExtent &extent = ExtentVec[extent_num];
    if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
      const size_t volume_start = extent_num * extent.Span + (start_offset - extent.Start);
      long long bytes_to_write = nbytes;
      size_t cur_offset = volume_start;
      for (; bytes_to_write > 0 && devices_added < num_devices; ++devices_added) {
        const size_t offset_in_stripe = cur_offset % bytes_per_stripe;
        const size_t cur_bytes = min(bytes_per_stripe - offset_in_stripe, static_cast<size_t>(bytes_to_write));
        const size_t device_to_add = ((cur_offset / bytes_per_stripe) % num_devices);
        AppendTouchedDevicesToSet(device_set, device_to_add);
        bytes_to_write -= cur_bytes;
        cur_offset += cur_bytes;
      }
      return;
    }
  }
  throw std::logic_error("cross extent block ranges are not support for DelegateAppendTouchedDevicesToSet");
}

template <typename ...TArgs>
void TVolume::TStripedStrategy::DoStripe(TOp op, const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                         const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args) {
  assert(this);
  const size_t num_logical_block_per_stripe = Volume->GetDesc().NumLogicalBlockPerStripe;
  const size_t num_devices = Volume->GetDesc().NumLogicalExtent;
  const size_t end_offset = start_offset + nbytes;
  const size_t logical_block_size = Volume->GetDesc().DeviceDesc.LogicalBlockSize;
  const size_t bytes_per_stripe = num_logical_block_per_stripe * logical_block_size;
  for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
    const TLogicalExtent &extent = ExtentVec[extent_num];
    if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
      const size_t volume_start = extent_num * extent.Span + (start_offset - extent.Start);
      long long bytes_to_write = nbytes;
      size_t cur_offset = volume_start;
      for (; bytes_to_write > 0;) {
        const size_t offset_in_stripe = cur_offset % bytes_per_stripe;
        const size_t cur_bytes = min(bytes_per_stripe - offset_in_stripe, static_cast<size_t>(bytes_to_write));
        const size_t device_to_write_stripe_to = ((cur_offset / bytes_per_stripe) % num_devices);
        const size_t physical_offset_on_device = (((cur_offset / bytes_per_stripe) / num_devices) * bytes_per_stripe) + offset_in_stripe;
        switch (op) {
          case R: {
            Read(code_location, buf_kind, util_src, reinterpret_cast<uint8_t *>(buf) + (nbytes - bytes_to_write), device_to_write_stripe_to,
                 physical_offset_on_device, cur_bytes, priority, abort_on_error, args...);
            break;
          }
          case W: {
            Write(code_location, buf_kind, util_src, reinterpret_cast<uint8_t *>(buf) + (nbytes - bytes_to_write), device_to_write_stripe_to,
                  physical_offset_on_device, cur_bytes, priority, abort_on_error, start_offset, args...);
            break;
          }
        }
        bytes_to_write -= cur_bytes;
        cur_offset += cur_bytes;
      }
      return;
    }
  }
  throw std::logic_error("cross extent io is not yet supported");
}

template <typename ...TArgs>
void TVolume::TStripedStrategy::DoStripeV(TOp op, const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src,
                                          void **buf_array, size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority
                                          , bool abort_on_error, TArgs &...args) {
  assert(this);
  const size_t buf_size = GetPhysicalSize(buf_kind);
  assert(num_buf * buf_size == static_cast<size_t>(nbytes));
  const size_t num_logical_block_per_stripe = Volume->GetDesc().NumLogicalBlockPerStripe;
  const size_t num_devices = Volume->GetDesc().NumLogicalExtent;
  const size_t end_offset = start_offset + nbytes;
  const size_t logical_block_size = Volume->GetDesc().DeviceDesc.LogicalBlockSize;
  const size_t bytes_per_stripe = num_logical_block_per_stripe * logical_block_size;

  vector<TDeviceRequest> device_req_arr(num_devices);

  for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
    const TLogicalExtent &extent = ExtentVec[extent_num];
    if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
      const size_t volume_start = extent_num * extent.Span + (start_offset - extent.Start);
      long long bytes_to_write = nbytes;
      size_t cur_offset = volume_start;
      for (; bytes_to_write > 0;) {
        const size_t offset_in_stripe = cur_offset % bytes_per_stripe;
        const size_t cur_bytes = min(bytes_per_stripe - offset_in_stripe, static_cast<size_t>(bytes_to_write));
        const size_t device_to_write_stripe_to = ((cur_offset / bytes_per_stripe) % num_devices);
        const size_t physical_offset_on_device = (((cur_offset / bytes_per_stripe) / num_devices) * bytes_per_stripe) + offset_in_stripe;
        switch (op) {
          case R: {
            const size_t cur_nbyte_offset = nbytes - bytes_to_write;
            assert(cur_nbyte_offset % buf_size == 0);
            for (size_t i = 0; i < cur_bytes; i += buf_size) {
              device_req_arr[device_to_write_stripe_to].AddRequest(buf_array[(cur_nbyte_offset + i) / buf_size],
                                                                   physical_offset_on_device + i,
                                                                   buf_size,
                                                                   (*DeviceVec[device_to_write_stripe_to].begin())->GetMaxSegments(),
                                                                   (*DeviceVec[device_to_write_stripe_to].begin())->GetMaxSectorsKb());
            }
            break;
          }
          case W: {
            throw std::logic_error("TODO: implement DoStripeV for writes");
            break;
          }
        }
        bytes_to_write -= cur_bytes;
        cur_offset += cur_bytes;
      }
      size_t total_num_requests = 0UL;
      for (size_t i = 0; i < num_devices; ++i) {
        if (device_req_arr[i].GetNumRequest() > 0) {
          assert(device_req_arr[i].GetNumIops() > 0);
          total_num_requests += device_req_arr[i].GetNumIops();
        }
      }
      assert(total_num_requests);
      TGroupRequest *const group_request = NewGroupRequest<TArgs...>(total_num_requests, args...);
      for (size_t i = 0; i < num_devices; ++i) {
        if (device_req_arr[i].GetNumRequest() > 0) {
          SubmitRequest(i, device_req_arr[i], group_request, code_location, buf_kind, util_src, priority, abort_on_error, args...);
        }
      }
      return;
    }
  }
  throw std::logic_error("cross extent io is not yet supported");
}

void TVolume::TChainedStrategy::DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                              const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                              TCompletionTrigger &trigger) {
  assert(this);
  DoChain(W, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger);
}

void TVolume::TChainedStrategy::DelegateWrite(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                              const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                              TCompletionTrigger &trigger, const TIOCallback &cb) {
  assert(this);
  DoChain(W, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
}

void TVolume::TChainedStrategy::DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                             TCompletionTrigger &trigger) {
  assert(this);
  DoChain(R, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger);
}

void TVolume::TChainedStrategy::DelegateRead(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                             const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                                             TCompletionTrigger &trigger, const TIOCallback &cb) {
  assert(this);
  DoChain(R, code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
}

void TVolume::TChainedStrategy::DelegateReadV(const Base::TCodeLocation &/*code_location*/ /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/,
                                              void **/*buf_array*/, size_t /*num_buf*/, const TOffset /*start_offset*/, long long /*nbytes*/,
                                              DiskPriority /*priority*/, bool /*abort_on_error*/, TCompletionTrigger &/*trigger*/) {
  assert(this);
  throw std::logic_error("TODO: implement TChainedStrategy::DelegateReadV");
}

void TVolume::TChainedStrategy::DelegateReadV(const Base::TCodeLocation &/*code_location*/ /* DEBUG */, TBufKind /*buf_kind*/, uint8_t /*util_src*/,
                                              void **/*buf_array*/, size_t /*num_buf*/, const TOffset /*start_offset*/, long long /*nbytes*/,
                                              DiskPriority /*priority*/, bool /*abort_on_error*/, TCompletionTrigger &/*trigger*/,
                                              const TIOCallback &/*cb*/) {
  assert(this);
  throw std::logic_error("TODO: implement TChainedStrategy::DelegateReadV");
}

void TVolume::TChainedStrategy::DelegateAppendTouchedDevicesToSet(TDeviceSet &device_set, const TBlockRange &block_range) const {
  const size_t &starting_block = block_range.first;
  const size_t &num_seq_blocks = block_range.second;
  const size_t start_offset = starting_block * PhysicalBlockSize;
  const size_t end_offset = (starting_block + num_seq_blocks) * PhysicalBlockSize;
  for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
    const TLogicalExtent &extent = ExtentVec[extent_num];
    if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
      AppendTouchedDevicesToSet(device_set, extent_num);
      return;
    }
  }
  throw std::logic_error("cross extent block ranges are not support for DelegateAppendTouchedDevicesToSet");
}

template <typename... TArgs>
void TVolume::TChainedStrategy::DoChain(TOp op, const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                                        const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args) {
  assert(this);
  const size_t end_offset = start_offset + nbytes;
  for (size_t extent_num = 0UL; extent_num < ExtentVec.size(); ++extent_num) {
    const TLogicalExtent &extent = ExtentVec[extent_num];
    if (start_offset >= extent.Start && end_offset <= extent.Start + extent.Span) {
      const size_t volume_start = start_offset - extent.Start;
      switch (op) {
        case R: {
          Read(code_location, buf_kind, util_src, buf, extent_num, volume_start, nbytes, priority, abort_on_error, args...);
          break;
        }
        case W: {
          Write(code_location, buf_kind, util_src, buf, extent_num, volume_start, nbytes, priority, abort_on_error, start_offset, args...);
          break;
        }
      }
      return;
    }
  }
  throw std::logic_error("cross extent io is not yet supported");
}

bool TVolume::TStrategy::Init(const TExtentSet &extent_set) {
  if (extent_set.size() != Volume->GetDesc().NumLogicalExtent) {
    syslog(LOG_ERR, "ExtentSet size [%ld] must be equal to the number of logical extents in the volume [%ld]", extent_set.size(), Volume->GetDesc().NumLogicalExtent);
    throw std::runtime_error("ExtentSet size must be equal to the number of logical extents in the volume");
  }
  size_t num_drives = 0UL;
  const size_t replication_factor = Volume->GetDesc().ReplicationFactor;
  const size_t num_logical_extent = Volume->GetDesc().NumLogicalExtent;
  for (TVolume::TDeviceCollection::TCursor csr(Volume->GetDeviceCollection()); csr; ++csr, ++num_drives) {}
  const size_t num_required_drives = replication_factor * num_logical_extent;
  if (num_drives != num_required_drives) {
    syslog(LOG_ERR, "Trying to initialize volume with [%ld] drives when [%ld] are required, replication factor [%ld], number of logical extents [%ld]",
           num_drives,
           num_required_drives,
           replication_factor,
           num_logical_extent);
    return false;
  }
  TVolume::TDeviceCollection::TCursor device_csr(Volume->GetDeviceCollection());
  for (const auto &extent : extent_set) {
    ExtentVec.emplace_back(extent);
    TDeviceSet device_set;
    for (size_t r = 0; r < replication_factor; ++r) {
      assert(device_csr);
      device_set.emplace(&*device_csr);
      ++device_csr;
    }
    DeviceVec.emplace_back(std::move(device_set));
  }
  assert(!device_csr);
  return true;
}

void TDevice::ApplyCorruptionCheck(TBufKind buf_kind, void *buf, const TOffset offset, long long nbytes) const {
  assert(this);
  assert(offset - Util::PhysicalBlockSize /* super block */ + nbytes <= Desc.Capacity);
  switch (buf_kind) {
    case SectorCheckedBlock: {
      assert(nbytes % PhysicalBlockSize == 0);
      for (size_t num_buf = 0; num_buf < nbytes / PhysicalBlockSize; ++num_buf) {
        for (size_t i = 0; i < SectorsPerBlock; ++i) {
          Util::TCorruptionDetector::WriteMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalBlockSize) + (i * PhysicalSectorSize)),
                                                 PhysicalSectorSize,
                                                 offset + (num_buf * PhysicalBlockSize) + (i * PhysicalSectorSize));
        }
      }
      break;
    }
    case PageCheckedBlock: {
      assert(nbytes % PhysicalBlockSize == 0);
      for (size_t num_buf = 0; num_buf < nbytes / PhysicalBlockSize; ++num_buf) {
        for (size_t i = 0; i < PagesPerBlock; ++i) {
          Util::TCorruptionDetector::WriteMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalBlockSize) + (i * PhysicalPageSize)),
                                                 PhysicalPageSize,
                                                 offset + (num_buf * PhysicalBlockSize) + (i * PhysicalPageSize));
        }
      }
      break;
    }
    case CheckedSector: {
      assert(nbytes % PhysicalSectorSize == 0);
      for (size_t num_buf = 0; num_buf < nbytes / PhysicalSectorSize; ++num_buf) {
        Util::TCorruptionDetector::WriteMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalSectorSize)), PhysicalSectorSize, offset + (num_buf * PhysicalSectorSize));
      }
      break;
    }
    case CheckedPage: {
      assert(nbytes % PhysicalPageSize == 0);
      for (size_t num_buf = 0; num_buf < nbytes / PhysicalPageSize; ++num_buf) {
        Util::TCorruptionDetector::WriteMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalPageSize)), PhysicalPageSize, offset + (num_buf * PhysicalPageSize));
      }
      break;
    }
    case CheckedBlock: {
      assert(nbytes % PhysicalBlockSize == 0);
      for (size_t num_buf = 0; num_buf < nbytes / PhysicalBlockSize; ++num_buf) {
        Util::TCorruptionDetector::WriteMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalBlockSize)), PhysicalBlockSize, offset + (num_buf * PhysicalBlockSize));
      }
      break;
    }
    case FullSector: {
      assert(nbytes % PhysicalSectorSize == 0);
      break;
    }
    case FullPage: {
      assert(nbytes % PhysicalPageSize == 0);
      break;
    }
    case FullBlock: {
      assert(nbytes % PhysicalBlockSize == 0);
      break;
    }
  }
}

bool TDevice::CheckCorruptCheck(TBufKind buf_kind, void *buf, const TOffset offset, long long nbytes) const {
  assert(this);
  if (likely(DoCorruptionCheck)) {
    bool passed_corruption_check = true;
    switch (buf_kind) {
      case SectorCheckedBlock: {
        assert(nbytes % PhysicalBlockSize == 0);
        for (size_t num_buf = 0; num_buf < nbytes / PhysicalBlockSize; ++num_buf) {
          for (size_t i = 0; i < SectorsPerBlock; ++i) {
            if (!Util::TCorruptionDetector::TryReadMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalBlockSize) + (i * PhysicalSectorSize)),
                                                          PhysicalSectorSize,
                                                          offset + (num_buf * PhysicalBlockSize) + (i * PhysicalSectorSize))) {
              passed_corruption_check = false;
            }
          }
        }
        break;
      }
      case PageCheckedBlock: {
        assert(nbytes % PhysicalBlockSize == 0);
        for (size_t num_buf = 0; num_buf < nbytes / PhysicalBlockSize; ++num_buf) {
          for (size_t i = 0; i < PagesPerBlock; ++i) {
            if (!Util::TCorruptionDetector::TryReadMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalBlockSize) + (i * PhysicalPageSize)),
                                                          PhysicalPageSize,
                                                          offset + (num_buf * PhysicalBlockSize) + (i * PhysicalPageSize))) {
              passed_corruption_check = false;
            }
          }
        }
        break;
      }
      case CheckedSector: {
        assert(nbytes % PhysicalSectorSize == 0);
        for (size_t num_buf = 0; num_buf < nbytes / PhysicalSectorSize; ++num_buf) {
          if (!Util::TCorruptionDetector::TryReadMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalSectorSize)), PhysicalSectorSize, offset + (num_buf * PhysicalSectorSize))) {
            passed_corruption_check = false;
          }
        }
        break;
      }
      case CheckedPage: {
        assert(nbytes % PhysicalPageSize == 0);
        for (size_t num_buf = 0; num_buf < nbytes / PhysicalPageSize; ++num_buf) {
          if (!Util::TCorruptionDetector::TryReadMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalPageSize)), PhysicalPageSize, offset + (num_buf * PhysicalPageSize))) {
            passed_corruption_check = false;
          }
        }
        break;
      }
      case CheckedBlock: {
        assert(nbytes % PhysicalBlockSize == 0);
        for (size_t num_buf = 0; num_buf < nbytes / PhysicalBlockSize; ++num_buf) {
          if (!Util::TCorruptionDetector::TryReadMurmur(reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(buf) + (num_buf * PhysicalBlockSize)), PhysicalBlockSize, offset + (num_buf * PhysicalBlockSize))) {
            passed_corruption_check = false;
          }
        }
        break;
      }
      case FullSector: {
        assert(nbytes % PhysicalSectorSize == 0);
        break;
      }
      case FullPage: {
        assert(nbytes % PhysicalPageSize == 0);
        break;
      }
      case FullBlock: {
        assert(nbytes % PhysicalBlockSize == 0);
        break;
      }
    }
    return passed_corruption_check;
  } else {
    return true;
  }
}

void TMemoryDevice::Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset offset,
                          long long nbytes, DiskPriority priority, bool abort_on_error, const TOffset /*logical_start_offset*/,
                          TCompletionTrigger &trigger) {
  WriteImpl(code_location, buf_kind, util_src, buf, offset, nbytes, priority, abort_on_error);
  trigger.Callback(Success, "");
}

void TMemoryDevice::Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset offset,
                          long long nbytes, DiskPriority priority, bool abort_on_error, const TOffset /*logical_start_offset*/,
                          const TIOCallback &cb) {
  WriteImpl(code_location, buf_kind, util_src, buf, offset, nbytes, priority, abort_on_error);
  cb(Success, "");
}

void TMemoryDevice::WriteImpl(const Base::TCodeLocation &/*code_location*/ /* DEBUG */, TBufKind buf_kind, uint8_t /*util_src*/, void *buf,
                              const TOffset offset, long long nbytes, DiskPriority /*priority*/, bool /*abort_on_error*/) {
  assert(this);
  // we probably don't intend to abort on memory writes...
  ApplyCorruptionCheck(buf_kind, buf, offset, nbytes);
  memcpy(Data + offset, buf, nbytes);
}

void TMemoryDevice::Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset offset,
                         long long nbytes, DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger) {
  assert(this);
  if (ReadImpl(code_location, buf_kind, util_src, buf, offset, nbytes, priority, abort_on_error)) {
    trigger.Callback(Success, "");
  } else {
    trigger.Callback(Error, "Corrupt Data");
  }
}

void TMemoryDevice::Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf, const TOffset offset,
                         long long nbytes, DiskPriority priority, bool abort_on_error, const TIOCallback &cb) {
  assert(this);
  if (ReadImpl(code_location, buf_kind, util_src, buf, offset, nbytes, priority, abort_on_error)) {
    cb(Success, "");
  } else {
    cb(Error, "Corrupt Data");
  }
}

void TMemoryDevice::ReadV(const Base::TCodeLocation &code_location /* DEBUG */,
                          TBufKind buf_kind,
                          uint8_t util_src,
                          const std::vector<void *> &buf_vec,
                          const TOffset offset,
                          long long nbytes,
                          DiskPriority priority,
                          bool abort_on_error,
                          TCompletionTrigger &trigger) {
  bool success = true;
  TOffset cur_offset = offset;
  const size_t bytes_per_req = nbytes / buf_vec.size();
  for (void *buf : buf_vec) {
    bool ret = ReadImpl(code_location, buf_kind, util_src, buf, cur_offset, bytes_per_req, priority, abort_on_error);
    success = success && ret;
    cur_offset += bytes_per_req;
  }
  assert(cur_offset == offset + nbytes);
  if (success) {
    trigger.Callback(Success, "");
  } else {
    trigger.Callback(Error, "Corrupt Data");
  }
}

void TMemoryDevice::ReadV(const Base::TCodeLocation &code_location /* DEBUG */,
                          TBufKind buf_kind,
                          uint8_t util_src,
                          const std::vector<void *> &buf_vec,
                          const TOffset offset,
                          long long nbytes,
                          DiskPriority priority,
                          bool abort_on_error,
                          TGroupRequest *group_request) {
  bool success = true;
  TOffset cur_offset = offset;
  const size_t bytes_per_req = nbytes / buf_vec.size();
  for (void *buf : buf_vec) {
    bool ret = ReadImpl(code_location, buf_kind, util_src, buf, cur_offset, bytes_per_req, priority, abort_on_error);
    success = success && ret;
    cur_offset += bytes_per_req;
  }
  assert(cur_offset == offset + nbytes);
  if (success) {
    group_request->Callback(Success, "");
  } else {
    group_request->Callback(Error, "Corrupt Data");
  }
}

bool TMemoryDevice::ReadImpl(const Base::TCodeLocation &/*code_location*/ /* DEBUG */, TBufKind buf_kind, uint8_t /*util_src*/, void *buf,
                             const TOffset offset, long long nbytes, DiskPriority /*priority*/, bool abort_on_error) {
  assert(this);
  assert(offset + nbytes <= Desc.Capacity);
  memcpy(buf, Data + offset, nbytes);
  bool ret = CheckCorruptCheck(buf_kind, buf, offset, nbytes);
  if (unlikely(!ret) && abort_on_error) {
    abort();
  }
  return ret;
}

void TPersistentDevice::Write(const Base::TCodeLocation &code_location /* DEBUG */,
                              TBufKind buf_kind,
                              uint8_t /*util_src*/,
                              void *buf,
                              const TOffset offset,
                              long long nbytes,
                              DiskPriority priority,
                              bool abort_on_error,
                              const TOffset logical_start_offset,
                              TCompletionTrigger &trigger) {
  assert(this);
  assert(TDiskController::TEvent::LocalEventPool);
  ApplyCorruptionCheck(buf_kind, buf, offset, nbytes);

  TDiskController::TEvent *new_event = TDiskController::TEvent::LocalEventPool->Alloc();
  new_event->Init(this,
                  TDiskController::TEvent::LocalEventPool,
                  code_location,
                  TDiskController::TEvent::TriggeredWrite,
                  priority,
                  logical_start_offset,
                  trigger,
                  buf_kind,
                  buf,
                  offset,
                  nbytes,
                  abort_on_error);
  Enqueue(new_event);
}

void TPersistentDevice::Write(const Base::TCodeLocation &code_location /* DEBUG */,
                              TBufKind buf_kind,
                              uint8_t /*util_src*/,
                              void *buf,
                              const TOffset offset,
                              long long nbytes,
                              DiskPriority priority,
                              bool abort_on_error,
                              const TOffset logical_start_offset,
                              const TIOCallback &cb) {
  assert(this);
  assert(TDiskController::TEvent::LocalEventPool);
  ApplyCorruptionCheck(buf_kind, buf, offset, nbytes);
  TDiskController::TEvent *new_event = TDiskController::TEvent::LocalEventPool->Alloc();
  new_event->Init(this,
                  TDiskController::TEvent::LocalEventPool,
                  code_location,
                  TDiskController::TEvent::CallbackWrite,
                  priority,
                  logical_start_offset,
                  cb,
                  buf_kind,
                  buf,
                  offset,
                  nbytes,
                  abort_on_error);
  Enqueue(new_event);
}

void TPersistentDevice::Read(const Base::TCodeLocation &code_location /* DEBUG */,
                             TBufKind buf_kind,
                             uint8_t /*util_src*/,
                             void *buf,
                             const TOffset offset,
                             long long nbytes,
                             DiskPriority priority,
                             bool abort_on_error,
                             TCompletionTrigger &trigger) {
  assert(this);
  assert(TDiskController::TEvent::LocalEventPool);
  TDiskController::TEvent *new_event = TDiskController::TEvent::LocalEventPool->Alloc();
  new_event->Init(this,
                  TDiskController::TEvent::LocalEventPool,
                  code_location,
                  TDiskController::TEvent::TriggeredRead,
                  priority,
                  0, /* not used */
                  trigger,
                  buf_kind,
                  buf,
                  offset,
                  nbytes,
                  abort_on_error);
  Enqueue(new_event);
}

void TPersistentDevice::Read(const Base::TCodeLocation &code_location /* DEBUG */,
                             TBufKind buf_kind,
                             uint8_t /*util_src*/,
                             void *buf,
                             const TOffset offset,
                             long long nbytes,
                             DiskPriority priority,
                             bool abort_on_error,
                             const TIOCallback &cb) {
  assert(this);
  assert(TDiskController::TEvent::LocalEventPool);
  TDiskController::TEvent *new_event = TDiskController::TEvent::LocalEventPool->Alloc();
  new_event->Init(this,
                  TDiskController::TEvent::LocalEventPool,
                  code_location,
                  TDiskController::TEvent::CallbackRead,
                  priority,
                  0, /* not used */
                  cb,
                  buf_kind,
                  buf,
                  offset,
                  nbytes,
                  abort_on_error);
  Enqueue(new_event);
}

void TPersistentDevice::ReadV(const Base::TCodeLocation &code_location /* DEBUG */,
                              TBufKind buf_kind,
                              uint8_t /*util_src*/,
                              const std::vector<void *> &buf_vec,
                              const TOffset offset,
                              long long nbytes,
                              DiskPriority priority,
                              bool abort_on_error,
                              TCompletionTrigger &trigger) {
  assert(this);
  assert(TDiskController::TEvent::LocalEventPool);
  TDiskController::TEvent *new_event = TDiskController::TEvent::LocalEventPool->Alloc();
  new_event->Init(this,
                  TDiskController::TEvent::LocalEventPool,
                  code_location,
                  TDiskController::TEvent::TriggeredReadV,
                  priority,
                  0, /* not used */
                  trigger,
                  buf_kind,
                  buf_vec,
                  offset,
                  nbytes,
                  abort_on_error);
  Enqueue(new_event);
}

void TPersistentDevice::ReadV(const Base::TCodeLocation &code_location /* DEBUG */,
                              TBufKind buf_kind,
                              uint8_t /*util_src*/,
                              const std::vector<void *> &buf_vec,
                              const TOffset offset,
                              long long nbytes,
                              DiskPriority priority,
                              bool abort_on_error,
                              TGroupRequest *group_request) {
  assert(this);
  assert(TDiskController::TEvent::LocalEventPool);
  TDiskController::TEvent *new_event = TDiskController::TEvent::LocalEventPool->Alloc();
  new_event->Init(this,
                  TDiskController::TEvent::LocalEventPool,
                  code_location,
                  TDiskController::TEvent::CallbackReadV,
                  priority,
                  0, /* not used */
                  group_request,
                  buf_kind,
                  buf_vec,
                  offset,
                  nbytes,
                  abort_on_error);
  Enqueue(new_event);
}

void TPersistentDevice::AsyncSyncFlush(std::mutex &mut, std::condition_variable &cond, size_t &num_finished, size_t &num_err) {
  try {
    Sync();
  } catch (const std::exception &ex) {
    std::lock_guard<std::mutex> lock(mut);
    ++num_err;
  }
  std::lock_guard<std::mutex> lock(mut);
  ++num_finished;
  cond.notify_one();
}

void TPersistentDevice::Sync() {
  if (FsyncOn) {
    Base::IfLt0(fsync(DiskFd));
    Base::IfLt0(ioctl(DiskFd, BLKFLSBUF, 0));
  }
}

void TMemoryDevice::AsyncSyncFlush(std::mutex &mut, std::condition_variable &cond, size_t &num_finished, size_t &/*num_err*/) {
  std::lock_guard<std::mutex> lock(mut);
  ++num_finished;
  cond.notify_one();
}

void TMemoryDevice::Sync() {}

TVolume::TVolume(TDesc desc, const TCacheCb &cache_cb, Base::TScheduler *scheduler)
  : DeviceCollection(this),
    ManagerMembership(this),
    VolumeId(0UL),
    Desc(desc),
    Strategy(nullptr),
    Scheduler(scheduler),
    CacheCb(cache_cb) {
  if (Desc.Kind == TDesc::Striped && (Desc.NumLogicalBlockPerStripe * Desc.DeviceDesc.LogicalBlockSize) % PhysicalBlockSize != 0) {
    throw std::runtime_error("Stripe Size must be a multiple of PhysicalBlockSize");
  }
  if (Desc.Kind == TDesc::Striped && Desc.DeviceDesc.Capacity % (Desc.NumLogicalBlockPerStripe * Desc.DeviceDesc.LogicalBlockSize) != 0) {
    throw std::runtime_error("Device Capacity must be a multiple of the striping factor.");
  }
  if (Desc.DeviceDesc.Capacity % PhysicalBlockSize != 0) {
    throw std::runtime_error("Device Capacity must be a multiple of the physical block size.");
  }
}

TVolume::~TVolume() {
  assert(this);
  delete Strategy;
}

const std::vector<TLogicalExtent> &TVolume::GetLogicalExtentVec() const {
  assert(this);
  assert(Strategy);
  return Strategy->GetLogicalExtentVec();
}

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        template <typename... TArgs>
        void TVolume::Write(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                            const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TCacheInstr cache_instr,
                            TArgs &...args) {
          assert(this);
          DoCache(cache_instr, start_offset, buf, nbytes);
          Strategy->DelegateWrite(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, args...);
        }

        template <>
        void TVolume::Write<Stig::Indy::Disk::TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                  uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes,
                                                                  DiskPriority priority, bool abort_on_error, TCacheInstr cache_instr,
                                                                  TCompletionTrigger &trigger) {
          assert(this);
          DoCache(cache_instr, start_offset, buf, nbytes);
          Strategy->DelegateWrite(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger);
        }

        template <>
        void TVolume::Write<Stig::Indy::Disk::TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */,
                                                                                     TBufKind buf_kind, uint8_t util_src, void *buf,
                                                                                     const TOffset start_offset, long long nbytes,
                                                                                     DiskPriority priority, bool abort_on_error,
                                                                                     TCacheInstr cache_instr, TCompletionTrigger &trigger,
                                                                                     const TIOCallback &cb) {
          assert(this);
          DoCache(cache_instr, start_offset, buf, nbytes);
          Strategy->DelegateWrite(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
        }

        template <typename... TArgs>
        void TVolume::Read(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void *buf,
                           const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error, TArgs &...args) {
          assert(this);
          Strategy->DelegateRead(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, args...);
        }

        template <>
        void TVolume::Read<Stig::Indy::Disk::TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                 uint8_t util_src, void *buf, const TOffset start_offset, long long nbytes,
                                                                 DiskPriority priority, bool abort_on_error, TCompletionTrigger &trigger) {
          assert(this);
          Strategy->DelegateRead(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger);
        }

        template <>
        void TVolume::Read<Stig::Indy::Disk::TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */,
                                                                                    TBufKind buf_kind, uint8_t util_src, void *buf,
                                                                                    const TOffset start_offset, long long nbytes,
                                                                                    DiskPriority priority, bool abort_on_error,
                                                                                    TCompletionTrigger &trigger, const TIOCallback &cb) {
          assert(this);
          Strategy->DelegateRead(code_location, buf_kind, util_src, buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
        }

        template <typename... TArgs>
        void TVolume::ReadV(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind, uint8_t util_src, void **buf_array,
                            size_t num_buf, const TOffset start_offset, long long nbytes, DiskPriority priority, bool abort_on_error,
                            TArgs &...args) {
          assert(this);
          Strategy->DelegateReadV(code_location, buf_kind, util_src, buf_array, num_buf, start_offset, nbytes, priority, abort_on_error, args...);
        }

        template <>
        void TVolume::ReadV<Stig::Indy::Disk::TCompletionTrigger>(const Base::TCodeLocation &code_location /* DEBUG */, TBufKind buf_kind,
                                                                  uint8_t util_src, void **buf_array, size_t num_buf, const TOffset start_offset,
                                                                  long long nbytes, DiskPriority priority, bool abort_on_error,
                                                                  TCompletionTrigger &trigger) {
          assert(this);
          Strategy->DelegateReadV(code_location, buf_kind, util_src, buf_array, num_buf, start_offset, nbytes, priority, abort_on_error, trigger);
        }

        template <>
        void TVolume::ReadV<Stig::Indy::Disk::TCompletionTrigger, const TIOCallback>(const Base::TCodeLocation &code_location /* DEBUG */,
                                                                                     TBufKind buf_kind, uint8_t util_src, void **buf_array,
                                                                                     size_t num_buf, const TOffset start_offset, long long nbytes,
                                                                                     DiskPriority priority, bool abort_on_error,
                                                                                     TCompletionTrigger &trigger, const TIOCallback &cb) {
          assert(this);
          Strategy->DelegateReadV(code_location, buf_kind, util_src, buf_array, num_buf, start_offset, nbytes, priority, abort_on_error, trigger, cb);
        }

      }

    }

  }

}

inline void TVolume::TryAllocateSequentialBlocks(size_t num_blocks, const std::function<void (const TBlockRange &block_range)> &cb) {
  assert(this);
  assert(Strategy);
  Strategy->TryAllocateSequentialBlocks(num_blocks, cb);
}

void TVolume::FreeSequentialBlocks(const TBlockRange &block_range) {
  assert(this);
  assert(Strategy);
  Strategy->FreeSequentialBlocks(block_range);
}

void TVolume::MarkBlockRangeUsed(const TBlockRange &block_range) {
  assert(this);
  assert(Strategy);
  Strategy->MarkBlockRangeUsed(block_range);
}

void TVolume::AppendTouchedDevicesToSet(TDeviceSet &device_set, const TBlockRange &block_range) const {
  assert(this);
  Strategy->DelegateAppendTouchedDevicesToSet(device_set, block_range);
}

std::pair<size_t, size_t> TVolume::AppendUsage(std::stringstream &ss) const {
  assert(this);
  assert(Strategy);
  return Strategy->AppendUsage(ss);
}

bool TVolume::Init(const TExtentSet &extent_set) {
  assert(this);
  bool success = true;
  switch (Desc.Kind) {
    case TDesc::Striped: {
      Strategy = new TStripedStrategy(this, Scheduler);
      break;
    }
    case TDesc::Chained: {
      Strategy = new TChainedStrategy(this, Scheduler);
      break;
    }
  }
  try {
    success = Strategy->Init(extent_set);
  } catch (...) {
    delete Strategy;
    Strategy = nullptr;
  }
  return success;
}

void TVolume::DiscardAll() {
  Strategy->DiscardAll();
}

TVolumeManager::TVolumeManager(Base::TScheduler *scheduler)
    : VolumeCollection(this),
      Scheduler(scheduler),
      AllocatedExtentBlocks(std::numeric_limits<TOffset>::max() / ExtentAllocationBlockSize, false) {}

TVolumeManager::~TVolumeManager() {}

bool TVolumeManager::AddNewVolume(TVolume *volume) {
  assert(this);
  TExtentSet logical_extent_set;
  AllocateLogicalExtents(logical_extent_set, volume->GetDesc().NumLogicalExtent, volume->GetDesc().DeviceDesc.Capacity, volume);
  bool success = volume->Init(logical_extent_set);
  if (success) {
    VolumeCollection.Insert(volume->GetManagerMembership());
  }
  return success;
}

void TVolumeManager::AddExistingVolume(TVolume *volume, size_t volume_id) {
  assert(this);
  volume->SetVolumeId(volume_id);
  const size_t num_dev = volume->GetNumDevices();
  bool success = (volume->GetLogicalExtentVec().size() == num_dev && num_dev > 0);
  if (success) {
    for (const auto &extent : volume->GetLogicalExtentVec()) {
      const size_t consecutive_extent = ceil(static_cast<double>(extent.Span) / ExtentAllocationBlockSize);
      assert(extent.Start % ExtentAllocationBlockSize == 0);
      const size_t start_pos = extent.Start / ExtentAllocationBlockSize;
      for (size_t i = 0; i < consecutive_extent; ++i) {
        if (AllocatedExtentBlocks[start_pos + i]) {
          throw std::runtime_error("Trying to add volume that covers logical extent which is already allocated.");
        }
        AllocatedExtentBlocks[start_pos + i] = true;
        LogicalExtentStartToVolumeMap.emplace((start_pos + i) * ExtentAllocationBlockSize, volume);
      }
    }
    VolumeCollection.Insert(volume->GetManagerMembership());
  } else {
    throw std::runtime_error("AddExistingVolume requires initialized volume.");
  }
}

void TVolumeManager::TryAllocateSequentialBlocks(TVolume::TDesc::TStorageSpeed storage_speed, size_t num_blocks, const std::function<void (const TBlockRange &block_range)> &cb) {
  assert(this);
  for (TVolumeCollection::TCursor csr(&VolumeCollection); csr; ++csr) {
    if (csr->GetDesc().StorageSpeed == storage_speed) {
      try {
        return csr->TryAllocateSequentialBlocks(num_blocks, cb);
      } catch (const std::exception &) {
        continue;
      }
    }
  }
  /* this means there are no blocks left of the storage kind we requested. We're going to start allocating from other volumes...
     TODO: We can be specific about the policy we use to satisfy this request. */
  for (TVolumeCollection::TCursor csr(&VolumeCollection); csr; ++csr) {
    try {
      return csr->TryAllocateSequentialBlocks(num_blocks, cb);
    } catch (const std::exception &) {
      continue;
    }
  }
  throw std::logic_error("Out of disk space.");
}

void TVolumeManager::MarkBlockRangeUsed(const TBlockRange &block_range) {
  assert(this);
  assert(block_range.second > 0);
  const size_t &starting_block = block_range.first;
  const size_t &num_seq_blocks = block_range.second;
  const size_t start_extent_address = starting_block * PhysicalBlockSize;
  const size_t end_extent_address = (starting_block + num_seq_blocks) * PhysicalBlockSize;
  const size_t logical_extent_block_start = (start_extent_address / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
  const size_t logical_extent_block_end = (end_extent_address / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
  TVolume *const start_vol = LogicalExtentStartToVolumeMap[logical_extent_block_start];
  TVolume *const end_vol = LogicalExtentStartToVolumeMap[logical_extent_block_end];
  assert(start_vol && end_vol);
  if (start_vol == end_vol) {
    start_vol->MarkBlockRangeUsed(block_range);
  } else {
    throw std::logic_error("TODO: implement support for cross-volume MarkBlockRangeUsed");
  }
}

void TVolumeManager::FreeSequentialBlocks(const TBlockRange &block_range) {
  assert(this);
  const size_t &starting_block = block_range.first;
  const size_t &num_seq_blocks = block_range.second;
  const size_t start_extent_address = starting_block * PhysicalBlockSize;
  const size_t end_extent_address = (starting_block + num_seq_blocks) * PhysicalBlockSize;
  const size_t logical_extent_block_start = (start_extent_address / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
  const size_t logical_extent_block_end = (end_extent_address / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
  TVolume *const start_vol = LogicalExtentStartToVolumeMap[logical_extent_block_start];
  TVolume *const end_vol = LogicalExtentStartToVolumeMap[logical_extent_block_end];
  assert(start_vol && end_vol);
  if (start_vol == end_vol) {
    start_vol->FreeSequentialBlocks(block_range);
  } else {
    throw std::logic_error("TODO: implement support for cross-volume block release");
  }
}

void TVolumeManager::SyncToDisk(const std::vector<TBlockRange> &block_range_vec) {
  assert(this);
  assert(block_range_vec.size());
  TDeviceSet device_set;
  for (const auto &block_range : block_range_vec) {
    const size_t &starting_block = block_range.first;
    const size_t &num_seq_blocks = block_range.second;
    const size_t start_extent_address = starting_block * PhysicalBlockSize;
    const size_t end_extent_address = (starting_block + num_seq_blocks) * PhysicalBlockSize;
    const size_t logical_extent_block_start = (start_extent_address / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
    const size_t logical_extent_block_end = (end_extent_address / ExtentAllocationBlockSize) * ExtentAllocationBlockSize;
    TVolume *const start_vol = LogicalExtentStartToVolumeMap[logical_extent_block_start];
    TVolume *const end_vol = LogicalExtentStartToVolumeMap[logical_extent_block_end];
    assert(start_vol && end_vol);
    if (start_vol == end_vol) {
      start_vol->AppendTouchedDevicesToSet(device_set, block_range);
    } else {
      throw std::logic_error("TODO: implement support for cross-volume block range input to SyncToDisk");
    }
  }
  assert(!device_set.empty());
  if (device_set.size() == 1) {
    (*device_set.begin())->Sync();
  } else {
    const size_t num_scheduled = device_set.size();
    std::mutex mut;
    std::condition_variable cond;
    size_t num_finished = 0UL;
    size_t num_err = 0UL;
    for (TDevice *dev : device_set) {
      Scheduler->Schedule(std::bind(&TDevice::AsyncSyncFlush, dev, ref(mut), ref(cond), ref(num_finished), ref(num_err)));
    }
    /* acquire mutex */ {
      std::unique_lock<std::mutex> lock(mut);
      while (num_finished != num_scheduled) {
        cond.wait(lock);
      }
    }  // release mutex
    if (num_err > 0) {
      throw std::runtime_error("Error while SyncToDisk()");
    }
  }
}

void TVolumeManager::AppendVolumeUsageReport(std::stringstream &ss) const {
  assert(this);
  std::pair<size_t, size_t> total_usage = make_pair(0UL, 0UL);
  std::pair<size_t, size_t> slow_usage = make_pair(0UL, 0UL);
  std::pair<size_t, size_t> fast_usage = make_pair(0UL, 0UL);
  for (TVolumeCollection::TCursor csr(&VolumeCollection); csr; ++csr) {
    const std::pair<size_t, size_t> vol_usage = csr->AppendUsage(ss);
    total_usage.first += vol_usage.first;
    total_usage.second += vol_usage.second;
    switch (csr->GetDesc().StorageSpeed) {
      case TVolume::TDesc::TStorageSpeed::Fast: {
        fast_usage.first += vol_usage.first;
        fast_usage.second += vol_usage.second;
        break;
      }
      case TVolume::TDesc::TStorageSpeed::Slow: {
        slow_usage.first += vol_usage.first;
        slow_usage.second += vol_usage.second;
        break;
      }
      default: {
        throw std::logic_error("Unhandled storage speed in AppendVolumeUsageReport");
        break;
      }
    }
  }
  ss << "Disk Usage = " << total_usage.first << " / " << total_usage.second << std::endl;
  ss << "Slow Usage = " << slow_usage.first << " / " << slow_usage.second << std::endl;
  ss << "Fast Usage = " << fast_usage.first << " / " << fast_usage.second << std::endl;
}

void TVolumeManager::AllocateLogicalExtents(TExtentSet &logical_extent_set, size_t num_extent, size_t extent_size, TVolume *volume) {
  assert(this);
  assert(logical_extent_set.empty());
  const size_t required_consecutive_extent = ceil(static_cast<double>(extent_size) / ExtentAllocationBlockSize);
  size_t consective_found = 0UL;
  for (size_t num_find = 0UL; num_find < num_extent; ++num_find) {
    bool found = false;
    for (size_t i = 0UL; i < AllocatedExtentBlocks.size(); ++i) {
      if (!AllocatedExtentBlocks[i]) {
        ++consective_found;
        if (consective_found == required_consecutive_extent) {
          const size_t extent_start = (i - required_consecutive_extent + 1UL) * ExtentAllocationBlockSize;
          for (size_t j = (i - required_consecutive_extent + 1UL); j <= i; ++j) {
            AllocatedExtentBlocks[j] = true;
            LogicalExtentStartToVolumeMap.emplace(j * ExtentAllocationBlockSize, volume);
          }
          logical_extent_set.insert(TLogicalExtent{extent_start, extent_size});
          found = true;
          break;
        }
      } else {
        consective_found = 0UL;
      }
    }
    if (!found) {
      /* TODO: unwind any extents we just allocated. */
      throw std::runtime_error("Cannot allocate enough consecutive extent space.");
    }
  }
  assert(logical_extent_set.size() == num_extent);
}

void TVolumeManager::DiscardAllDevices() {
  assert(this);
  for (TVolumeCollection::TCursor csr(&VolumeCollection); csr; ++csr) {
    csr->DiscardAll();
  }
}
