/* <stig/atom/core_vector.cc>

   Implements <stig/atom/core_vector.h>.

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

#include <stig/atom/core_vector.h>

#include <cstdint>
#include <cstdlib>
#include <new>

using namespace std;
using namespace Io;
using namespace Stig::Atom;

TCoreVector::TCoreVector(TBinaryInputStream &strm) {
  assert(&strm);
  Arena = new TPackedArena(strm);
  uint32_t size;
  strm >> size;
  Cores.resize(size);
  strm.ReadExactly(&Cores[0], size * sizeof(TCore));
}

TCoreVector::~TCoreVector() {
  assert(this);
  delete Arena;
}

TCoreVector::TPackedArena::TPackedArena(TBinaryInputStream &strm)
    : TCore::TArena(false) {
  assert(&strm);
  uint32_t raw_size;
  strm /*>> Offsets*/ >> raw_size;
  RawSize = raw_size;
  RawData = static_cast<char *>(malloc(RawSize));
  if (!RawData) {
    throw bad_alloc();
  }
  try {
    strm.ReadExactly(RawData, RawSize);
  } catch (...) {
    free(RawData);
    throw;
  }
}

TCoreVector::TPackedArena::~TPackedArena() {
  assert(this);
  free(RawData);
}

void TCoreVector::TPackedArena::ReleaseNote(const TNote *, TOffset, void *, void *, void *) {}

const TCoreVector::TNote *TCoreVector::TPackedArena::TryAcquireNote(TOffset offset, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  return reinterpret_cast<TNote *>(RawData + offset);
}

const TCoreVector::TNote *TCoreVector::TPackedArena::TryAcquireNote(TOffset offset, size_t /*known_size*/, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  return reinterpret_cast<TNote *>(RawData + offset);
}