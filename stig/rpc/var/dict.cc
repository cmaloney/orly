/* <stig/rpc/var/dict.cc> 

   Implements <stig/rpc/var/dict.h>.

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

#include <stig/rpc/var/dict.h>

#include <stig/rpc/all_vars.h>
#include <stig/rpc/type_mismatch_error.h>
#include <stig/rpc/type/cmp.h>
#include <stig/rpc/var/get_type.h>

using namespace std;
using namespace Stig::Rpc::Var;

TDict::~TDict() {
  assert(this);
  for (auto &elem: Elems) {
    delete const_cast<TVar *>(elem.first);
    delete const_cast<TVar *>(elem.second);
  }
}

void TDict::AddElem(TPtr &&key, TPtr &&val) {
  assert(this);
  assert(key);
  assert(val);
  auto key_type = GetType(*key);
  if (*key_type != *KeyType) {
    throw TTypeMismatchError();
  }
  auto val_type = GetType(*val);
  if (*val_type != *ValType) {
    throw TTypeMismatchError();
  }
  const TVar *&slot = Elems.insert(make_pair(key.get(), val.get())).first->second;
  if (slot != val.get()) {
    delete const_cast<TVar *>(slot);
    slot = val.get();
  }
  key.release();
  val.release();
}

