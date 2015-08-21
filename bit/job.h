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

#include <cassert>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <base/fd.h>
#include <base/opt.h>
#include <jhm/naming.h>
#include <util/stl.h>
#include <util/time.h>

namespace Jhm {

class TEnv;
class TFile;
class TJob;

// Max-size capped output buffer. Ideally a Twine class but lazy for now.
class TOutputBuffer {
  using TPtr = std::unique_ptr<uint8_t[]>;

  private:
  std::vector<TPtr> Blocks;
};

class TJob {
  public:

  template <typename TVal>
  using TSet = std::unordered_set<TVal>;

  struct TOutput {
    enum TResult {
      NewNeeds,
      Error,
      Complete
    } Result;

    // Write adapters to do subprocess -> OutputBuffer via pump or just
    // using OutputBuffer as a streaming data target directly with no virtual
    // calls.
    // TODO(cmaloney): Allow output of "structured" result objects where there
    // is just a conversion to string available in case of error, as well as
    // introspection available for checking things like "gather the test output"
    TOutputBuffer Output;
  };

  virtual ~TJob() = default;

  virtual const TSet<TFile *> GetNeeds() = 0;

  /* This is used purely for cache completion. Return a set of things where, if any of them can
     exist, the job's
     output must be invalidated. Called only after the job has returned true from IsComplete. */
  virtual TSet<TFile *> GetAntiNeeds() { return TSet<TFile *>(); }

  /* Allows a job to verify that it's complete. If it returns false here, the command __WILL__ get
     run again when
     all the files returned by GetNeeds() are all done. */
  virtual bool IsComplete() = 0;

  virtual TOutput Run() = 0;

  TFile *GetInput() {
    assert(this);
    return Input;
  }

  const TSet<TFile *> &GetOutput() const {
    assert(this);
    return Output;
  }

  TFile *GetSoleOutput() const {
    assert(this);
    assert(Output.size() == 1);
    for(TFile *f : GetOutput()) {
      return f;
    }
    __builtin_unreachable();
  }


  protected:
  // NOTE: In theory we can take multiple files in. In  Ppractice we have no instances of that.
  TJob(TFile *input, TSet<TFile *> output);

  private:
  TFile *Input;
  TSet<TFile *> Output;
  const TJobProducer *JobInfo;
};

std::ostream &operator<<(std::ostream &out, TJob *job);
}
