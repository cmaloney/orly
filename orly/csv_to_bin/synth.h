/* <orly/csv_to_bin/synth.h>

   Construct a new table symbol (along with its columns and keys) from
   an SQL-like source text.

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

#pragma once

#include <memory>
#include <ostream>

#include <orly/csv_to_bin/symbol/kit.h>

namespace Orly {

  namespace CsvToBin {

    /* Construct a new table symbol (along with its columns and keys) from
       an SQL-like source text.  If there are errors, write the diagnostic
       messages to the given stream and return null. */
    std::unique_ptr<Symbol::TTable> NewTable(
        std::ostream &strm, const char *src_text);

  }  // Csv2Bin

}  // Orly