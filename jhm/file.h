/* <jhm/file.h>

   A registry of file types (Generally based on extension).

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

#include <string>

#include <jhm/config.h>
#include <jhm/naming.h>

namespace Jhm {
  class TEnv;
  class TJob;

  /* A file is a lightweight proxy object for actual data grabbers (which for now live independently). It's simply used
     to track unique paths inside the system, as well as if we believe a path can be built. Getting/storing data in a
     file is entirely seperate.

     Config relating to a file can be retrieved from the file.

     TODO: make the config for file point to the config of the environment as fallback. */
  class TFile {
    public:
    TFile(TAbsPath &&path, bool is_src, Base::TJson &&config)
        : Src(is_src), Path(std::move(path)), Config(std::move(config)) {}

    const TAbsPath &GetPath() const {
      return Path;
    }

    void PushComputedConfig(Base::TJson &&config) {
      assert(this);
      Config.AddComputed(std::move(config));
    }

    void WriteConfig(std::ostream &out) {
      assert(this);
      Config.WriteComputed(out);
    }

    void LoadConfig(const std::string &filename) {
      assert(this);
      Config.LoadComputed(filename);
    }

    const TConfig &GetConfig() const  {
      return Config;
    }

    bool IsSrc() const {
      return Src;
    }

    //TODO: GetGitHash for uniquely iding src files. Out files are id'd by job kind + input kind.

    private:
    bool Src;
    TAbsPath Path;

    // Config: Stack of file-based config for the file (file.jhm), as well as producer-added config.
    // NOTE: JHM cannont and will not allow generated config. That isn't how you should pass in that info.
    // TODO: Allow attaching things like producer config in a uniform manner.
    // TODO: Out files are defined by their producer id (in_file, producer)
    TConfig Config;
  };

  inline std::ostream &operator<<(std::ostream &out, TFile *f) {
    out << f->GetPath().GetRelPath();

    return out;
  }
}