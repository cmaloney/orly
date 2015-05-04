/* <base/code_location.cc>

   Implements <base/code_location.h>.

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

#include <base/code_location.h>

#include <cassert>
#include <cstring>
#include <ostream>

using namespace std;
using namespace Base;

// NOTE: Calculating this once and caching it is perf critical.
static size_t GetSrcRootLen() {
  static size_t len = strlen(TCodeLocation::SrcRoot);
  return len;
}

TCodeLocation::TCodeLocation() : File(""), LineNumber(1) {}

TCodeLocation::TCodeLocation(const char *file, unsigned line_number)
    : File(file), LineNumber(line_number) {
  assert(file);
  assert(line_number);
}

bool TCodeLocation::operator==(const TCodeLocation &that) const {
  return LineNumber == that.LineNumber && strcmp(File, that.File) == 0;
}

const char *TCodeLocation::GetFile() const {
  assert(this);
  return File + ((strncmp(File, SrcRoot, GetSrcRootLen()) == 0) ? (GetSrcRootLen()) : 0);
}

unsigned TCodeLocation::GetLineNumber() const {
  assert(this);
  return LineNumber;
}

void TCodeLocation::Write(ostream &strm) const {
  assert(this);
  assert(&strm);
  strm << '[' << GetFile() << ", " << LineNumber << ']';
}

const char *TCodeLocation::SrcRoot = SRC_ROOT;

/* Standard stream inserter for Base::TCodeLocation. */
std::ostream &Base::operator<<(std::ostream &strm, const Base::TCodeLocation &that) {
  assert(&that);
  that.Write(strm);
  return strm;
}
