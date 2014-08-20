/* <jhm/jobs/nycr.h>

   Job which calculates / generates a C dependency file

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

#include <jhm/job.h>

namespace Jhm {

  namespace Job {

    /* Produces a file listing the languages contained in a nycr file. */
    class TNycrLang final : public TJob {
      public:

      static TJobProducer GetProducer();

      virtual const char *GetName() final;
      virtual const TSet<TFile*> GetNeeds() final;
      virtual std::string GetCmd() final;
      virtual Util::TTimestamp GetCmdTimestamp() const final;
      virtual bool IsComplete() final;

      private:
      TNycrLang(TEnv &env, TFile *input);

      TEnv &Env;
    };

    class TNycr final : public TJob {
      public:

      static TJobProducer GetProducer();

      virtual const char *GetName() final;
      virtual const TSet<TFile*> GetNeeds() final;
      virtual std::string GetCmd() final;
      virtual Util::TTimestamp GetCmdTimestamp() const final;
      virtual bool IsComplete() final;

      private:
      TNycr(TEnv &env, TFile *input);

      TEnv &Env;
      TFile *Need;
    };

  }
}