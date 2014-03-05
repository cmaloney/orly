/* <io/device.cc>

   Implements <io/device.h>.

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

#include <io/device.h>

#include <base/io_utils.h>
#include <io/chunk_and_pool.h>

using namespace std;
using namespace Base;
using namespace Io;

TDevice::TTimeout::TTimeout()
    : runtime_error("timeout") {}

void TDevice::ConsumeOutput(const shared_ptr<const TChunk> &chunk) {
  assert(this);
  assert(&chunk);
  assert(chunk);
  const char *start, *limit;
  chunk->GetData(start, limit);
  WriteExactly(Fd, start, limit - start);
}

shared_ptr<const TChunk> TDevice::TryProduceInput() {
  assert(this);
  if (Timeout >= 0 && !Fd.IsReadable(Timeout)) {
    throw TTimeout();
  }
  auto chunk = Pool->AcquireChunk();
  if (!chunk->Store(Fd)) {
    chunk.reset();
  }
  return chunk;
}
