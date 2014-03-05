/* <gz/input_producer.h>

   TODO

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

#include <memory>
#include <utility>

#include <gz/file.h>
#include <io/chunk_and_pool.h>
#include <io/input_producer.h>

namespace Gz {

  /* TODO */
  class TInputProducer final
      : public Io::TInputProducer {
    NO_COPY_SEMANTICS(TInputProducer);
    public:

    /* TODO */
    using TChunk = Io::TChunk;
    using TPool  = Io::TPool;

    /* TODO */
    TInputProducer(const char *path, const char *mode, const TPool::TArgs &args = TPool::TArgs())
        : File(path, mode), Pool(std::make_shared<TPool>(args)) {}

    /* TODO */
    TInputProducer(Base::TFd &&fd, const char *mode, const TPool::TArgs &args = TPool::TArgs())
        : File(std::move(fd), mode), Pool(std::make_shared<TPool>(args)) {}

    /* See base class. */
    virtual std::shared_ptr<const TChunk> TryProduceInput() override;

    private:

    /* TODO */
    TFile File;

    /* TODO */
    std::shared_ptr<TPool> Pool;

  };  // TInputProducer

}  // Gz
