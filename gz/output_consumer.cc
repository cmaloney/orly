/* <gz/output_consumer.cc>

   Implements <gz/output_consumer.h>.

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

#include <gz/output_consumer.h>

#include <cassert>

using namespace std;
using namespace Gz;

void TOutputConsumer::ConsumeOutput(const shared_ptr<const TChunk> &chunk) {
  assert(this);
  assert(&chunk);
  const char *start, *limit;
  chunk->GetData(start, limit);
  while (start < limit) {
    size_t size = File.WriteAtMost(start, limit - start);
    start += size;
  }
}
