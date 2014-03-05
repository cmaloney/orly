/* <starsha/bison_file.cc>

   Implements <starsha/bison_file.h>.

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

#include <starsha/bison_file.h>

#include <cstring>
#include <iostream>

#include <starsha/cc_file.h>
#include <starsha/hdr_file.h>
#include <starsha/runner.h>
#include <starsha/string_builder.h>

using namespace std;
using namespace Base;
using namespace Starsha;

const TBisonFile::TKind *TBisonFile::GetKind() const {
  return &Kind;
}

TBisonFile::~TBisonFile() {}

void TBisonFile::OnNew() {
  assert(this);
  struct product_t {
    const char *Ext;
    const TKind *Kind;
  };
  static const size_t product_cnt = 3;
  const product_t products[product_cnt] = {
    { ".bison.cc", TSpecificKind<TCcFile>::GetKind() },
    { ".bison.h",  TSpecificKind<THdrFile>::GetKind() },
    { ".bison.hh", TSpecificKind<THdrFile>::GetKind() }
  };
  for (size_t i = 0; i < product_cnt; ++i) {
    const product_t &product = products[i];
    string path;
    auto file = GetCorpus()->TryGetFile(TCorpus::ChangeExt(path, GetRelPath(), product.Ext));
    if (!file) {
      file = product.Kind->NewOutFile(GetCorpus(), path);
    } else if (file->GetKind() != product.Kind) {
      THROW
          << '"' << path << "\" seems to be produced by \"" << GetRelPath() << "\" but is the wrong kind of file; expected "
          << product.Kind->GetDesc() << " but got " << file->GetKind()->GetDesc();
    }
    file->SetProducer(this);
    Targets.insert(file);
    if (product.Kind == TSpecificKind<TCcFile>::GetKind()) {
      PrimaryTarget = file;
    }
  }
  assert(PrimaryTarget);
}

void TBisonFile::Produce(bool run, const unordered_set<TFile *> &, unordered_set<TFile *> &products) {
  assert(this);
  if (run) {
    string cmd_line;
    TStringBuilder(cmd_line) << "bison -rall -o" << PrimaryTarget->GetAbsPath() << ' ' << GetAbsPath();
    vector<string> lines;
    Run(cmd_line, lines);
  }
  products = Targets;
}

const TBisonFile::TSpecificKind<TBisonFile> TBisonFile::Kind("a Bison source", ".y\0");
