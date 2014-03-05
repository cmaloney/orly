/* <starsha/obj_file.cc>

   Implements <starsha/obj_file.h>.

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

#include <starsha/obj_file.h>

#include <iostream>
#include <sstream>

#include <starsha/runner.h>
#include <starsha/string_builder.h>

using namespace std;
using namespace Starsha;

void TObjFile::CollectImplFiles(unordered_set<TFile *> &impl_files) {
  assert(this);
  assert(&impl_files);
  if (impl_files.insert(this).second) {
    auto producer = TryGetProducer();
    if (producer) {
      producer->CollectImplFiles(impl_files);
    }
  }
}

const TObjFile::TKind *TObjFile::GetKind() const {
  return &Kind;
}

TObjFile::~TObjFile() {}

bool TObjFile::ForEachPrereq(const function<bool (TFile *)> &cb) {
  assert(this);
  assert(&cb);
  unordered_set<TFile *> impl_files;
  CollectImplFiles(impl_files);
  for (auto file: impl_files) {
    if (!cb(file)) {
      return false;
    }
  }
  return true;
}

void TObjFile::Init() {
  assert(this);
  auto tool = TryGetToolConfigNote("ld");
  if (tool) {
    auto flags = tool->TryGetChild("flags");
    if (flags) {
      ostringstream strm;
      flags->ForEachChild([&strm](const TNote *child) {
        strm << ' ' << child->GetKey();
        return true;
      });
      Flags = strm.str();
    }
    auto libs = tool->TryGetChild("libs");
    if (libs) {
      ostringstream strm;
      libs->ForEachChild([&strm](const TNote *child) {
        strm << ' ' << child->GetKey();
        return true;
      });
      Libs = strm.str();
    }
  }
}

void TObjFile::Produce(bool run, const unordered_set<TFile *> &targets, unordered_set<TFile *> &products) {
  assert(this);
  assert(&targets);
  assert(&products);
  if (targets.size() != 1) {
    THROW << '"' << GetRelPath() << "\" must produce one target at a time";
  }
  if (run) {
    TFile *target = *targets.begin();
    assert(target);
    unordered_set<TFile *> impl_files;
    CollectImplFiles(impl_files);
    string cmd_line;
    TStringBuilder(cmd_line) << "g++" << Flags << " -o" << target->GetAbsPath() << impl_files << Libs;
    vector<string> lines;
    Run(cmd_line, lines);
  }
  products = targets;
}

const TObjFile::TSpecificKind<TObjFile> TObjFile::Kind("an object module", ".o\0", ".\0");
