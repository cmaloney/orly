/* <orly/csv_to_bin/write_orly.h>

   Write Orly script as a starter package for a csv import.

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

#include <orly/code_gen/cpp_printer.h>
#include <orly/csv_to_bin/symbol/kit.h>

namespace Orly {

  namespace CsvToBin {

    /* Write Orly script as a starter package for a csv import. */
    void WriteOrly(CodeGen::TCppPrinter &strm, const Symbol::TTable *table);

    /* Write Orly script object type to represent all the fields in the given. */
    void WriteValObjectTypeForRow(CodeGen::TCppPrinter &strm, const Symbol::TTable *table);

    /* Write Orly script object type to represent the fields in the table that are not part of the given key. */
    void WriteValObjectTypeForKey(CodeGen::TCppPrinter &strm, const Symbol::TTable *table, const Symbol::TKey *key, const char *name);

    /* Write Orly script object value with the fields in the table that are not part of the given key. */
    void WriteValObjectForKey(CodeGen::TCppPrinter &strm, const Symbol::TTable *table, const Symbol::TKey *key);

    /* Write Orly script type for the given Symbol Type. */
    void PrintOrlyType(CodeGen::TCppPrinter &strm, Symbol::TType type, bool is_null);

  }  // Csv2Bin

}  // Orly
