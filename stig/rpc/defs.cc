/* <stig/rpc/defs.cc> 

   Implements <stig/rpc/defs.h>.

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

#include <stig/rpc/defs.h>

using namespace Stig::Rpc;

int Stig::Rpc::CmpDirs(TDir lhs, TDir rhs) {
  if (lhs == TDir::Asc && rhs == TDir::Desc) {
    return -1;
  }
  if (lhs == TDir::Desc && rhs == TDir::Asc) {
    return +1;
  }
  return 0;
}

const char *Stig::Rpc::GetDirName(TDir dir) {
  const char *result;
  switch (dir) {
    case TDir::Asc: {
      result = "asc";
      break;
    }
    case TDir::Desc: {
      result = "desc";
      break;
    }
  }
  return result;
}

