/* Links [.o] -> [] (Linux Executable)

   Copyright 2016 Theoretical Chaos.

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

#include <bit/job.h>

namespace Bit {

namespace Jobs {

class TLink final : public TJob {
  public:
  static TJobProducer GetProducer(const TJobConfig &job_config);

  virtual TOutput Run(TFileEnvironment *file_environment) final;
  virtual std::string GetConfigId() const final;
  virtual std::unordered_map<TFileInfo *, TJobConfig> GetOutputExtraData() const final;

  private:
  TLink(TMetadata &&metadata, const TJobConfig *job_config);

  std::unordered_set<TFileInfo *> AntiNeeds;
  std::unordered_set<TFileInfo *> ObjToCheck;
  std::unordered_set<TFileInfo *> ObjFiles;
};

}  // Job

}  // Jhm
