/* <jhm/jobs/util.h>

   Misc. utility functions for jobs

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

#include <unordered_set>

#include <base/opt.h>
#include <jhm/naming.h>

namespace Jhm {
  class TFile;
  class TEnv;

  std::unordered_set<TFile *> GetOutputSet(const std::vector<std::vector<std::string>> &out_exts,
                                           TEnv &env,
                                           const TRelPath &input);
  TFile *GetOutputWithExtension(std::unordered_set<TFile *> output_set, const std::vector<std::string> &ext);
}