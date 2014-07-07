/* <base/error.cc>

   Implements <base/error.h>.

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

#include <base/error.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <base/no_default_case.h>
#include <util/error.h>

using namespace std;
using namespace Base;

TError::TError() : WhatPtr("PostCtor() was not called") {}

void TError::PostCtor(const TCodeLocation &code_location, const char *details) {
  assert(this);
  assert(&code_location);
  try {
    CodeLocation = code_location;
    stringstream out_strm;
    out_strm << CodeLocation << ", " << Demangle(GetTypeInfo());

    if (details) {
      out_strm << ", " << details;
    }

    WhatBuffer = out_strm.str();
    WhatPtr = WhatBuffer.c_str();
  } catch (...) {
    Util::Abort(HERE);
  }
}


void TError::PostCtor(const TCodeLocation &code_location, const char *details_start, const char* details_end) {
  assert(this);
  assert(&code_location);
  try {
    CodeLocation = code_location;
    stringstream out_strm;
    out_strm << CodeLocation << ", " << Demangle(GetTypeInfo());

    if (details_start) {
      out_strm << ", ";
      out_strm.write(details_start, details_end - details_start);
    }
    WhatBuffer = out_strm.str();
    WhatPtr = WhatBuffer.c_str();
  } catch (...) {
    Util::Abort(HERE);
  }
}