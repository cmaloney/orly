/* <starsha/c_family_file.h>

   TODO

   Copyright 2010-2014 Stig LLC

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
#include <string>
#include <vector>

#include <base/no_copy_semantics.h>
#include <starsha/corpus.h>

namespace Starsha {

  /* TODO */
  class THdrFile;
  class TObjFile;

  /* TODO */
  class TCFamilyFile
      : public TCorpus::TFile {
    NO_COPY_SEMANTICS(TCFamilyFile);
    public:

    /* TODO */
    virtual void CollectImplFiles(std::unordered_set<TFile *> &impl_files);

    protected:

    /* TODO */
    TCFamilyFile(TCorpus *corpus, const std::string &rel_path, const char *cmd, const char *gnu_cmd)
        : TFile(corpus, rel_path), Cmd(cmd), GnuCmd(gnu_cmd) {
      Init();
    }

    /* TODO */
    TCFamilyFile(TCorpus *corpus, const std::string &rel_path, time_t mod_time, const char *cmd, const char *gnu_cmd)
        : TFile(corpus, rel_path, mod_time), Cmd(cmd), GnuCmd(gnu_cmd) {
      Init();
    }

    /* TODO */
    virtual ~TCFamilyFile();

    private:

    /* TODO */
    const char *Cmd;
    const char *GnuCmd;

    /* TODO */
    void Init();

    /* TODO */
    virtual bool ForEachPrereq(const std::function<bool (TFile *)> &cb);

    /* TODO */
    virtual void OnExists();

    /* TODO */
    virtual void Produce(bool run, const std::unordered_set<TFile *> &targets, std::unordered_set<TFile *> &products);

    /* TODO */
    bool ReadNote();

    /* TODO */
    TObjFile *ObjFile;

    /* TODO */
    std::string Flags;

    /* TODO */
    std::vector<THdrFile *> HdrFiles;

  };  // TCFamilyFile

}  // Starsha
