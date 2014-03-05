/* <gz/output_consumer.h>

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

#include <gz/file.h>
#include <io/chunk_and_pool.h>
#include <io/output_consumer.h>

namespace Gz {

  /* TODO */
  class TOutputConsumer final
      : public Io::TOutputConsumer {
    NO_COPY_SEMANTICS(TOutputConsumer);
    public:

    /* TODO */
    using TChunk = Io::TChunk;

    /* TODO */
    TOutputConsumer(const char *path, const char *mode)
        : File(path, mode) {}

    /* See base class. */
    virtual void ConsumeOutput(const std::shared_ptr<const TChunk> &chunk) override;

    private:

    /* TODO */
    TFile File;

  };  // TOutputConsumer

}  // Gz
