/* <bit/jobs/compile_c_family.h>

   Compiles {.c,.cc} -> .o

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

#include <bit/job.h>

namespace Bit {

namespace Jobs {

class TCompileCFamily final : public TJob {
  public:

  static std::vector<std::string> GetBaseCArgs();
  static std::vector<std::string> GetBaseCppArgs();

  static TJobProducer GetCProducer(const TJobConfig &job_config);
  static TJobProducer GetCcProducer(const TJobConfig &job_config);

  virtual TOutput Run(TFileEnvironment *file_environment) final;
  virtual std::string GetConfigId() const final;
  virtual std::unordered_map<TFileInfo *, TJobConfig> GetOutputExtraData() const final;

  private:
  TCompileCFamily(TMetadata &&metadata, bool is_cc);

  // NOTE: We could make IsCc be constant / CompileCFamily be templated on it
  // But that results in more ug than the tiny perf benefit is worth.
  bool IsCc;
  TFileInfo *Needs = nullptr;
  Base::TJson::TArray LinkNeeds;
};

}  // Job

}  // Bit
