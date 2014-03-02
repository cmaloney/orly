/* <stig/rpc/args.cc> 

   Implementation of <stig/rpc/args.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/args.h>

using namespace std;
using namespace Stig::Rpc;

TArgsReader::~TArgsReader() {}

bool TArgsReader::ReadArgs(TBlob::TReader &reader) const {
  assert(this);
  assert(&reader);
  TBlob::TElemCount size;
  reader.ReadNbo(size);
  bool keep_going = true;
  for (TBlob::TElemCount i = 0; keep_going && i < size; ++i) {
    string name;
    reader >> name;
    TArgKind kind;
    reader.ReadAsIs(kind);
    switch (kind) {
      case TArgKind::Bool: {
        bool val;
        reader >> val;
        keep_going = (*this)(name, val);
        break;
      }
      case TArgKind::Int: {
        int64_t val;
        reader >> val;
        keep_going = (*this)(name, val);
        break;
      }
      case TArgKind::Real: {
        double val;
        reader >> val;
        keep_going = (*this)(name, val);
        break;
      }
      case TArgKind::Str: {
        string val;
        reader >> val;
        keep_going = (*this)(name, val);
        break;
      }
      default: {
        throw 0;  // TODO
      }
    }
  }
  return keep_going;
}

