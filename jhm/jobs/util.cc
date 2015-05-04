/* <jhm/jobs/util.cc>

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

#include <jhm/jobs/util.h>

#include <jhm/env.h>
#include <jhm/file.h>
#include <util/stl.h>

using namespace Jhm;
using namespace std;
using namespace Util;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"

template <>
std::string Jhm::GetCmd<Tools::C>(const TConfig &config) {
  static const std::string cmd = config.Read<std::string>({"tools", "c"});
  assert(cmd == config.Read<std::string>({"tools", "c"}));
  return cmd;
}

template <>
std::string Jhm::GetCmd<Tools::Cc>(const TConfig &config) {
  static const std::string cmd = config.Read<std::string>({"tools", "c++"});
  assert(cmd == config.Read<std::string>({"tools", "c++"}));
  return cmd;
}

template <>
std::string Jhm::GetCmd<Tools::Link>(const TConfig &config) {
  static const std::string cmd = config.Read<std::string>({"tools", "link"});
  assert(cmd == config.Read<std::string>({"tools", "link"}));
  return cmd;
}

#pragma clang diagnostic pop

unordered_set<TFile *> Jhm::GetOutputSet(const vector<vector<string>> &out_exts,
                                         TEnv &env,
                                         const TRelPath &input) {
  unordered_set<TFile *> out_set;
  for(const auto &ext : out_exts) {
    InsertOrFail(out_set, env.GetFile(TRelPath(SwapExtension(Base::TPath(input.Path), {ext}))));
  }
  return out_set;
}

TFile *Jhm::GetOutputWithExtension(unordered_set<TFile *> output_set, const vector<string> &ext) {
  TFile *primary_output = nullptr;
  for(TFile *f : output_set) {
    if(f->GetRelPath().Path.EndsWith(ext)) {
      return primary_output = f;
    }
  }
  assert(false);
  __builtin_unreachable();
}
