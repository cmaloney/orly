/* <stig/rpc/type/addr.cc> 

   Implements <stig/rpc/type/addr.h>.

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

#include <stig/rpc/type/addr.h>

#include <stig/rpc/all_types.h>

using namespace std;
using namespace Stig::Rpc::Type;

TAddr::~TAddr() {
  assert(this);
  for (const auto &field: Fields) {
    delete const_cast<TType *>(field.second);
  }
}

void TAddr::AddField(TDir dir, TPtr &&type) {
  assert(this);
  assert(type);
  Fields.push_back(TField(dir, nullptr));
  Fields.back().second = type.release();
}

