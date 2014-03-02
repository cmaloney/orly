/* <stig/rpc/var/opt.cc> 

   Implements <stig/rpc/var/opt.h>.

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

#include <stig/rpc/var/opt.h>

#include <stig/rpc/all_vars.h>
#include <stig/rpc/type_mismatch_error.h>
#include <stig/rpc/type/cmp.h>
#include <stig/rpc/var/get_type.h>

using namespace std;
using namespace Stig::Rpc::Var;

TOpt::~TOpt() {
  assert(this);
  delete Elem;
}

void TOpt::AddElem(TPtr &&elem) {
  assert(this);
  assert(elem);
  auto elem_type = GetType(*elem);
  if (*elem_type != *ElemType) {
    throw TTypeMismatchError();
  }
  TVar *temp = elem.release();
  swap(Elem, temp);
  delete temp;
}

