/* <io/recorder_and_player.h>

   A consumer of output which holds all its data in memory,
   and an input producer which plays back a recording.

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
#include <string>
#include <vector>

#include <base/no_copy_semantics.h>
#include <io/input_producer.h>
#include <io/output_consumer.h>

namespace Io {

  /* This forward declaration is necessary because TRecorder makes reference to TPlayer. */
  class TPlayer;

  /* A consumer of output which holds all its data in memory. */
  class TRecorder
      : public TOutputConsumer {
    NO_COPY_SEMANTICS(TRecorder);
    public:

    /* Do-little. */
    TRecorder() {}

    /* Push a reference to the given string into the stream.
       The string must continue to exist. */
    TRecorder(const std::string &in) {
      ConsumeOutput(std::make_shared<TChunk>(TChunk::Full, in));
    }

    /* See TOutputConsumer::ConsumeOutput(). */
    virtual void ConsumeOutput(const std::shared_ptr<const TChunk> &chunk);

    /* Copy our contents to a string. */
    void CopyOut(std::string &out) const;

    private:

    /* See accessor. */
    std::vector<std::shared_ptr<const TChunk>> Chunks;

    /* TPlayer needs access to our collection of chunks. */
    friend class TPlayer;

  };  // TRecorder

  /* An input producer which plays back a recording. */
  class TPlayer
      : public TInputProducer {
    NO_COPY_SEMANTICS(TPlayer);
    public:

    /* Play back from the given recorder, which must not be null. */
    explicit TPlayer(const std::shared_ptr<TRecorder> &recorder,
                     const std::shared_ptr<std::function<std::shared_ptr<const TChunk> ()>> &next_chunk_cb = std::shared_ptr<std::function<std::shared_ptr<const TChunk> ()>>())
        : Recorder(recorder), Idx(0), NextChunkCb(next_chunk_cb) {
      assert(recorder);
    }

    /* The recorder from which we are playing back.  Never null. */
    const std::shared_ptr<TRecorder> &GetRecorder() const {
      assert(this);
      return Recorder;
    }

    /* See TInputProducer::ProduceInput(). */
    virtual std::shared_ptr<const TChunk> TryProduceInput();

    private:

    /* See accessor. */
    std::shared_ptr<TRecorder> Recorder;

    /* The index (within our Recorder's collection of chunks) of the next chunk to produce.
       If this is >= the size of the collection, then we have run out of chunks. */
    size_t Idx;

    /* TODO */
    std::shared_ptr<std::function<std::shared_ptr<const TChunk> ()>> NextChunkCb;

  };  // TPlayer

}  // Io
