/* <orly/csv_to_bin/level1.cc>

   Implements <orly/csv_to_bin/level1.h>.

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

#include <orly/csv_to_bin/level1.h>

using namespace std;
using namespace Orly::CsvToBin;

const TLevel1::TOptions TLevel1::DefaultOptions = {
  ',', '"', true, true, '\\', true
};

void TLevel1::Update() {
  assert(this);
  for (;;) {
    /* Peek at the next byte.  If there is none, we've reached end-of-file;
       otherwise, pop it. */
    const uint8_t *ptr = TryPeek();
    if (!ptr) {
      Cache.State = EndOfFile;
      Cache.Byte = 0u;
      break;
    }
    uint8_t c = Pop();
    /* If we're in a quoted range, we have other special cases to handle. */
    if (Quoted) {
      /* If we're at an escape, the next byte is to be taken literally.
         If there's is no next byte, just take the escape byte itself
         literally. */
      if (Options.UseEsc && c == Options.Esc) {
        ptr = TryPeek();
        if (ptr) {
          c = Pop();
          /* This case is specifically to support a weird behavior in MySQL
             dump.  It likes its zeros to be visible for some reason. */
          if (c == '0') {
            c = '\0';
          }
          Cache.State = Byte;
          Cache.Byte = c;
          break;
        }
      }
      /* Any other non-quote character inside of quotes is just a byte. */
      if (c != Options.Quote) {
        Cache.State = Byte;
        Cache.Byte = c;
        break;
      }
      /* A quote inside of quotes might be the start of a quote-quote. */
      if (Options.UseQuoteQuote) {
        ptr = TryPeek();
        if (ptr && *ptr == Options.Quote) {
          /* This is quoted quote-quote, so we report a single byte which is
             the quote byte. */
          Skip();
          Cache.State = Byte;
          Cache.Byte = c;
          break;
        }
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
      Cache.State = EndOfField;
      Cache.Byte = 0u;
      break;
    }
    /* If we're not doing Unix line endings, a CRLF is an EOL. */
    if (!Options.UnixEol && c == '\r') {
      ptr = TryPeek();
      if (ptr && *ptr == '\n') {
        Skip();
        Cache.State = EndOfRecord;
        Cache.Byte = 0u;
        break;
      }
    }
    /* If we are doing Unix line endings, a LF is an EOL. */
    if (Options.UnixEol && c == '\n') {
      Cache.State = EndOfRecord;
      Cache.Byte = 0u;
      break;
    }
    /* Anything else is just a byte to be reported. */
    Cache.State = Byte;
    Cache.Byte = c;
    break;
  }  // forever
}
