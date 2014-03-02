/* <stig/rpc/var/obj.cc> 

   Implements <stig/rpc/var/obj.h>.

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

#include <stig/rpc/var/obj.h>

#include <stig/rpc/all_vars.h>

using namespace std;
using namespace Stig::Rpc::Var;

TObj::~TObj() {
  assert(this);
  for (auto &field: Fields) {
    delete const_cast<TVar *>(field.second);
  }
}

void TObj::AddField(string &&name, TPtr &&var) {
  assert(this);
  assert(var);
  const TVar *&slot = Fields.insert(make_pair(move(name), var.get())).first->second;
  if (slot != var.get()) {
    delete const_cast<TVar *>(slot);
    slot = var.get();
  }
  var.release();
}

