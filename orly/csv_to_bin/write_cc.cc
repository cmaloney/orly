/* <orly/csv_to_bin/write_cc.cc>

   Implements <orly/csv_to_bin/write_cc.h>.

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

#include <orly/csv_to_bin/write_cc.h>

using namespace std;
using namespace Orly::CsvToBin;

void Orly::CsvToBin::WriteCc(ostream &strm, const Symbol::TTable *table) {
  assert(&strm);
  assert(table);
  strm
      << "#include <orly/csv_to_bin/csv_parser.h>" << endl
      << "#include <orly/csv_to_bin/in.h>" << endl
      << "#include <orly/data/core_vector_generator.h>" << endl << endl
      << "struct TRecord final {" << endl
      << "};" << endl;
}
