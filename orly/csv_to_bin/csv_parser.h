/* <orly/csv_to_bin/csv_parser.h>

   A parser for data in CSV format.

   See RFC-4180 for details. (http://tools.ietf.org/html/rfc4180)

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

#include <cassert>

#include <orly/csv_to_bin/in.h>

namespace Orly {

  namespace CsvToBin {

    /* A parser for data in CSV format. */
    class TCsvParser {
      public:

      /* Do-little. */
      virtual ~TCsvParser() {}

      /* Consume the given stream to its end, parsing as we go.  We'll call
         the event handlers below as needed. */
      void Parse(TIn &strm);

      protected:

      /* Do-little. */
      TCsvParser() {}

      /* Always called before any other parsing, even for an empty file. */
      virtual void OnFileStart() {}

      /* Called at the start of each record.  Called at least once for any
         non-empty file. */
      virtual void OnRecordStart() {}

      /* Called at the start of each field.  Called at least once for any
         record, empty or not.  (An empty record is considered to be a record
         containing a single, empty field.) */
      virtual void OnFieldStart() {}

      /* Called for each byte in a field.  This is not called at all for an
         empty field. */
      virtual void OnByte(uint8_t /*byte*/) {}

      /* Called at the end of each field. */
      virtual void OnFieldEnd() {}

      /* Called at the end of each record. */
      virtual void OnRecordEnd() {}

      /* Always called after all other parsing, even for an empty file. */
      virtual void OnFileEnd() {}

    };  // TCsvParser

  }  // Csv2Bin

}  // Orly
