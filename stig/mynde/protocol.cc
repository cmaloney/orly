/* <stig/mynde/proto.cc>

   Implements <stig/mynde/proto.h>

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

#include <stig/mynde/protocol.h>

#include <cassert>

#include <stig/mynde/binary_protocol.h>
#include <strm/bin/in.h>
#include <strm/bin/out.h>

using namespace Stig::Mynde;

TBuffer::TBuffer() : Size(0), Data(nullptr) {}

TBuffer::~TBuffer() {
  if (Data) {
    delete [] Data;
  }
}

uint32_t TBuffer::GetSize() const {
  assert(this);
  return Size;
}

const uint8_t *TBuffer::GetData() const {
  assert(this);

  assert(Data);

  return Data;
}

void TBuffer::Fill(uint32_t size, Strm::Bin::TIn &in) {
  assert(this);

  // Buffers may only be filled once
  assert(!Data);
  assert(Size == 0);

  // Make and fill the buffer
  Size = size;
  Data = new uint8_t[Size];
  in.Read(Data, Size);
}

Strm::Bin::TOut &Stig::Mynde::operator<<(Strm::Bin::TOut &out, const TBuffer &that) {
  assert(&out);
  assert(&that);
  out.Write(that.GetData(), that.GetSize());

  return out;
}
