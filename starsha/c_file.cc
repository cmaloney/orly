/* <starsha/c_file.cc>

   Implements <starsha/c_file.h>.

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

#include <starsha/c_file.h>

using namespace Starsha;

const TCFile::TKind *TCFile::GetKind() const {
  return &Kind;
}

TCFile::~TCFile() {}

const TCFile::TSpecificKind<TCFile> TCFile::Kind("a C source", ".c\0", ".o\0");
