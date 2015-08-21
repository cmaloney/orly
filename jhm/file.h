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

#include <string>

#include <base/path.h>
#include <jhm/config.h>
#include <jhm/naming.h>
#include <util/time.h>

namespace Jhm {

/* A file is a lightweight proxy object for actual data grabbers (which for now live independently).
   It's simply used
   to track unique paths inside the system, as well as if we believe a path can be built.
   Getting/storing data in a
   file is entirely seperate.

   Config relating to a file can be retrieved from the file.

   TODO: make the config for file point to the config of the environment as fallback. */
class TFile {
  public:
  TFile(TRelPath &&path, const TTree *tree, bool is_src, const std::string &config_filename);

  const TRelPath &GetRelPath() const;
  const std::string &GetPath() const;

  /* Computes (and doesn't cache!) the timestamp for the given file. Newest of either it's config or
   * file in the tree) */
  Util::TOptTimestamp GetTimestamp() const;

  void PushComputedConfig(Base::TJson &&config);

  void WriteConfig(std::ostream &out);

  void LoadConfig(const std::string &filename);

  void SetComputed(std::vector<Base::TJson> &&config);

  const TConfig &GetConfig() const;

  bool IsSrc() const;

  // TODO: GetGitHash for uniquely iding src files. Out files are id'd by job kind + input kind.

  private:
  bool IsSrc_;
  TRelPath Path;
  std::string CmdPath;

  // Config: Stack of file-based config for the file (file.jhm), as well as producer-added config.
  // NOTE: JHM cannont and will not allow generated config. That isn't how you should pass in that
  // info.
  TConfig Config;
};

std::ostream &operator<<(std::ostream &out, TFile *f);

}
