/* <jhm/job_kind.h>

   A registry of job types.

   Used for type checking between different job kinds / figuring out if an input is valid for a given job kind.

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

#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <jhm/naming.h>

namespace Jhm {

  class TFile;

  /* Generic job. Jobs are strongly typed using classical inheritance to expose common attributes.

     Jobs provide some common bits like Config


     TODO: May make a proxy like File is, where the actual job in question then doesn't have to implement the generic
     behavoirs of the job class. */
  class TJob {
    public:
    template<typename TVal>
    using TSet = std::unordered_set<TVal>;

    virtual ~TJob() = default;


    virtual const TSet<TFile*> GetNeeds() = 0;
    virtual TFile *GetInput() = 0;
    virtual TSet<TFile*> GetOutput() = 0;
    virtual std::string GetCmd() = 0;

    protected:
    // NOTE: In theory we can take multiple files in. In  Ppractice we have no instances of that.
    TJob();

  };

  /* JobKind
    TStr &&name, TExtension &&core_in, std::set<TExtension> &&OutExtensions);

    private:
    TFile In;
    TStr Name;
    TExtension in_ext;
    std::set<TExtension> out_ext;
  */
}