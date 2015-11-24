/* Copyright 2015 Theoretical Chaos.

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
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <base/class_traits.h>
#include <base/cyclic_buffer.h>
#include <bit/job_config.h>
#include <bit/naming.h>

namespace Bit {

class TFileInfo;
struct TJobProducer;

class TJob {
  public:
  NO_COPY(TJob)
  NO_MOVE(TJob)

  struct TNeeds {
    std::unordered_set<TRelPath> Mandatory;
    std::unordered_set<TRelPath> IfBuildable;
    std::unordered_set<std::string> IfInTree;
  };

  // TODO(cmaloney): Make it so only Output, Needs, and Complete along with
  // their state are mutually exclusive.
  struct TOutput {
    MOVE_ONLY(TOutput)
    TOutput() = default;

    // Default to error so jobs which don't set result don't succeed ever to
    // make debugging easier.
    // NewNeeds means new needs have been generated which should be evaulated
    // and the job _must_ be run again.
    // CompleteIfNeeds means the job is complete if all the items in the Needs
    // set it returns are done (This is effectively preliminary output). The
    // job may be called again if CompleteIfNeeds is returned as result without
    // any needs changes.
    // CompleteIfNeeds may also be pre-emptively re-run before Needs are
    // complete.
    enum TResult { Error, NewNeeds, CompleteIfNeeds, Complete } Result = Error;

    // Write adapters to do subprocess -> OutputBuffer via pump or just
    // using OutputBuffer as a streaming data target directly with no virtual
    // calls.
    // TODO(cmaloney): Allow output of "structured" result objects where there
    // is just a conversion to string available in case of error, as well as
    // introspection available for checking things like "gather the test output"
    Base::TCyclicBuffer Output;
    TNeeds Needs;
  };

  struct TId {
    const TJobProducer *Producer;
    TFileInfo *Input;

    bool operator==(const TId &that) const noexcept;

    struct THash {
      size_t operator()(const TId &that) const noexcept;
    };
  };

  struct TMetadata {
    const TJobProducer *Producer;
    TFileInfo *Input;
    std::unordered_set<TFileInfo *> Output;
  };

  virtual ~TJob() = default;

  virtual TOutput Run() = 0;

  virtual std::unordered_map<TFileInfo*, TJobConfig> GetOutputExtraData() const = 0;

  // Returns a string which represents the full configuration of this job.
  // Callable at any time after a job is created (both before and after the job
  // is completed), must always return the same value. Used for cache completing
  // files produced by this job (If input timestamps are newer than output
  // timestamps and the ConfigId matches, then nothing has changed and the
  // existing output can be used). Note ConfigId shouldn't contain the producer
  // name, input file, set of output files, or the needs. Those are already
  // accounted for in job cache completion.
  virtual std::string GetConfigId() const = 0;

  TFileInfo *GetInput() const;

  const std::unordered_set<TFileInfo *> &GetOutput() const;

  TFileInfo *GetSoleOutput() const;

  const TJobProducer *GetJobProducer() const;

  protected:
  // NOTE: In theory we can take multiple files in. In  practice we have no instances of that.
  TJob(TMetadata &&metadata);

  private:
  const TMetadata Metadata;
};

std::ostream &operator<<(std::ostream &out, TJob *job);

std::ostream &operator<<(std::ostream &out, const Base::TCyclicBuffer &buffer);

}  // namespace Bit
