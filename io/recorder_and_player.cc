/* <io/recorder_and_player.cc>

   Implements <io/recorder_and_player.h>.

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

#include <io/recorder_and_player.h>

using namespace std;
using namespace Io;

void TRecorder::ConsumeOutput(const shared_ptr<const TChunk> &chunk) {
  assert(this);
  assert(chunk);
  Chunks.push_back(chunk);
}

void TRecorder::CopyOut(string &out) const {
  assert(this);
  assert(&out);
  size_t size = 0;
  for (const auto &chunk: Chunks) {
    size += chunk->GetSize();
  }
  out.clear();
  out.reserve(size);
  for (const auto &chunk: Chunks) {
    const char *start, *limit;
    chunk->GetData(start, limit);
    out.append(start, limit);
  }
}

shared_ptr<const TChunk> TPlayer::TryProduceInput() {
  assert(this);
  shared_ptr<const TChunk> chunk;
  if (Idx < Recorder->Chunks.size()) {
    chunk = Recorder->Chunks[Idx];
    ++Idx;
  } else if (NextChunkCb) {
    chunk = (*NextChunkCb)();
  }
  return chunk;
}
