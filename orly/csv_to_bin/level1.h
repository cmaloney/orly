/* <orly/csv_to_bin/level1.h>

   An input consumer that translates CSV level-1 syntax.

   The consumer handles quotes automatically.  It only reports quote bytes
   when they appear as escaped sequences.

   The consumer also reports end-of-field, end-of-record, and end-of-file
   markers.

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

#include <strm/in.h>

namespace Orly {

  namespace CsvToBin {

    /* An input consumer that translates CSV level-1 syntax. */
    class TLevel1 final
        : public Strm::In::TCons {
      public:

      /* Options for parsing. */
      struct TOptions final {

        /* The byte used as an end-of-field mark. */
        uint8_t Delim;

        /* The byte used as a quote mark. */
        uint8_t Quote;

        /* If true, LF is an EOL; otherwise, CRLF is an EOL. */
        bool UnixEol;

      };  // TLevel1::TOptions

      /* The default options use comma, quote, and CRLF. */
      static const TOptions DefaultOptions;

      /* Describes our position in the parse. */
      enum TState {

        /* Just a byte, no particular meaning. */
        Byte,

        /* An end-of-field marker, such as a comma. */
        EndOfField,

        /* An end-of-record marker, such as CRLF. */
        EndOfRecord,

        /* An end-of-file marker, such as a lack of any more bytes. */
        EndOfFile

      };  // TLevel1::TState

      /* The type of our cache, which we expose via the dereferencing
         operators. */
      struct TCache {

        /* See TState. */
        TState State;

        /* If State is Byte, this is the byte; otherwise, this is junk. */
        uint8_t Byte;

      };  // TLevel1::TCache

      /* Attaches to the given producer and uses the given options for
         parsing. */
      explicit TLevel1(
            Strm::In::TProd *prod, const TOptions &options = DefaultOptions)
          : TCons(prod), Options(options), IsCached(false), Quoted(false) {}

      /* Our cached state. */
      const TCache &operator*() const {
        assert(this);
        Refresh();
        return Cache;
      }

      /* Our cached state. */
      const TCache *operator->() const {
        assert(this);
        Refresh();
        return &Cache;
      }

      /* Dump our cached state and advance. */
      TLevel1 &operator++() {
        assert(this);
        Refresh();
        IsCached = false;
        return *this;
      }

      private:

      /* If our cache is fresh, do nothing; otherwise, update it. */
      void Refresh() const {
        assert(this);
        if (!IsCached) {
          const_cast<TLevel1 *>(this)->Update();
          IsCached = true;
        }
      }

      /* Update our cache. */
      void Update();

      /* Parsing options, cached at construction time. */
      const TOptions Options;

      /* See TCache.  Valid only if IsCached is true. */
      TCache Cache;

      /* If true, then Cache contains valid data. */
      mutable bool IsCached;

      /* True when we're inside of a quoted range. */
      bool Quoted;

    };  // TLevel1

  }  // Csv2Bin

}  // Orly
