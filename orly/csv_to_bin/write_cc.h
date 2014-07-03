/* <orly/csv_to_bin/write_cc.h>

   Write C++ to convert a CSV table into an Orly binary core vector.

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

#include <ostream>

#include <orly/csv_to_bin/symbol/kit.h>

namespace Orly {

  namespace CsvToBin {

    /* Write C++ to convert a CSV table into an Orly binary core vector. */
    void WriteCc(std::ostream &strm, const Symbol::TTable *table);

    /* Write C++ struct to represent the fields in the table that are not part of the given key. */
    void WriteValRecordForKey(std::ostream &strm, const Symbol::TTable *table, const Symbol::TKey *key, const char *name);

    /* Write C++ tuple type to represent the key. */
    void WriteKeyTupleType(std::ostream &strm, const Symbol::TKey *key);

    /* Make a typedef for the key's tuple type. */
    void DefKeyTupleType(std::ostream &strm, const Symbol::TKey *key, const char *name);

    /* Write C++ type for the given Symbol type. */
    void PrintType(std::ostream &strm, Symbol::TType type, bool is_null);

  }  // Csv2Bin

}  // Orly
