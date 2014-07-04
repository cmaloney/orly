/* <orly/csv_to_bin/csv_parser.cc>

   Implements <orly/csv_to_bin/csv_parser.h>.

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

#include <orly/csv_to_bin/csv_parser.h>

using namespace std;
using namespace Orly::CsvToBin;

void TCsvParser::Parse(TIn &strm) {
  assert(this);
  assert(&strm);
  enum {
    start_of_file, start_of_record, start_of_field, mid_field,
    end_of_field, end_of_record, end_of_record_pause, end_of_file
  } state = start_of_file;
  bool keep_going = true;
  do {
    switch (state) {
      case start_of_file: {
        OnFileStart();
        switch (strm->Kind) {
          case TIn::Byte:
          case TIn::EndOfField:
          case TIn::EndOfRecord: {
            state = start_of_record;
            break;
          }
          case TIn::EndOfFile: {
            state = end_of_file;
            break;
          }
        }
        break;
      }
      case start_of_record: {
        OnRecordStart();
        switch (strm->Kind) {
          case TIn::Byte:
          case TIn::EndOfField:
          case TIn::EndOfRecord: {
            state = start_of_field;
            break;
          }
          case TIn::EndOfFile: {
            state = end_of_record;
            break;
          }
        }
        break;
      }
      case start_of_field: {
        OnFieldStart();
        switch (strm->Kind) {
          case TIn::Byte:
          case TIn::EndOfField: {
            state = mid_field;
            break;
          }
          case TIn::EndOfRecord:
          case TIn::EndOfFile: {
            state = end_of_field;
            break;
          }
        }
        break;
      }
      case mid_field: {
        switch (strm->Kind) {
          case TIn::Byte: {
            OnByte(strm->Byte);
            ++strm;
            break;
          }
          case TIn::EndOfField:
          case TIn::EndOfRecord:
          case TIn::EndOfFile: {
            state = end_of_field;
            break;
          }
        }
        break;
      }
      case end_of_field: {
        OnFieldEnd();
        switch (strm->Kind) {
          case TIn::Byte: {
            abort();
          }
          case TIn::EndOfField: {
            ++strm;
            state = start_of_field;
            break;
          }
          case TIn::EndOfRecord:
          case TIn::EndOfFile: {
            state = end_of_record;
            break;
          }
        }
        break;
      }
      case end_of_record: {
        OnRecordEnd();
        switch (strm->Kind) {
          case TIn::Byte:
          case TIn::EndOfField: {
            abort();
          }
          case TIn::EndOfRecord: {
            ++strm;
            state = end_of_record_pause;
            break;
          }
          case TIn::EndOfFile: {
            state = end_of_file;
            break;
          }
        }
        break;
      }
      case end_of_record_pause: {
        /* In case you're wondering, this state exists only to deal with an
           edge case in RFC-4180.  That document says that the final record
           in a file may or may not end with a EOL.  That means that an EOL
           doesn't necessarily introduce another record.  If it's the last
           thing in the file, the EOL essentially means nothing.  We pause
           here, after parsing an EOL, to see if there is anything following
           it.  If so, it's the start of the next record; otherwise, it's the
           end of the file. */
        switch (strm->Kind) {
          case TIn::Byte:
          case TIn::EndOfField:
          case TIn::EndOfRecord: {
            state = start_of_record;
            break;
          }
          case TIn::EndOfFile: {
            state = end_of_file;
            break;
          }
        }
        break;
      }
      case end_of_file: {
        OnFileEnd();
        keep_going = false;
        break;
      }
    }
  } while (keep_going);
}
