/* <starsha/nycr_file.cc>

   Implements <starsha/nycr_file.h>.

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

#include <starsha/nycr_file.h>

#include <cstring>
#include <iostream>

#include <starsha/bison_file.h>
#include <starsha/cc_file.h>
#include <starsha/flex_file.h>
#include <starsha/hdr_file.h>
#include <starsha/runner.h>
#include <starsha/string_builder.h>

using namespace std;
using namespace Base;
using namespace Starsha;

const TNycrFile::TKind *TNycrFile::GetKind() const {
  return &Kind;
}

TNycrFile::~TNycrFile() {}

void TNycrFile::OnExists() {
  assert(this);
  if (!ReadNote()) {
    TNote *note = GetCorpus()->GetNote()->GetChild("files")->ReplaceOrNewChild(GetRelPath().c_str());
    note->SetChildValueAs("mod_time", *GetModTime());
    note = note->GetOrNewChild("languages");
    /* extra: build and run the command, capturing its output lines */ {
      string cmd_line;
      TStringBuilder(cmd_line) << GetCorpus()->GetBootstrap() << "/tools/nycr/nycr -l " << GetAbsPath();
      Run(cmd_line, Languages);
    }
    for (string &language: Languages) {
      new TNote(note, language);
    }
  }
  char path[PATH_MAX];
  strcpy(path, GetRelPath().c_str());
  char *end_of_root = strrchr(path, '.');
  if (end_of_root) {
    ++end_of_root;
  } else {
    end_of_root = path + strlen(path);
    *end_of_root++ = '.';
  }
  for (string &language: Languages) {
    strcpy(end_of_root, language.c_str());
    char *end_of_lang = path + strlen(path);
    strcpy(end_of_lang, ".y");
    TFile *file = TSpecificKind<TBisonFile>::GetOrNewOutFile(GetCorpus(), path);
    file->SetProducer(this);
    Targets.insert(file);
    strcpy(end_of_lang, ".l");
    file = TSpecificKind<TFlexFile>::GetOrNewOutFile(GetCorpus(), path);
    file->SetProducer(this);
    Targets.insert(file);
    strcpy(end_of_lang, ".cst.cc");
    file = TSpecificKind<TCcFile>::GetOrNewOutFile(GetCorpus(), path);
    file->SetProducer(this);
    Targets.insert(file);
    strcpy(end_of_lang, ".cst.h");
    file = TSpecificKind<THdrFile>::GetOrNewOutFile(GetCorpus(), path);
    file->SetProducer(this);
    Targets.insert(file);
    strcpy(end_of_lang, ".dump.cc");
    file = TSpecificKind<TCcFile>::GetOrNewOutFile(GetCorpus(), path);
    file->SetProducer(this);
    Targets.insert(file);
  }
}

void TNycrFile::Produce(bool run, const unordered_set<TFile *> &, unordered_set<TFile *> &products) {
  assert(this);
  if (run) {
    char path[PATH_MAX];
    strcpy(path, GetRelPath().c_str());
    const char *branch;
    char *atom = strrchr(path, '/');
    if (atom) {
      branch = path;
      *atom = '\0';
      ++atom;
    } else {
      branch = "";
      atom = path;
    }
    char *atom_end = strchr(atom, '.');
    if (atom_end) {
      *atom_end = '\0';
    }
    string root = GetCorpus()->GetOut();
    root += "/";
    root += branch;
    string cmd_line;
    TStringBuilder(cmd_line)
        << GetCorpus()->GetBootstrap() << "/tools/nycr/nycr -a " << atom << " -p " << branch << " -r " << root << ' ' << GetAbsPath();
    vector<string> lines;
    Run(cmd_line, lines);
  }
  products = Targets;
}

bool TNycrFile::ReadNote() {
  assert(this);
  TNote *note = GetCorpus()->GetNote()->GetChild("files")->TryGetChild(GetRelPath().c_str());
  if (!note) {
    return false;
  }
  time_t mod_time;
  note->GetChildValueAs("mod_time", mod_time);
  if (mod_time < *GetModTime()) {
    return false;
  }
  note->GetChild("languages")->ForEachChild([this](TNote *child){
    Languages.push_back(child->GetKey());
    return true;
  });
  return true;
}

const TNycrFile::TSpecificKind<TNycrFile> TNycrFile::Kind("a Nycr source", ".nycr\0");
