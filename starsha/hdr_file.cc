/* <starsha/hdr_file.cc>

   Implements <starsha/hdr_file.h>.

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

#include <starsha/hdr_file.h>

#include <starsha/obj_file.h>

using namespace std;
using namespace Starsha;

const THdrFile::TKind *THdrFile::GetKind() const {
  return &Kind;
}

THdrFile::TFile *THdrFile::TryGetBuildableImplFile() const {
  assert(this);
  return (ImplFile && ImplFile->CanBuild()) ? ImplFile : 0;
}

THdrFile::~THdrFile() {}

void THdrFile::OnNew() {
  assert(this);
  TFile::OnNew();
  string path;
  ImplFile = TSpecificKind<TObjFile>::GetOrNewOutFile(GetCorpus(), TCorpus::ChangeExt(path, GetRelPath(), ".o"));
}

const THdrFile::TSpecificKind<THdrFile> THdrFile::Kind("a C/C++ header", ".h\0.hh\0");
