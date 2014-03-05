/* <tools/nycr/error.cc>

   Implements <tools/nycr/error.h>.

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

#include <tools/nycr/error.h>

#include <algorithm>
#include <vector>

using namespace Tools::Nycr;

void TError::DeleteEach() {
  while (FirstError) {
    TError *error = FirstError->NextError;
    delete FirstError;
    FirstError = error;
  }
  FirstError = 0;
  LastError = 0;
}

void TError::PrintSortedErrors(std::ostream &out) {
  //Build a vector out of the errors
  std::vector<const TError*> errors;

  for(const TError *cur = FirstError; cur; cur = cur->GetNextError()) {
    errors.push_back(cur);
  }

  std::sort(errors.begin(), errors.end(), [](const TError *first, const TError *second) {
    return first->GetPosRange().GetStart() < second->GetPosRange().GetStart();
  });

  for(auto error: errors) {
    out << error->GetPosRange() << ' ' << error->GetMsg() << std::endl;
  }


  //Sort the vector
  //Print out the errors
}


TError
    *TError::FirstError = 0,
    *TError::LastError  = 0;
