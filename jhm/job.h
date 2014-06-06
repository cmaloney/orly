/* <jhm/job.h>

   The generic form of a job.

   Copyright 2010-2014 OrlyAtomics, Inc.

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
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <base/opt.h>
#include <base/stl_utils.h>
#include <jhm/naming.h>

namespace Jhm {

  class TEnv;
  class TFile;
  class TJob;

  struct TJobProducer {
    const char *Name;
    // TODO: Should really be a set...
    std::vector<TExtension> OutExtensions;
    std::function<Base::TOpt<TRelPath> (const TRelPath &name)> TryGetInput;
    std::function<std::unique_ptr<TJob> (TEnv &env, TFile *in_file)> MakeJob;
  };

  /* Generic job. Jobs are strongly typed using classical inheritance to expose common attributes.

     Jobs provide some common bits like Config


     TODO: May make a proxy like File is, where the actual job in question then doesn't have to implement the generic
     behavoirs of the job class. */
  class TJob {
    public:
    template<typename TVal>
    using TSet = std::unordered_set<TVal>;

    virtual ~TJob() = default;

    virtual const char *GetName() = 0;
    virtual const TSet<TFile*> GetNeeds() = 0;

    void AddOutput(TFile *file) {
      assert(UnknownOutputs);
      Base::InsertOrFail(Output, file);
    }

    /* Allows a job to verify that it's complete. If it returns false here, the command __WILL__ get run again when
       all the files returned by GetNeeds() are all done. */
    virtual bool IsComplete() = 0;

    TFile *GetInput() {
      assert(this);
      return Input;
    }

    const TSet<TFile*> &GetOutput() const {
      assert(this);
      return Output;
    }

    TFile *GetSoleOutput() const {
      assert(this);
      assert(Output.size() == 1);
      for(TFile *f: GetOutput()) {
        return f;
      }
      __builtin_unreachable();
    }

    virtual std::string GetCmd() = 0;

    bool HasUnknownOutputs() const {
      assert(this);
      return UnknownOutputs;
    }

    protected:
    // NOTE: In theory we can take multiple files in. In  Ppractice we have no instances of that.
    TJob(TFile *input, TSet<TFile*> output, bool unknown_outputs=false);

    void MarkAllOutputsKnown() {
      assert(UnknownOutputs);
      UnknownOutputs = false;
    }

    private:
    // True iff we have outputs we don't know about initially (Ex: nycr). This enables adding more outputs later.
    TFile *Input;
    TSet<TFile*> Output;
    bool UnknownOutputs;
  };

  std::ostream &operator<<(std::ostream &out, TJob *job);
  /* JobKind
    TStr &&name, TExtension &&core_in, std::set<TExtension> &&OutExtensions);

    private:
    TFile In;
    TStr Name;
    TExtension in_ext;
    std::set<TExtension> out_ext;
  */
}