/* <orly/csv_to_bin/in.cc>

   Implements <orly/csv_to_bin/in.h>.

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

#include <orly/csv_to_bin/in.h>

using namespace std;
using namespace Orly::CsvToBin;

const TIn::TOptions TIn::DefaultOptions = { ',', '"', false };

void TIn::Update() {
  assert(this);
  for (;;) {
    /* Peek at the next byte.  If there is none, we've reached end-of-file;
       otherwise, pop it. */
    const uint8_t *ptr = TryPeek();
    if (!ptr) {
      Cache.Kind = EndOfFile;
      Cache.Byte = 0u;
      break;
    }
    uint8_t c = Pop();
    /* If we're in a quoted range, we have fewer special cases to handle. */
    if (Quoted) {
      /* Any non-quote character inside of quotes is just a byte. */
      if (c != Options.Quote) {
        Cache.Kind = Byte;
        Cache.Byte = c;
        break;
      }
      /* A quote inside of quotes might be the start of a quote-quote. */
      ptr = TryPeek();
      if (ptr && *ptr == Options.Quote) {
        /* This is quoted quote-quote, so we report a single byte which is
           the quote byte. */
        Skip();
        Cache.Kind = Byte;
        Cache.Byte = c;
        break;
      }
      /* This quote marks the end of a quoted range.  Keep scanning. */
      Quoted = false;
      continue;
    }
    /* If this is the start of a quoted range, keep scanning. */
    if (c == Options.Quote) {
      Quoted = true;
      continue;
    }
    /* If this is a delimiter, report an end-of-field. */
    if (c == Options.Delim) {
      Cache.Kind = EndOfField;
      Cache.Byte = 0u;
      break;
    }
    /* If we're not doing Unix line endings, a CRLF is an EOL. */
    if (!Options.UnixEol && c == '\r') {
      ptr = TryPeek();
      if (ptr && *ptr == '\n') {
        Skip();
        Cache.Kind = EndOfRecord;
        Cache.Byte = 0u;
        break;
      }
    }
    /* If we are doing Unix line endings, a LF is an EOL. */
    if (Options.UnixEol && c == '\n') {
      Cache.Kind = EndOfRecord;
      Cache.Byte = 0u;
      break;
    }
    /* Anything else is just a byte to be reported. */
    Cache.Kind = Byte;
    Cache.Byte = c;
    break;
  }  // forever
}
