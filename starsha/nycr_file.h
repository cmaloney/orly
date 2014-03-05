/* <starsha/nycr_file.h>

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
  class TNycrFile
      : public TCorpus::TFile {
    NO_COPY_SEMANTICS(TNycrFile);
    public:

    /* TODO */
    virtual const TKind *GetKind() const;

    private:

    /* TODO */
    TNycrFile(TCorpus *corpus, const std::string &rel_path)
        : TFile(corpus, rel_path) {}

    /* TODO */
    TNycrFile(TCorpus *corpus, const std::string &rel_path, time_t mod_time)
        : TFile(corpus, rel_path, mod_time) {}

    /* TODO */
    virtual ~TNycrFile();

    /* TODO */
    virtual void OnExists();

    /* TODO */
    virtual void Produce(bool run, const std::unordered_set<TFile *> &targets, std::unordered_set<TFile *> &products);

    /* TODO */
    bool ReadNote();

    /* TODO */
    std::unordered_set<TFile *> Targets;

    /* TODO */
    std::vector<std::string> Languages;

    /* TODO */
    static const TSpecificKind<TNycrFile> Kind;

    /* TODO */
    friend class TSpecificKind<TNycrFile>;

  };  // TNycrFile

}  // Starsha
