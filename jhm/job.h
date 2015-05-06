/* <jhm/job.h>

   The generic form of a job.

   Copyright 2015 Theoretical Chaos.

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

struct TJobProducer {
  const char *Name;
  // TODO: Should really be a set...
  std::vector<std::vector<std::string>> OutExtensions;
  std::function<Base::TOpt<TRelPath>(const TRelPath &name)> TryGetInput;
  std::function<std::unique_ptr<TJob>(TEnv &env, TFile *in_file)> MakeJob;
};

/* Generic job. Jobs are strongly typed using classical inheritance to expose common attributes.

   Jobs provide some common bits like Config


   TODO: May make a proxy like File is, where the actual job in question then doesn't have to
   implement the generic
   behavoirs of the job class. */
class TJob {
  public:
  template <typename TVal>
  using TSet = std::unordered_set<TVal>;

  virtual ~TJob() = default;

  virtual const char *GetName() = 0;
  virtual const TSet<TFile *> GetNeeds() = 0;

  /* This is used purely for cache completion. Return a set of things where, if any of them can
     exist, the job's
     output must be invalidated. Calle only after the job has returned true from IsComplete. */
  virtual TSet<TFile *> GetAntiNeeds() { return TSet<TFile *>(); }

  void AddOutput(TFile *file) {
    assert(UnknownOutputs);
    Util::InsertOrFail(Output, file);
  }

  /* Allows a job to verify that it's complete. If it returns false here, the command __WILL__ get
     run again when
     all the files returned by GetNeeds() are all done. */
  virtual bool IsComplete() = 0;

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

  virtual std::vector<std::string> GetCmd() = 0;

  // TODO(cmaloney): Implement this per job rather than giving this default.
  virtual void ProcessOutput(Base::TFd /*stdout*/, Base::TFd /*stderr*/) {}

  virtual Util::TTimestamp GetCmdTimestamp() const = 0;

  bool HasUnknownOutputs() const {
    assert(this);
    return UnknownOutputs;
  }

  void MarkAllOutputsKnown() {
    assert(UnknownOutputs);
    UnknownOutputs = false;
  }

  protected:
  // NOTE: In theory we can take multiple files in. In  Ppractice we have no instances of that.
  TJob(TFile *input, TSet<TFile *> output, bool unknown_outputs = false);

  private:
  TFile *Input;
  TSet<TFile *> Output;
  // True iff we have outputs we don't know about initially (Ex: nycr). This enables adding more
  // outputs later.
  bool UnknownOutputs;
};

std::ostream &operator<<(std::ostream &out, TJob *job);
}
