/* <orly/csv_to_bin/level2.cc>

   Implements <orly/csv_to_bin/level2.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/csv_to_bin/level2.h>

#include <algorithm>
#include <new>

using namespace std;
using namespace Orly::CsvToBin;

const size_t
    TLevel2::MinBufferSize = 0x100,
    TLevel2::DefBufferSize = 0x10000;

TLevel2::TLevel2(TLevel1 &level1, size_t buffer_size)
    : Level1(level1), IsCached(false), NextState(StartOfFile) {
  buffer_size = max(buffer_size, MinBufferSize);
  Start = static_cast<uint8_t *>(malloc(buffer_size));
  if (!Start) {
    throw bad_alloc();
  }
  Limit = Start + buffer_size;
}

TLevel2::~TLevel2() {
  assert(this);
  free(Start);
}

void TLevel2::Update() {
  assert(this);
  Cache.State = NextState;
  Cache.Start = nullptr;
  Cache.Limit = nullptr;
  switch (NextState) {
    case StartOfFile: {
      switch (Level1->State) {
        case TLevel1::Byte:
        case TLevel1::EndOfField:
        case TLevel1::EndOfRecord: {
          NextState = StartOfRecord;
          break;
        }
        case TLevel1::EndOfFile: {
          NextState = EndOfFile;
          break;
        }
      }
      break;
    }
    case StartOfRecord: {
      NextState = StartOfField;
      break;
    }
    case StartOfField: {
      switch (Level1->State) {
        case TLevel1::Byte:
        case TLevel1::EndOfField: {
          NextState = Bytes;
          break;
        }
        case TLevel1::EndOfRecord:
        case TLevel1::EndOfFile: {
          NextState = EndOfField;
          break;
        }
      }
      break;
    }
    case Bytes: {
      uint8_t *cursor = Start;
      do {
        switch (Level1->State) {
          case TLevel1::Byte: {
            *cursor++ = Level1->Byte;
            ++Level1;
            break;
          }
          case TLevel1::EndOfField:
          case TLevel1::EndOfRecord:
          case TLevel1::EndOfFile: {
            NextState = EndOfField;
            break;
          }
        }
      } while (cursor < Limit && NextState == Bytes);
      Cache.Start = Start;
      Cache.Limit = cursor;
      break;
    }
    case EndOfField: {
      switch (Level1->State) {
        case TLevel1::Byte: {
          abort();
        }
        case TLevel1::EndOfField: {
          ++Level1;
          NextState = StartOfField;
          break;
        }
        case TLevel1::EndOfRecord:
        case TLevel1::EndOfFile: {
          NextState = EndOfRecord;
          break;
        }
      }
      break;
    }
    case EndOfRecord: {
      switch (Level1->State) {
        case TLevel1::Byte:
        case TLevel1::EndOfField: {
          abort();
        }
        case TLevel1::EndOfRecord: {
          ++Level1;
          /* RFC-4180 says that the final record in a file may or may not end
             with an EOL.  That means that an EOL doesn't necessarily
             introduce another record.  If it's the last thing in the file,
             an EOL essentially means nothing. */
          switch (Level1->State) {
            case TLevel1::Byte:
            case TLevel1::EndOfField:
            case TLevel1::EndOfRecord: {
              NextState = StartOfRecord;
              break;
            }
            case TLevel1::EndOfFile: {
              NextState = EndOfFile;
              break;
            }
          }
          break;
        }
        case TLevel1::EndOfFile: {
          NextState = EndOfFile;
          break;
        }
      }
      break;
    }
    case EndOfFile: {
      break;
    }
  }
}

ostream &operator<<(ostream &strm, TLevel2::TState that) {
  assert(&strm);
  const char *name;
  switch (that) {
    case TLevel2::StartOfFile: {
      name = "StartOfFile";
      break;
    }
    case TLevel2::StartOfRecord: {
      name = "StartOfRecord";
      break;
    }
    case TLevel2::StartOfField: {
      name = "StartOfField";
      break;
    }
    case TLevel2::Bytes: {
      name = "Bytes";
      break;
    }
    case TLevel2::EndOfField: {
      name = "EndOfField";
      break;
    }
    case TLevel2::EndOfRecord: {
      name = "EndOfRecord";
      break;
    }
    case TLevel2::EndOfFile: {
      name = "EndOfFile";
      break;
    }
  }
  return strm << name;
}
