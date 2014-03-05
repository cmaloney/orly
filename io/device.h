/* <io/device.h>

   A file-descriptor-based I/O device.

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
#include <stdexcept>

#include <base/fd.h>
#include <base/no_copy_semantics.h>
#include <io/chunk_and_pool.h>
#include <io/input_producer.h>
#include <io/output_consumer.h>

namespace Io {

  /* A file-descriptor-based I/O device. */
  class TDevice
      : public TInputProducer,
        public TOutputConsumer {
    NO_COPY_SEMANTICS(TDevice);
    public:

    /* Thrown when TryProduceInput() times out while trying to read. */
    class TTimeout
        : public std::runtime_error {
      public:

      /* Do-little. */
      TTimeout();

    };  // TTimeout

    /* Use the given file descriptor for I/O.  It must already be open.
       Construct our own pool. */
    explicit TDevice(const Base::TFd &fd, const TPool::TArgs &args = TPool::TArgs())
        : Timeout(-1), Fd(fd), Pool(std::make_shared<TPool>(args)) {}

    /* Use the given file descriptor for I/O.  It must already be open.
       Construct our own pool. */
    explicit TDevice(Base::TFd &&fd, const TPool::TArgs &args = TPool::TArgs())
        : Timeout(-1), Fd(std::move(fd)), Pool(std::make_shared<TPool>(args)) {}

    /* Use the given file descriptor for I/O.  It must already be open.
       Use the given pool, which must not be null. */
    TDevice(const Base::TFd &fd, const std::shared_ptr<TPool> &pool)
        : Timeout(-1), Fd(fd), Pool(pool) {
      assert(pool);
    }

    /* Use the given file descriptor for I/O.  It must already be open.
       Use the given pool, which must not be null. */
    TDevice(Base::TFd &&fd, const std::shared_ptr<TPool> &pool)
        : Timeout(-1), Fd(std::move(fd)), Pool(pool) {
      assert(pool);
    }

    /* Do-little. */
    virtual ~TDevice() {}

    /* The file descriptor which we wrap. */
    const Base::TFd &GetFd() const {
      assert(this);
      return Fd;
    }

    /* See TOutputConsumer::ConsumeOutput(). */
    virtual void ConsumeOutput(const std::shared_ptr<const TChunk> &chunk);

    /* The pool from which we acquire chunks.  Never null. */
    const std::shared_ptr<TPool> &GetPool() const {
      assert(this);
      return Pool;
    }

    /* See TInputProducer::ProduceInput().
       If TImeout is non-negative, then this function can throw TTimeout. */
    virtual std::shared_ptr<const TChunk> TryProduceInput();

    /* The maximum number of milliseconds to wait for data to become available.
       A negative value here means to wait forever. */
    int Timeout;

    private:

    /* See accessor. */
    Base::TFd Fd;

    /* See accessor. */
    std::shared_ptr<TPool> Pool;

  };  // TDevice

}  // Io
